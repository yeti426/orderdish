#include "init.h"

// 变量声明 
extern int table_no;           // 餐台号 
extern shopping_cart cart;     // 购物车全局变量
extern char hot_dish_filename[20];
extern char cold_dish_filename[20];
extern char staple_food_filename[20];
extern char drink_filename[20]; 

// 函数声明 
void cold_dish();             // 凉菜 
void hot_dish();              // 热菜 
void staple_food();           // 主食 
void drink();                 // 饮品 

void menu_controller(dish_menu* , int);       // 菜单交互 
void read_menu(char*, dish_menu*, int*);      // 从文件中读取菜单 
void ordering_menu(void);                     // 点菜子菜单
double display_order_common(int, int);        // 显示订单信息（通用函数）
void view_bill(void);                         // 查看账单（已点菜品）
void checkout(void);                          // 结账（呼叫店小二）

// 外部函数声明   
extern void error_check(int,int,int*);              // 输入错误检查
extern void greet(struct tm* p,int);                // 问候语
extern struct tm* get_time();                       // 获取系统时间
extern void clear_stdin_buffer(void);               // 清空输入缓冲区
extern void create_order_filename(int,char*,int);   // 生成订单文件名  
extern void order_status(void);                     // 生成订单状态
extern void display_cart(void);                     // 查看已点菜品
extern void check_bill(void);                       // 支付订单 
extern int find_item_in_cart(char*, char*);         // 在购物车中查找菜品
extern void update_total(void);                     // 更新购物车总金额


/*
 * 函数功能：顾客主界面
 */
void customer_form() {
    CLEAR_SCREEN();
    int greet_type = 1;
	struct tm* p = get_time();
	greet(p,greet_type);           // 根据时间和身份，打印问候语
    
    // 选桌
    printf("**************************************\n");
    printf("请入坐雅间雅座：");

    // 输入验证，确保整数
    while (scanf("%d", &table_no) != 1) {
        while (getchar() != '\n'); // 清空输入缓冲区
        printf("雅座有误，请重报雅座编号：");
    }
    
    init_cart(table_no);           // 初始化购物车（绑定雅座）
    
    // 选择服务
    int choice;
    do {
        CLEAR_SCREEN();
        printf("========================================\n");
        printf("         雅座：%d 客官膳务堂\n", table_no);
        printf("========================================\n");
        printf("1. 点菜\n");
        printf("2. 选膳筐（购物车）\n");
        printf("3. 览已点佳肴\n");
        printf("4. 结付膳单（呼叫店小二）\n");
        printf("5. 退出\n");
        printf("========================================\n");
        printf("请选择: ");
        scanf("%d", &choice);
        
        error_check(1, 5, &choice);
        
        switch(choice) {
            case 1: 
                ordering_menu();  // 点菜
                break;
            case 2: 
                display_cart();   // 查看购物车
                break;
            case 3: 
                view_bill();      // 查看账单（已提交订单）
                break;
            case 4: 
                checkout();       // 呼叫结账
                break;
        }
    } while(choice != 5);
}


/*
 * 函数功能：读取菜单文件到结构体数组
 */
void read_menu(char* filename, dish_menu* dm, int* cnt) {
    FILE* fp = fopen(filename, "r");
    *cnt = 0;

    if (fp == NULL) {
        printf("错误: 无法打开菜单文件 %s\n", filename);
        getch();
        return;
    }

    while (*cnt < MAX_LENGTH) {
        // fscanf 返回成功读取的项数，如果txt最后一行有空格或回车，这能防止多读一行空数据
        if (fscanf(fp, "%d", &dm[*cnt].no) != 1) break; // 读编号
        fscanf(fp, "%s", dm[*cnt].dish_name);           // 读名称
        fscanf(fp, "%lf", &dm[*cnt].dish_price);        // 读价格
        fscanf(fp, "%d", &dm[*cnt].type);               // 读分类

        // 读取是否有辣度等口味选项
        if (fscanf(fp, "%d", &dm[*cnt].has_options) != 1) {
            dm[*cnt].has_options = 0;   // 默认没有口味选项
        }
        (*cnt)++;
    }
    fclose(fp);
}

