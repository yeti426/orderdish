#include "init.h"

int find_item_in_cart(int dish_no);
void update_total(void);

//变量声明 
extern int table_no;                 //餐台号 
extern shopping_cart cart;    //购物车全局变量
extern char hot_dish_filename[20];
extern char cold_dish_filename[20];
extern char staple_food_filename[20];
extern char drink_filename[20]; 

//函数声明 
void cold_dish();             //凉菜 
void hot_dish();              //热菜 
void staple_food();           //主食 
void drink();                 //饮品 

void menu_controller(dish_menu* , int);       //菜单交互 
void read_menu(char* , dish_menu* , int*);    //从文件中读取菜单 
void ordering_menu(void);                     //点菜子菜单
void view_bill(void);                         //查看账单（已点菜品）
void checkout(void);                          //结账（呼叫店小二）

//外部函数声明   
extern void error_check(int,int,int*);              //输入错误检查
extern void greet(struct tm* p,int);                //问候语
extern struct tm* get_time();                       //获取系统时间
extern void clear_stdin_buffer(void);
extern void create_order_filename(int,char*,int);   //生成订单文件名  
extern void order_status(void);                         //生成订单状态
extern void display_cart(void);                         //查看已点菜品
extern void check_bill(void);                           //支付订单 


/*
 * 函数功能：顾客主界面
 */
void customer_form() {
    system("cls");
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
        system("cls");
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
            case 5: 
                printf("感谢光临，欢迎客官下次再来！\n");
                break;
        }
    } while(choice != 5);
}


/*
 * 函数功能：读取菜单文件到结构体数组
 */
