#include"init.h"   //引入初始化模块

//外部函数声明
extern int main_form();
extern void customer_form();
extern void admin_form();

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
		}
	}while(op!=3); //op=3 退出
	
	return 0;
} 