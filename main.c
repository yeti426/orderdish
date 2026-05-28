#include"tools.c" //引入工具模块

//外部函数声明
extern int main_form();
extern void customer_form();
extern void admin_form();

/*
* function_name: main
* description  : 程序入口，设定基础循环保证程序运行
*/
int main(int argc, char *argv[]){
	int choice; //定义一个选择变量 
	do{
		choice = main_form();
		switch(choice){
			case 1:customer_form();
			       break;
			case 2:admin_form();
			       break;
		}
	}while(choice!=3); //菜单循环 
	
	return 0;
} 