////////////////////////////////////////////////////////////////////

/*
 * 函数功能：点菜子菜单（整合四类菜品）
 */
void ordering_menu() {

    int choice;
    do {
        CLEAR_SCREEN();
        printf("========================================\n");
        printf("         点菜菜单\n");
        printf("========================================\n");
        printf("1. 炉上热馔\n"); //热菜
        printf("2. 清润冷碟\n"); //凉菜
        printf("3. 五谷主食\n"); //主食
        printf("4. 琼浆茶饮\n"); //饮品
        printf("5. 返回主菜单\n");
        printf("========================================\n");
        printf("请选择: ");
        scanf("%d", &choice);
        
        error_check(1, 5, &choice);
        
        switch(choice) {
            case 1: hot_dish();
                    break;
            case 2: cold_dish(); 
                    break;
            case 3: staple_food(); 
                    break;
            case 4: drink(); 
                    break;
        }
    } while(choice != 5);
}


/*
 * 函数功能：模块化菜单并处理点菜交互
 */
void cold_dish(){
	//采用结构体数组保存读取到的菜单品信息
	dish_menu cold_dish_menu[MAX_LENGTH];
	//设置一个计数器保存菜品实际数量 
	int cnt = 0;
	//打开 cold_dish.txt 并读取数据进行显示
	read_menu(cold_dish_filename,cold_dish_menu,&cnt);
	//调用菜单交互函数，处理点菜逻辑
	menu_controller(cold_dish_menu,cnt); 
}
void hot_dish(){
	dish_menu hot_dish_menu[MAX_LENGTH]; 
	int cnt = 0; 
	read_menu(hot_dish_filename,hot_dish_menu,&cnt);
	menu_controller(hot_dish_menu,cnt); 
}
void staple_food(){
	dish_menu staple_food_menu[MAX_LENGTH]; 
	int cnt = 0; 
	read_menu(staple_food_filename,staple_food_menu,&cnt);
	menu_controller(staple_food_menu,cnt); 
}
void drink(){
	dish_menu drink_menu[MAX_LENGTH]; 
	int cnt = 0; 
	read_menu(drink_filename,drink_menu,&cnt);
	menu_controller(drink_menu,cnt); 
}


/*
 * 函数功能：显示菜单信息（支持排序）
 */ 
void display_menu(dish_menu *dm, int cnt, int sort_type) {
    // 创建一个临时数组用于排序
    dish_menu temp_dm[MAX_LENGTH];
    for(int i = 0; i < cnt; i++) {
        temp_dm[i] = dm[i];
    }

    // 根据选择进行排序 (冒泡排序)
    if (sort_type == 1 || sort_type == 2) {
        for (int i = 0; i < cnt - 1; i++) {
            for (int j = 0; j < cnt - 1 - i; j++) {
                int should_swap = 0;
                if (sort_type == 1) {         // 升序
                    if (temp_dm[j].dish_price > temp_dm[j+1].dish_price) should_swap = 1;
                } else if (sort_type == 2) {  // 降序
                    if (temp_dm[j].dish_price < temp_dm[j+1].dish_price) should_swap = 1;
                }
                if (should_swap) {
                    dish_menu temp = temp_dm[j];
                    temp_dm[j] = temp_dm[j+1];
                    temp_dm[j+1] = temp;
                }
            }
        }
    }

	
	CLEAR_SCREEN();
	printf("------------------------------------------------------------\n");
    printf("%-10s%-20s%s\n", "菜品编号", "菜品名称", "价格");
    printf("------------------------------------------------------------\n");
    
    for(int i = 0; i < cnt; i++){
        // 拼接显示名：招牌前缀 + 菜名 + 选后缀
        char display_name[40] = "";
        if (is_recommend(temp_dm[i].type, temp_dm[i].no)) {
            strcat(display_name, "【招牌】");
        }
        strcat(display_name, temp_dm[i].dish_name);
        if (temp_dm[i].has_options) {
            strcat(display_name, "[选]");
        }
        printf("%-10d%-20s%.2lf\n",
               temp_dm[i].no, display_name, temp_dm[i].dish_price);
    }
	printf("------------------------------------------------------------\n");
}


