#include "tools.c"

//变量声明 
int table_no;                 //餐台号 
extern char hot_dish_filename[20];
extern char cold_dish_filename[20];
extern char staple_food_filename[20];
extern char drink_filename[20]; 
//函数声明 
int customer_menu();          //菜单界面
void cold_dish();             //凉菜 
void hot_dish();              //热菜 
void staple_food();           //主食 
void drink();                 //饮品 
void over_view();             //总览 
void check_bill();            //支付订单 
void order_status();          //订单状态
void page_controller(dish_menu* , int);       //菜单分页 
int display_menu(dish_menu* , int , int);     //显示菜单信息
void create_order(dish_menu* , int ,int);     //生成订单 
void read_menu(char* , dish_menu* , int*);             //从文件中读取菜单 

//外部函数声明   
extern void error_check(int,int,int*);
extern void greet(struct tm* p,int);
extern struct tm* get_time();
extern void create_order_filename(int,char*); 
// 添加以下函数声明
extern void ordering_menu();          // 点菜子菜单
extern void view_bill();              // 查看账单
extern void checkout();               // 结账

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

// 购物车全局变量
shopping_cart cart;

// 函数声明
void init_cart();                              // 初始化购物车
void add_to_cart(dish_menu* dm, int index, int nums);  // 添加到购物车
void display_cart();                           // 显示购物车
void remove_from_cart(int index);              // 从购物车删除
void update_cart_total();                      // 更新总金额
void submit_order();                           // 提交订单到厨房

/*
 * 函数功能：初始化购物车
 */
void init_cart() {
    cart.count = 0;
    cart.total_amount = 0;
    cart.kitchen_received = 0;
    cart.table_no = table_no;
    
    // 清空购物车数组
    for (int i = 0; i < MAX_LENGTH; i++) {
        cart.items[i].no = 0;
        cart.items[i].nums = 0;
        cart.items[i].subtotal = 0;
        cart.items[i].status = 0;
        memset(cart.items[i].dish_name, 0, sizeof(cart.items[i].dish_name));
    }
}

/*
 * 函数功能：添加菜品到购物车
 * 参数：dm - 菜单数组, index - 菜品索引, nums - 数量
 */
void add_to_cart(dish_menu* dm, int index, int nums) {
    if (cart.count >= MAX_LENGTH) {
        printf("购物车已满！\n");
        getch();
        return;
    }
    
    // 检查是否已存在该菜品
    for (int i = 0; i < cart.count; i++) {
        if (cart.items[i].no == dm[index].no) {
            // 已存在，增加数量
            cart.items[i].nums += nums;
            cart.items[i].subtotal = cart.items[i].dish_price * cart.items[i].nums;
            printf("已增加 %s 的数量！\n", dm[index].dish_name);
            update_cart_total();
            getch();
            return;
        }
    }
    
    // 不存在，添加新项
    cart.items[cart.count].no = dm[index].no;
    strcpy(cart.items[cart.count].dish_name, dm[index].dish_name);
    cart.items[cart.count].dish_price = dm[index].dish_price;
    cart.items[cart.count].type = dm[index].type;
    cart.items[cart.count].nums = nums;
    cart.items[cart.count].subtotal = dm[index].dish_price * nums;
    cart.items[cart.count].status = 0;  // 未提交状态
    
    cart.count++;
    update_cart_total();
    
    printf("已将 %s 加入购物车！\n", dm[index].dish_name);
    getch();
}


/*
 * 函数功能：更新购物车总金额
 */
void update_cart_total() {
    cart.total_amount = 0;
    for (int i = 0; i < cart.count; i++) {
        cart.total_amount += cart.items[i].subtotal;
    }
}

/*
 * 函数功能：显示购物车内容
 */
