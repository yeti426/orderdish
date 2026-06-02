#include"init.h"   //引入初始化模块

//外部函数声明
extern int main_form();
extern void customer_form();
extern void admin_form();
extern void kitchen_form(); // ← 新增：厨房端函数声明

/*
 * 函数功能: 主函数，程序入口
 */
int main(){
	int op;
	do{
		op = main_form();           //显示主界面并获取选择信息
		switch(op){
			case 1:customer_form(); //顾客界面
			       break;
			case 2:admin_form();    //管理员界面
			       break;
			case 3:kitchen_form();  //厨房界面 ← 新增
			       break;
		}
	}while(op!=4); //op=4 退出 ← 修改：因为现在退出是选项4
	
	return 0;
} 