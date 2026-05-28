//声明全局变量以及用到的头文件
#ifndef __init__
#define __init__

#include<stdio.h>   //实现菜单显示与用户交互 
#include<stdlib.h>  //系统命令调用与安全退出 
#include<math.h>
#include<windows.h> //进行清屏与控制台操作 
#include<io.h>
#include<string.h>  //菜品名称与用户信息的字符串处理 
#include<conio.h>
#include<time.h>    //获取系统时间并实现问候 

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

#endif