/*
 * 函数功能：菜单交互（显示菜单、处理点菜逻辑）
 */
void menu_controller(dish_menu* dm, int cnt) {
    if (dm == NULL || cnt <= 0) {
        printf("当前分类没有可用菜单。\n");
        getch();
        return;
    }

    int sort_type = 0; // 0=默认 1=降序 2=升序
    int choice, nums, continue_choice;

    // 默认顺序菜单
    display_menu(dm, cnt, sort_type);

    printf("是否改变本类菜品显示顺序？(1.是 2.否)：");
    int change_order;
    scanf("%d", &change_order);
    error_check(1, 2, &change_order);

    if (change_order == 1) {
        CLEAR_SCREEN();
        printf("========================================\n");
        printf("       选择排序方式（仅影响显示）\n");
        printf("========================================\n");
        printf("1. 价格升序 (低 → 高)\n");
        printf("2. 价格降序 (高 → 低)\n");
        printf("========================================\n");
        printf("请选择: ");
        scanf("%d", &sort_type);
        error_check(1, 2, &sort_type);

        // 立即重新显示（不跳菜单）
        display_menu(dm, cnt, sort_type);
    }

    // 点菜
    do {
        printf("请输入要点的菜品编号(0返回)：");
        scanf("%d", &choice);

        if (choice == 0) return;

        if (choice < 1 || choice > cnt) {
            printf("无效编号！\n");
            getch();
            continue;
        }

        printf("请输入点菜数量：");
        scanf("%d", &nums);

        if (nums <= 0) {
            printf("份数需至少一份\n");
            getch();
            continue;
        }

        // 加入购物车
        dish_menu selected = dm[choice - 1];

        // 辣度选择（仅对有口味选项的菜品）
        char spicy_remark[50] = ""; // 默认备注为空
        if (selected.has_options) {
            printf("\n===== 请选择辣度 =====\n");
            printf("1. 不辣\n");
            printf("2. 微辣\n");
            printf("3. 中辣\n");
            printf("4. 特辣\n");
            printf("======================\n");
            printf("请选择 (1-4): ");
            int spicy_choice;
            scanf("%d", &spicy_choice);
            error_check(1, 4, &spicy_choice);
            switch(spicy_choice) {
                case 1: strcpy(spicy_remark, "不辣"); 
                        break;
                case 2: strcpy(spicy_remark, "微辣"); 
                        break;
                case 3: strcpy(spicy_remark, "中辣"); 
                        break;
                case 4: strcpy(spicy_remark, "特辣"); 
                        break;
            }
        }

        // 查找购物车中是否已有同编号菜品
        int idx = find_item_in_cart(selected.dish_name, spicy_remark);

        if (idx != -1) {
            // 如果已有同编号菜品，检查辣度是否一致，一致则累加数量
            if (strcmp(cart.items[idx].remark, spicy_remark) == 0) {
                cart.items[idx].nums += nums;
            } else {
                // 辣度不同，作为新菜品添加
                cart.items[cart.count].no = selected.no;
                strcpy(cart.items[cart.count].dish_name, selected.dish_name);
                cart.items[cart.count].dish_price = selected.dish_price;
                cart.items[cart.count].type = selected.type;
                cart.items[cart.count].nums = nums;
                cart.items[cart.count].status = ORDER_STATUS_PENDING;
                strcpy(cart.items[cart.count].remark, spicy_remark);
                cart.count++;
            }
        } else {
            cart.items[cart.count].no = selected.no;
            strcpy(cart.items[cart.count].dish_name, selected.dish_name);
            cart.items[cart.count].dish_price = selected.dish_price;
            cart.items[cart.count].type = selected.type;
            cart.items[cart.count].nums = nums;
            cart.items[cart.count].status = ORDER_STATUS_PENDING;
            strcpy(cart.items[cart.count].remark, spicy_remark);
            cart.count++;
        }

        // 更新总金额
        update_total();

        if (selected.has_options) {
            printf("已添加：%s × %d (%s)\n", selected.dish_name, nums, spicy_remark);
        } else {
            printf("已添加：%s × %d\n", selected.dish_name, nums);
        }
        getch();

        printf("是否继续点菜？ 1.是 2.否：");
        scanf("%d", &continue_choice);
        error_check(1, 2, &continue_choice);

    } while (continue_choice == 1);
}


