#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define MAX_LENGTH 100

// 菜单结构体
typedef struct {
    int no;
    char dish_name[20];
    double dish_price;
    int type;
} dish_menu;

// 订单结构体
typedef struct {
    int no;
    char dish_name[20];
    double dish_price;
    int type;
    int nums;
} dish_order;

// 订单模块（C 同学）
void create_order_filename(int, char*);
void check_bill();
void order_status();
void calculate_value(char*, double*, double*, double*, double*, double*);
void order_check();
void order_complete();

#endif