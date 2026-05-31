#include"init.h" //引入初始化模块

//外部函数声明
extern int main_form();
extern void customer_form();
extern void admin_form();

/*
* 名称: main
* 功能: 程序入口，构建循环运行逻辑 
*/
int main(){
	int op; //定义一个选择变量
	do{
		op = main_form();
		switch(op){
			case 1:customer_form();
			       break;
			case 2:admin_form();
			       break;
		}
	}while(op!=3); //菜单循环 
	
	return 0;
} 