/*
 * 函数功能：显示订单信息（通用函数）
 */
double display_order_common(int t_no, int show_menu) {
    char filename[50] = "order//";
    create_order_filename(t_no, filename, sizeof(filename));

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        if (show_menu) {
            CLEAR_SCREEN();
            printf("========================================\n");
            printf("         膳单 (雅座: %d)\n", t_no);
            printf("========================================\n");
            printf("\n您还没有点菜！\n");
            printf("\n========================================\n");
            printf("1. 去点菜\n2. 返回主菜单\n请选择: ");
        }
        return -1.0;
    }

    cart_item orders[MAX_LENGTH];
    int count = 0;
    double total = 0.0;

    // 循环读取订单项
    while (count < MAX_LENGTH) {
        int ret = fscanf(fp, "%d %s %lf %d %d %d %s",
                        &orders[count].no,
                        orders[count].dish_name,
                        &orders[count].dish_price,
                        &orders[count].type,
                        &orders[count].nums,
                        &orders[count].status,
                        orders[count].remark);

        // 兼容旧格式
        if (ret == 6) {
            strcpy(orders[count].remark, "-");
        } else if (ret < 6) {
            break;
        }
        // 给默认值
        if (strlen(orders[count].remark) == 0) {
            strcpy(orders[count].remark, "-");
        }

        total += orders[count].dish_price * orders[count].nums;
        count++;
    }
    fclose(fp);

    CLEAR_SCREEN();
    printf("========================================\n");
    printf("         膳单 (雅座: %d)\n", t_no);
    printf("========================================\n");
    printf("\n");

    if (count == 0) {
        printf("暂无已点菜品\n");
    } else {
        printf("%-6s %-18s %-10s %-8s %-12s %-10s %-10s\n",
               "编号", "菜品名称", "单价", "数量", "小计", "状态", "口味");
        printf("-----------------------------------------------------------------------\n");

        for (int i = 0; i < count; i++) {
            double subtotal = orders[i].dish_price * orders[i].nums;
            const char* status_str = (orders[i].status == STATUS_DONE) ? "已完成" : "制作中";

            printf("%-6d %-18s %-10.2lf %-8d %-12.2lf %-10s [%s]\n",
                   i + 1,
                   orders[i].dish_name,
                   orders[i].dish_price,
                   orders[i].nums,
                   subtotal,
                   status_str,
                   orders[i].remark);
        }

        printf("----------------------------------------\n");
        printf("\n总金额: ¥%.2lf\n", total);
    }

    if (show_menu) {
        printf("\n========================================\n");
        printf("1. 去结账\n");
        printf("2. 继续点菜\n");
        printf("3. 返回主菜单\n");
        printf("请选择: ");
    }

    return total;
}


/*
 * 函数功能：查看账单
 */
void view_bill() {
    double total = display_order_common(table_no, 1); // show_menu = 1

    if (total < 0.0) {
        int choice;
        scanf("%d", &choice);
        error_check(1, 2, &choice);
        if (choice == 1) ordering_menu();
        return;
    }

    int choice;
    scanf("%d", &choice);
    error_check(1, 3, &choice);

    switch(choice) {  
        case 1:  
            checkout();       // 去结账  
                break;
        case 2:  
            ordering_menu();  // 继续点菜  
                break;
        case 3:  
            break;
    }  
}
  
  
  
/*  
 * 函数功能：结账（呼叫店小二）  
 */  
void checkout() {
    double total = display_order_common(table_no, 0); // show_menu = 0

    if (total < 0.0) {
        printf("\n按任意键返回...");
        getch();
        return;
    }

    printf("\n========================================\n");
    printf("已呼叫店小二，请稍候...\n");
    printf("店小二将为您办理结账手续\n");
    printf("\n按任意键返回主菜单...");
    getch();

    check_bill();
}
