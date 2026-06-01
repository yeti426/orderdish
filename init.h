#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define MAX_LENGTH 100

//菜单数据结构
typedef struct a{
	int no; //编号
	char dish_name[20]; //菜名
	double dish_price; //价格
	int type; //种类：1-热菜、2-凉菜、3-主食、4-饮品 
}dish_menu;

//顾客点餐订单文件中的两部分内容：
//1.订单状态：1-订单生成、2-支付成功、3-商家已确认
//2.数据结构中的内容

//顾客点菜订单的数据结构 
typedef struct b{
	int no;
	char dish_name[20];
	double dish_price;
	int type;
	int nums; //点餐时的数量 
}dish_order;

// 购物车项结构
typedef struct {
    int no;              // 菜品编号
    char dish_name[20];  // 菜品名称
    double dish_price;   // 单价
    int type;            // 类型
    int nums;            // 数量
    double subtotal;     // 小计价格
    int status;          // 状态：0-未提交, 1-制作中, 2-已上菜
} cart_item;

// 购物车结构
typedef struct {
    cart_item items[MAX_LENGTH];  // 购物车项数组
    int count;                     // 当前菜品数量
    double total_amount;           // 总金额
    int table_no;                  // 桌号
    int kitchen_received;          // 厨房是否收到：0-未收到, 1-已收到
} shopping_cart;


// 订单模块（C 同学）
void create_order_filename(int table_no, char* fstr, int size);
void check_bill();
void order_status();
void calculate_value(char*, double*, double*, double*, double*, double*);
void order_check();
void order_complete();
void record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink);


#endif