#include"init.h" // 引入初始化模块

/*
 * 函数功能: 主界面函数，显示主界面并获取用户选择信息
 */
int main_form(){
	CLEAR_SCREEN();            // 清屏
	int greet_type = 0;        // 初始化问候类型
	struct tm* p = get_time(); // 获取时间
	greet(p,greet_type);       // 根据时间打印问候语 
	
	int op; //定义一个选择变量 
	printf("请择一身份入内：\n");
	printf("1.往来客商\n");     // 顾客界面
	printf("2.掌柜内务\n");     // 管理员界面
	printf("3.后厨掌勺\n");     // 厨师界面
	printf("4.打烊离去\n");     // 退出程序
	printf("请在此输入：");
	scanf("%d",&op);
	
	error_check(1,4,&op);	   // 差错检测，防止输入错误选择导致程序崩溃
	return op; 
}


/*
 * 函数功能: 用于检测输入是否正确，若为非法输入，则重复修改选择直至合法输入
 */
void error_check(int range_left ,int range_right, int* op){
	while(*op < range_left || *op > range_right){
		printf("输入错误！请重新输入:");
		scanf("%d",op); 
	}
}


/*
 * 函数功能: 清空标准输入缓冲区
 */
void clear_stdin_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}


/*
 * 函数功能: 智能问候函数，根据时间和用户身份输出不同的问候语
 */
void greet(struct tm* p,int user_role){
	//转换时区
	int china_time = (p->tm_hour + 8) % 24;   //伦敦时间->北京时间（UTC+8）
	//依据时间首先问好
	printf("+++++++++++++++++++++++++++++++++++++++++++\n");
	if(china_time >= 6 && china_time < 8) printf("晨光熹微，客官安好！");
	else if(china_time >= 8 && china_time < 12) printf("日暖风和，客官安坐！"); 
	else if(china_time >= 12 && china_time < 14) printf("日正中天，客官午安！");
	else if(china_time >= 14 && china_time < 18) printf("午后清风，客官闲适！");
	else if(china_time >= 18 && china_time < 24) printf("夜色渐浓，静候君来！");
	else printf("夜深人静，君可安歇！");
	
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
	else if(user_role == 3){
		printf("恭迎大厨到岗，后厨灶火已旺！\n");
	}
	else{
		printf("今日阳光正好，愿君心情愉快！\n");
	}   
}


/*
 * 函数功能: 获取时间并返回相关参数
 */
struct tm* get_time(){
	time_t timep;         //定义时间戳变量：1970年到现在的秒数
	struct tm* p;  
	time(&timep);         //把时间填入变量
	p = gmtime(&timep);   //把秒数变成含年月日时分秒的结构体 
	
	return p; 
} 