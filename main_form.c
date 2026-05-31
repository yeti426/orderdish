#include"init.h" //引入初始化模块

/*
* 名称：main_form
* 功能: 程序主界面的显示界面，获取选择信息并返回至主函数main
*/
int main_form(){
	system("cls"); //清屏
	int greet_type = 0; //初始化问候类型
	struct tm* p = get_time(); //获取时间
	greet(p,greet_type); //根据时间打印问候语 
	
	int op; //定义一个选择变量 
	printf("请择一身份入内：\n");
	printf("1.往来客商\n");
	printf("2.掌柜内务\n");
	printf("3.打烊离去\n");
	printf("请在此输入：");
	scanf("%d",&op);
	
	//差错检测，防止输入错误选择导致程序崩溃
	error_check(1,3,&op);
	return op; 
}

/*
* 名称: error_check 
* 功能: 用于检测输入是否正确，若为非法输入，则重复修改选择直至合法
*/
void error_check(int range_left ,int range_right, int* op){
	while(*op < range_left || *op > range_right){
		printf("输入错误！请重新输入:");
		scanf("%d",op); 
	}
}

/*
* 名称: greet
* 功能: 问候函数，依据当前时间和用户类型给出问候
*/
void greet(struct tm* p,int user_role){
	//转换时区
	int china_time = (p->tm_hour + 8) % 24; //伦敦时间->北京时间 
	if(p->tm_hour + 8 >= 24) p->tm_mday -= 1; //日期回退 
	//依据时间首先问好
	printf("+++++++++++++++++++++++++++++++++++++++++++\n");
	if(china_time >= 6 && china_time < 8) printf("早上好！");
	else if(china_time >= 8 && china_time < 12) printf("上午好！"); 
	else if(china_time >= 12 && china_time < 14) printf("中午好！");
	else if(china_time >= 14 && china_time < 18) printf("下午好！");
	else if(china_time >= 18 && china_time < 24) printf("晚上好！");
	else printf("夜深人静，安歇吧！");
	
	printf("\n");
	
	//依据用户身份输出欢迎语 
	if(user_role == 0){
		printf("====================================\n");
        printf("          C语言客栈 ·点膳处          \n");
        printf("====================================\n");
	}
	else if(user_role == 1){
		printf("客官远道而来，小店蓬荜生辉！\n");
	}
	else{
		printf("今日阳光正好，愿君心情愉快！\n");
	}   
}

/*
* 名称: get_time 
* 功能: 获取时间并返回相关参数
*/
struct tm* get_time(){
	time_t timep; //定义时间戳变量：1970年到现在的秒数
	struct tm* p;  
	time(&timep); //把时间填入变量
	p = gmtime(&timep); //把秒数变成含年月日时分秒的结构体 
	
	return p; 
} 