void read_menu(char* filename, dish_menu* dm, int* cnt) {
    FILE* fp = fopen(filename, "r");
    int ret;

    *cnt = 0;
    if (fp == NULL) {
        printf("错误: 无法打开菜单文件 %s\n", filename);
        getch();
        return;
    }

    while (*cnt < MAX_LENGTH) {
        ret = fscanf(fp, "%d", &dm[*cnt].no);
        if (ret != 1) break;

        ret = fscanf(fp, "%s", dm[*cnt].dish_name);
        if (ret != 1) break;

        ret = fscanf(fp, "%lf", &dm[*cnt].dish_price);
        if (ret != 1) break;

        ret = fscanf(fp, "%d", &dm[*cnt].type);
        if (ret != 1) break;

        // --- 新增：读取口味选项标记 ---
        ret = fscanf(fp, "%d", &dm[*cnt].has_options);
        if (ret != 1) {
            // 兼容旧格式，如果没有这个字段，默认为 0
            dm[*cnt].has_options = 0;
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
    // 如果上一单已提交且购物车为空，重置状态以允许新点餐
    // if (cart.kitchen_received && cart.count == 0) {
    //     cart.kitchen_received = 0;
    // }

    int choice;
    do {
        system("cls");
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
            case 1: hot_dish(); break;
            case 2: cold_dish(); break;
            case 3: staple_food(); break;
            case 4: drink(); break;
        }
    } while(choice != 5);
}


/*
 * 函数功能：显示菜单并处理点菜交互
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
 * sort_type: 0-默认, 1-升序, 2-降序
 */ 
void display_menu(dish_menu *dm, int cnt, int sort_type) {
    int i;
    // 创建一个临时数组用于排序，避免修改原始菜单数据
    dish_menu temp_dm[MAX_LENGTH];
    for(int i = 0; i < cnt; i++) {
        temp_dm[i] = dm[i];
    }

    // 根据选择进行排序 (冒泡排序)
    if (sort_type == 1 || sort_type == 2) {
        for (int i = 0; i < cnt - 1; i++) {
            for (int j = 0; j < cnt - 1 - i; j++) {
                int should_swap = 0;
                if (sort_type == 1) { // 升序
                    if (temp_dm[j].dish_price > temp_dm[j+1].dish_price) should_swap = 1;
                } else if (sort_type == 2) { // 降序
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

	
	system("cls");
	printf("----------------------------------------------------------\n");
    printf("序号      菜品编号      菜品名称      价格\n");
    printf("----------------------------------------------------------\n");
    
    for(i = 0; i < cnt; i++){
        if (is_recommend(temp_dm[i].type, temp_dm[i].no)) {
            printf(" %d\t %d\t 【招牌】%s\t %.2lf\n",
                   i + 1, temp_dm[i].no, temp_dm[i].dish_name, temp_dm[i].dish_price);
        } else {
            printf(" %d\t %d\t %s\t %.2lf\n",
                   i + 1, temp_dm[i].no, temp_dm[i].dish_name, temp_dm[i].dish_price);
        }
    }
	printf("----------------------------------------------------------\n");
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

    // ===============================
    // ① 默认顺序显示
    // ===============================
    display_menu(dm, cnt, sort_type);

    // ===============================
    // ② 是否改变显示顺序
    // ===============================
    printf("是否改变本类菜品显示顺序？(1.是 2.否)：");
    int change_order;
    scanf("%d", &change_order);
    error_check(1, 2, &change_order);

    if (change_order == 1) {
        system("cls");
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

    // ===============================
    // ③ 然后才开始点菜
    // ===============================
    do {
        printf("请输入要点的菜品序号(0返回)：");
        scanf("%d", &choice);

        if (choice == 0) return;
        if (choice < 1 || choice > cnt) {
            printf("无效序号！\n");
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

        // ===============================
        // 加入购物车
        // ===============================
        dish_menu selected = dm[choice - 1];
        int idx = find_item_in_cart(selected.no);

        if (idx != -1) {
            cart.items[idx].nums += nums;
        } else {
            cart.items[cart.count].no = selected.no;
            strcpy(cart.items[cart.count].dish_name, selected.dish_name);
            cart.items[cart.count].dish_price = selected.dish_price;
            cart.items[cart.count].type = selected.type;
            cart.items[cart.count].nums = nums;
            cart.items[cart.count].status = DISH_STATUS_PENDING;
            cart.count++;
        }

        update_total();
        printf("已添加：%s × %d\n", selected.dish_name, nums);
        getch();

        printf("是否继续点菜？ 1.是 2.否：");
        scanf("%d", &continue_choice);
        error_check(1, 2, &continue_choice);

    } while (continue_choice == 1);
}


/*
 * 函数功能：查看已点菜品（显示所有已点菜品）
 */
void view_bill() {
    char filename[50] = "order//";
    create_order_filename(table_no, filename, sizeof(filename));
    
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        system("cls");
        printf("========================================\n");
        printf("         账单 (雅座: %d)\n", table_no);
        printf("========================================\n");
        printf("\n您还没有点菜！\n");
        printf("\n========================================\n");
        printf("1. 去点菜\n");
        printf("2. 返回主菜单\n");
        printf("请选择: ");
        
        int choice;
        scanf("%d", &choice);
        error_check(1, 2, &choice);
        
        if (choice == 1) {
            ordering_menu();
        }
        return;
    }

    // 读状态行（整行，含换行）
    char status_line[10];
    if (fgets(status_line, sizeof(status_line), fp) == NULL) {
        fclose(fp);
        printf("订单文件格式错误：缺失状态行！\n");
        getch();
        return;
    }

    // 读备注行（整行）
    char remark[200];
    if (fgets(remark, sizeof(remark), fp) == NULL) {
        strcpy(remark, "无备注");
    } else {
        // 去掉末尾换行（如果存在）
        size_t len = strlen(remark);
        if (len > 0 && remark[len - 1] == '\n') {
            remark[len - 1] = '\0';
        }
    }
    
    cart_item orders[MAX_LENGTH]; // 改用 cart_item
    int count = 0;
    double total = 0;
    
    // 读取所有订单项（现在是7个字段，包含菜品备注）
    while (count < MAX_LENGTH &&
           fscanf(fp, "%d %s %lf %d %d %d %[^\n]",
                  &orders[count].no,
                  orders[count].dish_name,
                  &orders[count].dish_price,
                  &orders[count].type,
                  &orders[count].nums,
                  &orders[count].status,
                  orders[count].remark) >= 6) {   
        // 兼容旧格式：如果没读到备注，给个默认值
        if (strlen(orders[count].remark) == 0) {
            strcpy(orders[count].remark, "正常");
        }
        total += orders[count].dish_price * orders[count].nums;
        count++;
    }
    fclose(fp);
    
    system("cls");
    printf("========================================\n");
    printf("         膳单 (雅座: %d)\n", table_no);
    printf("订单备注: %s", remark); // remark里包含换行符，所以不用再加\n
    printf("========================================\n");
    printf("\n");

    if (count == 0) {
        printf("暂无已点菜品\n");
    } else {
        printf("%-4s %-6s %-10s %-8s %-6s %-10s %-8s %-10s\n",
               "序号", "编号", "菜品名称", "单价", "数量", "小计", "状态", "口味");
        printf("-----------------------------------------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            double subtotal = orders[i].dish_price * orders[i].nums;
            const char* status_str = (orders[i].status == DISH_STATUS_DONE) ? "已完成" : "制作中";
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf %-8s [%s]\n",
                   i + 1,
                   orders[i].no,
                   orders[i].dish_name,
                   orders[i].dish_price,
                   orders[i].nums,
                   subtotal,
                   status_str,
                   orders[i].remark);
        }
        
        printf("-----------------------------------------------------------------------\n");
        printf("\n总金额: %.2lf 元\n", total);
    }
    
    printf("\n========================================\n");
    printf("1. 去结账\n");
    printf("2. 继续点菜\n");
    printf("3. 返回主菜单\n");
    printf("请选择: ");
    
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
    char filename[50] = "order//";
    create_order_filename(table_no, filename, sizeof(filename));
    
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        system("cls");
        printf("========================================\n");
        printf("         结账 (雅座: %d)\n", table_no);
        printf("========================================\n");
        printf("\n您还没有点菜，无法结账！\n");
        printf("\n按任意键返回...");
        getch();
        return;
    }
    
    // 读状态行
    char status_line[10];
    if (fgets(status_line, sizeof(status_line), fp) == NULL) {
        fclose(fp);
        printf("订单文件格式错误：缺失状态行！\n");
        getch();
        return;
    }

    // 读备注行
    char remark[200];
    if (fgets(remark, sizeof(remark), fp) == NULL) {
        strcpy(remark, "无备注");
    } else {
        size_t len = strlen(remark);
        if (len > 0 && remark[len - 1] == '\n') {
            remark[len - 1] = '\0';
        }
    }

    cart_item orders[MAX_LENGTH]; // 改用 cart_item
    int count = 0;
    double total = 0;
    
    // 读取所有订单项（7个字段）
     while (count < MAX_LENGTH) {
        int ret = fscanf(fp, "%d %s %lf %d %d %d %[^\n]",
                  &orders[count].no,
                  orders[count].dish_name,
                  &orders[count].dish_price,
                  &orders[count].type,
                  &orders[count].nums,
                  &orders[count].status,
                  orders[count].remark); // ← 新增：读取状态
        if (ret < 6) break;             

        // 兼容旧格式
        if (strlen(orders[count].remark) == 0) {
            strcpy(orders[count].remark, "正常");
        }

        total += orders[count].dish_price * orders[count].nums;
        count++;
    }
    fclose(fp);
    
    system("cls");
    printf("========================================\n");
    printf("         结账 (雅座: %d)\n", table_no);
    printf("========================================\n");
    printf("\n");
    
    if (count == 0) {
        printf("账单为空！\n");
    } else {
        printf("%-4s %-6s %-10s %-8s %-6s %-10s %-8s %-10s\n", 
               "序号", "编号", "菜品名称", "单价", "数量", "小计", "状态", "口味");
        printf("-----------------------------------------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            double subtotal = orders[i].dish_price * orders[i].nums;
            const char* status_str = (orders[i].status == DISH_STATUS_DONE) ? "已完成" : "制作中";
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf %-8s [%s]\n",
                   i + 1,
                   orders[i].no,
                   orders[i].dish_name,
                   orders[i].dish_price,
                   orders[i].nums,
                   subtotal,
                   status_str,
                   orders[i].remark);
        }
        
        printf("-----------------------------------------------------------------------\n");
        printf("\n总金额: %.2lf 元\n", total);
    }
    
    printf("\n========================================\n");
    printf("已呼叫店小二，请稍候...\n");
    printf("店小二将为您办理结账手续\n");
    printf("\n按任意键返回主菜单...");
    getch();
    check_bill();  // 更新订单状态为已支付
}
