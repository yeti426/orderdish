#include"init.h"   // 引入初始化模块

// 定义全局变量 table_no
int table_no = 0;

// 外部函数声明
extern int main_form();      // 主界面函数
extern void customer_form(); // 顾客界面函数
extern void admin_form();    // 管理员界面函数
extern void kitchen_form();  // 厨师界面函数

/*
 * 函数功能: 主函数，程序入口，实现基本菜单循环
 */
int main(){
	// 设置控制台输出为UTF-8，确保中文显示正常
    #ifdef _WIN32
        system("chcp 65001 >nul");
        SetConsoleOutputCP(65001);
    #endif

	int op;
	do{
		op = main_form();           // 显示主界面并获取选择信息
		switch(op){
			case 1:customer_form(); // 顾客界面
			       break;
			case 2:admin_form();    // 管理员界面
			       break;
			case 3:kitchen_form();  // 厨师界面
			       break;
		}
	}while(op!=4);                  // 退出
	
	return 0;
} 