void display_cart() {
    system("cls");
    printf("========================================\n");
    printf("         购物车 (桌号: %d)\n", cart.table_no);
    printf("========================================\n");
    
    if (cart.count == 0) {
        printf("\n购物车为空！\n\n");
    } else {
        printf("\n%-4s %-6s %-10s %-8s %-6s %-10s %-8s\n", 
               "序号", "编号", "菜品名称", "单价", "数量", "小计", "状态");
        printf("----------------------------------------------------------\n");
        
        for (int i = 0; i < cart.count; i++) {
            char* status_text;
            switch(cart.items[i].status) {
                case 0: status_text = "未提交"; break;
                case 1: status_text = "制作中"; break;
                case 2: status_text = "已上菜"; break;
                default: status_text = "未知"; break;
            }
            
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf %-8s\n",
                   i + 1,
                   cart.items[i].no,
                   cart.items[i].dish_name,
                   cart.items[i].dish_price,
                   cart.items[i].nums,
                   cart.items[i].subtotal,
                   status_text);
        }
        
        printf("----------------------------------------------------------\n");
        printf("厨房状态: %s\n", cart.kitchen_received ? "已收到订单" : "未收到订单");
        printf("\n总金额: %.2lf 元\n", cart.total_amount);
    }
    
    printf("\n========================================\n");
    
    // 如果购物车为空或已提交，只显示返回选项
    if (cart.count == 0 || cart.kitchen_received) {
        printf("1. 返回主菜单\n");
        printf("请选择: ");
        
        int choice;
        scanf("%d", &choice);
        error_check(1, 1, &choice);
        return;
    }
    
    // 否则显示完整选项
    printf("1. 删除菜品\n");
    printf("2. 提交订单到厨房\n");
    printf("3. 返回主菜单\n");
    printf("请选择: ");
    
    int choice;
    scanf("%d", &choice);
    error_check(1, 3, &choice);
    
    switch(choice) {
        case 1: {
            printf("请输入要删除的序号: ");
            int del_index;
            scanf("%d", &del_index);
            
            if (del_index < 1 || del_index > cart.count) {
                printf("无效的序号！\n");
                getch();
                break;
            }
            
            // 确认删除
            printf("确认删除 %s 吗？(1.是 2.否): ", 
                   cart.items[del_index - 1].dish_name);
            int confirm;
            scanf("%d", &confirm);
            
            if (confirm == 1) {
                remove_from_cart(del_index - 1);
                printf("删除成功！\n");
            }
            getch();
            break;
        }
        case 2: {
            submit_order();  // 提交后会显示已点菜品
            break;
        }
        case 3:
            break;
    }
}

/*
 * 函数功能：显示已提交的订单菜品（从文件读取）
 */
void display_ordered_dishes(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("无法读取订单信息！\n");
        getch();
        return;
    }
    
    int order_status;
    fscanf(fp, "%d", &order_status);
    
    dish_order orders[MAX_LENGTH];
    int count = 0;
    double total = 0;
    
    // 读取所有订单项
    while (count < MAX_LENGTH && 
           fscanf(fp, "%d %s %lf %d %d",
                  &orders[count].no,
                  orders[count].dish_name,
                  &orders[count].dish_price,
                  &orders[count].type,
                  &orders[count].nums) == 5) {
        
        orders[count].subtotal = orders[count].dish_price * orders[count].nums;
        total += orders[count].subtotal;
        count++;
    }
    fclose(fp);
    
    // 获取订单状态文字
    char* status_text;
    switch(order_status) {
        case 1: status_text = "待支付"; break;
        case 2: status_text = "已支付，等待确认"; break;
        case 3: status_text = "商家已确认，制作中"; break;
        default: status_text = "未知状态"; break;
    }
    
    system("cls");
    printf("========================================\n");
    printf("         已点菜品列表 (桌号: %d)\n", table_no);
    printf("========================================\n");
    printf("订单状态: %s\n", status_text);
    printf("厨房状态: 已收到订单\n");
    printf("\n");
    
    if (count == 0) {
        printf("暂无已点菜品\n");
    } else {
        printf("%-4s %-6s %-10s %-8s %-6s %-10s\n", 
               "序号", "编号", "菜品名称", "单价", "数量", "小计");
        printf("----------------------------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf\n",
                   i + 1,
                   orders[i].no,
                   orders[i].dish_name,
                   orders[i].dish_price,
                   orders[i].nums,
                   orders[i].subtotal);
        }
        
        printf("----------------------------------------------------------\n");
        printf("\n总金额: %.2lf 元\n", total);
    }
    
    printf("\n========================================\n");
    printf("提示：您可以继续点菜，新菜品将加入新的订单\n");
    printf("按任意键返回主菜单...");
    getch();
}
/*
 * 函数功能：提交订单到厨房（写入文件）
 */
/*
 * 函数功能：提交订单到厨房（写入文件并清空购物车）
 */
void submit_order() {
    if (cart.count == 0) {
        printf("购物车为空，无法提交！\n");
        getch();
        return;
    }
    
    if (cart.kitchen_received) {
        printf("订单已提交到厨房，请勿重复提交！\n");
        getch();
        return;
    }
    
    char filename[50] = "order//";
    create_order_filename(table_no, filename);
    
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("订单提交失败！\n");
        getch();
        return;
    }
    
    // 写入订单状态：1-待支付
    fprintf(fp, "1\n");
    
    // 写入所有购物车项
    for (int i = 0; i < cart.count; i++) {
        fprintf(fp, "%d %s %.2lf %d %d\n",
                cart.items[i].no,
                cart.items[i].dish_name,
                cart.items[i].dish_price,
                cart.items[i].type,
                cart.items[i].nums);
    }
    
    fclose(fp);
    
    // 标记厨房已收到
    cart.kitchen_received = 1;
    
    system("cls");
    printf("========================================\n");
    printf("         订单提交成功！\n");
    printf("========================================\n");
    printf("\n订单已提交到厨房！\n");
    printf("总金额: %.2lf 元\n", cart.total_amount);
    printf("\n正在为您查看已点菜品...\n");
    Sleep(1500);  // 暂停1.5秒让用户看到提示
    
    // 清空购物车
    init_cart();
    
    // 显示已点菜品（从文件读取）
    display_ordered_dishes(filename);
}