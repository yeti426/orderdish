#include "init.h"
#include <io.h>    // 用于 _findfirst / _findnext 遍历文件
//变量声明
//char pw[20] = "123456";                        //预置密码，方便调试 
//部分全局变量
char hot_dish_filename[20] = "hot_dish.txt";
char cold_dish_filename[20] = "cold_dish.txt";
char staple_food_filename[20] = "staple_food.txt";
char drink_filename[20] = "drink.txt"; 
//函数声明 
void gotoxy(int,int);
void input_password(char pw_input[],int wrong_time);     //输入密码 
int check_password(char pw_input[]);      //密码检测 
int admin_menu();                         //管理员菜单  
void income_check();                      //查看营业额 
void add_dish();                          //添加菜品 
void del_dish();                          //删除菜品 
void price_adjust();                      //价格调整 
void change_password();                   //密码修改 
void change_password_by_sms();             //手机验证码修改密码
void send_sms_code(char*, char*);          //发送短信验证码
void generate_code(char*);                 //生成验证码
int input_new_password(char*);             //输入并确认新密码(返回1成功0失败)
void create_date_filename(char*);         //生成当日文件名 
void set_recommend();                      //设置招牌菜
void cancel_recommend();                   //取消招牌菜
int is_recommend(int, int);               //检查菜品是否为招牌菜
void format_input_income(FILE*,double,double,double,double,double);            //收入格式化输入文件
int format_output_income(FILE*,double*,double*,double*,double*,double*);       //收入格式化输出到程序，返回1成功0失败 
void calculate_value(char* , double* , double* , double* , double * , double*);//计算收入   
void record_income(double,double,double,double,double);                        //用于记录单笔订单收入信息至收入文件中 
void view_reviews();                           //查看顾客评价


//外部函数声明 
extern void error_check(int,int,int*);
extern void greet(struct tm* p,int);
extern struct tm* get_time();


/*
* function_name: admin_form 
* return_type  : void
* param        : NULL
* description  : 管理员系统的主循环 
*/
void admin_form() {
    system("cls");
	system("mkdir income 2>nul");//mkdir income —在程序所在目录创建 income 文件夹（存放营业额记录）
	system("mkdir order 2>nul");//mkdir order — 创建 order 文件夹（存放订单文件）
	//2>nul — 如果文件夹已经存在，mkdir 会报错，2>nul 把错误信息吞掉，不让用户看到
    printf("========欢迎掌事入主后厨管事堂！========\n");
    int choice;
    printf("请在这里输入后台密码:");
    char pw_input[20];
    
    int wrong_time = 0;
    input_password(pw_input, wrong_time);
    
    int flag;
    do {
        flag = check_password(pw_input);
        if(flag != 0) {
            wrong_time++;
            printf("输入错误，请重新输入:");
            input_password(pw_input, wrong_time);
        } else {
            printf("输入正确！恭迎掌柜！\n");
            int i;
            for(i = 0; i < 3; i++) {
                printf(".");
                Sleep(1000);
            }
        }
    } while(flag != 0);
    
    do {
        choice = admin_menu();
        switch(choice) {
           case 1: income_check(); break;
            case 2: add_dish(); break; 
            case 3: del_dish(); break; 
            case 4: price_adjust(); break;
            case 5: change_password(); break; 
            case 6: set_recommend(); break;
            case 7: cancel_recommend(); break;
            case 8: view_reviews(); break;
            default: break; 
        }
    } while(choice != 9);
}



/*
* function_name: gotoxy 
* return_type  : void
* param        : int(x) , int(y)
* description  : 定位屏幕光标至 （x , y） 
*/ 
void gotoxy(int x, int y) {
    COORD pos = {x,y};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);// 获取标准输出设备句柄
    SetConsoleCursorPosition(hOut, pos);//两个参数分别是指定哪个窗体，具体位置
}



/*
* function_name: input_password
* return_type  : void
* param        : char*(要输入的密码) , int(输错的次数) 
* description  : 输入密码 
*/ 
void input_password(char pw_input[],int wrong_time){
	//清空字符串
	memset(pw_input,0,20);//memset:逐字节填充内存，常用于清空数组 / 缓冲区、初始化内存。

	int x,y;
	x = 21;
	y = 1 + wrong_time;//输错一次密码，输入位置就往下挪一行，让新密码在新行输入，不覆盖、不重叠、界面干净整齐！
	gotoxy(x,y);
	char ch;             // 保存每次按下的键
	char temp_input[20]; // 临时存密码
	int cnt = 0;         // 密码长度计数
	int i = 0;           // 数组下标
	do{
		ch = getch();
		if(ch == 13){//回车键 
			;
		}
		else if(ch == 8){//退格键 
			if(i>0){
				i--;
				cnt--;
				x--;// 光标左移一格
				gotoxy(x,y);// 移过去
				printf(" "); // 用空格覆盖掉 *
				gotoxy(x,y);// 再移回来，让光标停在正确位置
			}
		}
		else if (i >= 19) {
			// 密码已达最大长度，不允许再输入
			printf("\n");
			printf("密码最长只能输入19位。");
			continue;
		}
		else{//普通字符 
			temp_input[i] = ch;
			printf("*");
			x++;
			i++;
			cnt++;
		}
	}while(ch != 13);//直到按回车，才停止输入。
	printf("\n");
	
	if (cnt == 0) {
		pw_input[0] = '\0';  // 空密码，显式设为空字符串
	} else {
		strncpy(pw_input, temp_input, cnt);
		pw_input[cnt] = '\0';  // 手动补上字符串结束符
	}
}




/*
* function_name: check_password
* return_type  : int
* param        : char*(输入的密码)
* description  : 检测输入的密码是否正确，正确返回0 ，否则非0 
*/ 
int check_password(char pw_input[]){
	FILE* fp;
	char pw[20]="";
	fp = fopen("password.txt" , "r");
	if(fp == NULL){
		printf("密码文件不存在，请联系掌柜！\n");
		return -1;
	}
	fscanf(fp , "%s" , pw);
	fclose(fp); 
	
	return strcmp(pw_input,pw);
}




/*
* function_name: change_password
* return_type  : void
* param        : NULL
* description  : 修改密码 
*/
void change_password(){
	char pw_old[20];
	char pw_old_input[20];
	char pw_new[20];
	FILE* fp;
	fp = fopen("password.txt", "r");
	if (fp == NULL) {
		printf("密码文件不存在，无法修改密码！\n");
		getch();
		return;
	}
	fscanf(fp, "%s", pw_old);
	fclose(fp);
	
	system("cls");
	printf("**************************************\n");
	printf("请输入旧密码：");
	scanf("%19s", pw_old_input);
	
	int try_count = 0;
	while (strcmp(pw_old, pw_old_input) != 0) {
		try_count++;
		if (try_count >= 3) {
			printf("错误次数过多！\n");
			printf("是否通过手机验证码修改？(1.是 2.否)：");
			int sms_choice;
			scanf("%d", &sms_choice);
			error_check(1, 2, &sms_choice);
			if (sms_choice == 1) {
				change_password_by_sms();
			} else {
				printf("修改取消。\n");
			}
			getch();
			return;
		}
		printf("输入错误，请重新输入(剩余%d次):", 3 - try_count);
		scanf("%19s", pw_old_input);
	}
	
	if (!input_new_password(pw_new)) {
		getch();
		return;
	}
	
	fp = fopen("password.txt" , "w");
	fprintf(fp ,"%s" , pw_new);
	fclose(fp);
	
	int i ;
	printf("修改完成，请牢记新密码：%s\n" , pw_new);
	printf("**************************************\n");
	for( i = 0 ; i < 3 ; i++){
		printf(".");
		Sleep(1000);
	}
	
}




/*
* function_name: input_new_password
* return_type  : void
* param        : char*(新密码)
* description  : 输入新密码并确认一致
*/
int input_new_password(char* pw_new) {
	char pw_check[20];
	int retry = 0;

	printf("请输入新密码：");
	scanf("%19s", pw_new);

	printf("请确认新密码：");
	scanf("%19s", pw_check);

	while (strcmp(pw_new, pw_check) != 0) {
		retry++;
		if (retry >= 3) {
			printf("两次输入不一致次数过多，操作取消。\n");
			return 0;
		}
		printf("两次输入不一致！(剩余%d次)\n", 3 - retry);
		printf("请输入新密码：");
		scanf("%19s", pw_new);
		printf("请确认新密码：");
		scanf("%19s", pw_check);
	}
	return 1;
}

/*
* function_name: generate_code
* return_type  : void
* param        : char*(验证码)
* description  : 生成6位随机数字验证码
*/
void generate_code(char* code) {
	srand((unsigned int)time(NULL));
	int i;
	for (i = 0; i < 6; i++) {
		code[i] = '0' + rand() % 10;
	}
	code[6] = '\0';
}

/*
* function_name: send_sms_code
* return_type  : void
* param        : char*(手机号), char*(验证码)
* description  : 模拟发送短信验证码(正式部署请接入短信API)
*/
void send_sms_code(char* phone, char* code) {
	printf("【模拟短信】验证码已发送至手机号 %s，验证码为：%s\n", phone, code);
	printf("（正式部署时请接入短信服务商API）\n");
}

/*
* function_name: change_password_by_sms
* return_type  : void
* param        : NULL
* description  : 通过手机短信验证码修改密码
*/
void change_password_by_sms() {
	char phone[12];
	char sms_code[7];
	char input_code[7];
	char pw_new[20];

	system("cls");
	printf("**************************************\n");
	printf("         手机验证码修改密码\n");
	printf("**************************************\n");

	printf("请输入绑定的手机号(11位)：");
	scanf("%11s", phone);

	if (strlen(phone) != 11) {
		printf("手机号格式不正确！\n");
		getch();
		return;
	}

	generate_code(sms_code);
	send_sms_code(phone, sms_code);

	int try_count = 0;
	while (try_count < 3) {
		printf("请输入6位短信验证码：");
		scanf("%6s", input_code);

		if (strcmp(sms_code, input_code) == 0) {
			printf("验证通过！\n");
			break;
		}

		try_count++;
		if (try_count >= 3) {
			printf("验证码错误次数过多，操作取消。\n");
			getch();
			return;
		}
		printf("验证码错误，请重新输入(剩余%d次)：", 3 - try_count);
	}

	if (!input_new_password(pw_new)) {
		getch();
		return;
	}

	FILE* fp = fopen("password.txt", "w");
	if (fp == NULL) {
		printf("密码文件写入失败！\n");
		getch();
		return;
	}
	fprintf(fp, "%s", pw_new);
	fclose(fp);

	printf("\n密码修改成功！\n");
	printf("**************************************\n");
	getch();
}


/*
* function_name: admin_menu 
* return_type  : int
* param        : NULL
* description  : 管理员主界面显示，并返回选择 
*/ 
int admin_menu(){
	system("cls");
	int greet_type = 2;// 问候类型：2=管理员
	struct tm* p = get_time();// 获取当前系统时间（几点几分）
	greet(p,greet_type);// 根据时间 + 身份，打印问候语
	
	int choice=0;
	printf("请选择操作\n");
	printf("1.查看营业额\n");
	printf("2.添加菜品\n");
	printf("3.删除菜品\n");
	printf("4.修改价格\n");
	printf("5.密码修改\n"); 
	printf("6.设置招牌菜\n");
	printf("7.取消招牌菜\n");
	printf("8.查看顾客评价\n");
	printf("9.退出\n");

	printf("在此输入：");
	while (scanf("%d", &choice) != 1) {
		while (getchar() != '\n');  // 清空输入缓冲区，一个一个吃掉缓冲区里的字符，直到遇到换行符为止，把 abc\n 全部清空。
		printf("输入无效，请重新输入(1-9):");
	}
	
	error_check(1,9,&choice);
	return choice;
}





/*
* function_name: record_income
* return_type  : void
* param        : 5个double数据 传入订单中各项收入信息 
* description  : 将读取到的订单收入信息记录到 income 文件夹 对应日期的文件中 
*/
void record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink){
	double all_income = 0.0, staple_food_income = 0.0, hot_dish_income = 0.0, cold_dish_income = 0.0, drink_income = 0.0;
	//接收本次订单赚的钱，准备存起来。
	char fdate[50]="";
	create_date_filename(fdate); 
					
	FILE *fp1;
	fp1 = fopen(fdate , "r");// 第一步：用 "r" 打开，只想看看文件有没有
	if(fp1 == NULL){
		fp1 = fopen(fdate,"w");
		if(fp1 == NULL){
			printf("错误: 无法创建收入文件 %s\n", fdate);
			return;
		}
		format_input_income(fp1,account,account_hot_dish,account_cold_dish,account_staple_food,account_drink);
		fclose(fp1);
	}
	else{
		if(!format_output_income(fp1,&all_income,&hot_dish_income,&cold_dish_income,&staple_food_income,&drink_income)){
			printf("错误: 收入文件 %s 格式错误，本次数据可能丢失！\n", fdate);
			fclose(fp1);
			return;
		}
		fclose(fp1);
		all_income += account;                // 今天总钱 + 本次钱
		hot_dish_income += account_hot_dish;  // 热菜累加
		cold_dish_income += account_cold_dish;// 凉菜累加
		staple_food_income += account_staple_food;//主食累加
		drink_income += account_drink;        // 饮料累加
		fp1 = fopen(fdate , "w");
		if(fp1 == NULL){
			printf("错误: 无法写入收入文件 %s，本次收入数据丢失！\n", fdate);
			return;
		}
		format_input_income(fp1,all_income,hot_dish_income,cold_dish_income,staple_food_income,drink_income);
		fclose(fp1); 
	} 
}







/*
* function_name: format_input_income 
* return_type  : void
* param        : FILE* fp 5 个double数据 
* description  : 格式化输入数据 ，将5个double数据格式化输入到文件中 
*/
void format_input_income(FILE* fp,double a,double b,double c,double d,double e){
	fprintf(fp,"%lf\n",a);
	fprintf(fp,"%lf\n",b);
	fprintf(fp,"%lf\n",c);
	fprintf(fp,"%lf\n",d);
	fprintf(fp,"%lf\n",e);
}





/*
* function_name: format_output_income 
* return_type  : void
* param        : FILE* fp 5 个double数据 
* description  : 格式化输出数据 ，将5个double数据格式化输出到程序中 
*/
int format_output_income(FILE* fp,double* a,double* b,double* c,double* d,double* e){
	if(fscanf(fp,"%lf\n",a) != 1) return 0;
	if(fscanf(fp,"%lf\n",b) != 1) return 0;
	if(fscanf(fp,"%lf\n",c) != 1) return 0;
	if(fscanf(fp,"%lf\n",d) != 1) return 0;
	if(fscanf(fp,"%lf\n",e) != 1) return 0;
	return 1;
}






/*
* function_name: create_date_filename
* return_type  : void
* param        : char* fdate(用于返回生成后的文件名)
* description  : 自动获取当天日期，并返回文件名 如202161.txt 
*/
void create_date_filename(char* fdate){
	fdate[0] = '\0';// 防御性清空，防止调用方传入未初始化的字符串
	char date[20] = "";//strcat 拼接的前提：目标字符串必须是空的！
	struct tm* p = get_time();//struct tm 是系统固定结构，localtime 自动把当前时间 年 / 月 / 日 填进去，p 指向这个装满时间的盒子
	int mday = p->tm_mday;// 使用局部变量，避免修改gmtime返回的共享内存
	if(p->tm_hour + 8 >= 24) mday -= 1;//北京时间已过午夜，凌晨营业的收银额归入前一天
	
	char year[5] = "";
	char month[5] = "";
	char day[5] = "";
	//itoa:把 数字 变成 字符串 的工具函数！
	itoa(p->tm_year + 1900 , year , 10);//因为 tm_year 是从 1900 年开始算的，所以要 +1900。
	itoa(p->tm_mon + 1 , month , 10);//月份也是从 0 开始算
	itoa(mday, day , 10);// tm_mday 直接表示当月第几天，不需要 +1
	strcat(date,year);
	strcat(date,month);
	strcat(date,day);
	strcat(date,".txt");
	
	strcat(fdate , "income//");
	strcat(fdate , date);
}






/*
* function_name: income_check
* return_type  : void
* param        : NULL
* description  : 查看收入 
*/ 
void income_check(){
	//查看当天营业额
	system("cls"); 
	
	double all_income = 0.0, staple_food_income = 0.0, hot_dish_income = 0.0, cold_dish_income = 0.0, drink_income = 0.0;

	char fdate[50] = "";
	create_date_filename(fdate);
	
	FILE* fp;
	fp = fopen(fdate, "r");
	if(fp == NULL) {
		printf("今天好像还没有营业哟!\n");
		getch();//显示完收入表格后暂停，等用户看完按任意键再返回。
		return;
	}

	if(!format_output_income(fp, &all_income, &hot_dish_income, &cold_dish_income, &staple_food_income, &drink_income)) {
		printf("错误: 收入文件 %s 格式错误！\n", fdate);
		fclose(fp);
		getch();
		return;
	}
	fclose(fp);

	printf("**************************************\n");
	printf("总收入:%.2lf\n", all_income);
	printf("热菜收入:%.2lf\n", hot_dish_income);
	printf("凉菜收入:%.2lf\n", cold_dish_income);
	printf("主食收入:%.2lf\n", staple_food_income);
	printf("饮品收入:%.2lf\n", drink_income);
	printf("**************************************\n");
	getch();
}



/*
* function_name: add_dish 
* return_type  : void
* param        : NULL
* description  : 添加菜品 
*/ 
void add_dish(){
	//定义文件指针
	FILE *fp;
	
	//定义控制变量
	int choice; 
 
	//功能运行主体
	do{
		//定义添加主体
		dish_menu new_dish; 
		
		//功能界面 
		system("cls");
		printf("请输入菜品编号：");
		scanf("%d",&new_dish.no); 
		printf("请输入菜品名称：");
		scanf("%s",new_dish.dish_name); 
		printf("请输入菜品价格：");
		scanf("%lf",&new_dish.dish_price);
		printf("请选择菜品种类(1.热菜 2.凉菜 3.主食 4.饮品)：");
		scanf("%d",&new_dish.type);
		error_check(1,4,&new_dish.type);
		
		// --- 新增：询问是否需要口味选项 ---
		printf("该菜品是否需要调整辣度/口味？(1.是 0.否): ");
		scanf("%d", &new_dish.has_options);
		
		char filename[20];
		switch(new_dish.type){
			case 1:strcpy( filename , hot_dish_filename );break;
			case 2:strcpy( filename , cold_dish_filename );break;
			case 3:strcpy( filename , staple_food_filename );break;
			case 4:strcpy( filename , drink_filename );break;
		}
		
		fp = fopen( filename , "a");// 在文件末尾追加，不覆盖原有数据。
		if(fp == NULL){
			printf("错误: 无法打开菜品文件 %s\n", filename);
			getch();
			return;
		}
		//将菜品信息录入文件（现在是5个字段）
		fprintf(fp,"%d\n",new_dish.no);
		fprintf(fp,"%s\n",new_dish.dish_name);
		fprintf(fp,"%lf\n",new_dish.dish_price);
		fprintf(fp,"%d\n",new_dish.type); 
		fprintf(fp,"%d\n",new_dish.has_options); // ← 新增：写入选项标记
		
		fclose(fp);
		
		//判断是否继续执行 
		printf("是否继续添加菜品：\n");
		printf("1.是\n");
		printf("2.否\n");
		scanf("%d",&choice);
		
		//输入异常检测
		error_check(1,2,&choice);
		 
	}while(choice != 2);
}






/*
* function_name: del_dish
* return_type  : void
* param        : NULL
* description  : 删除菜品 
*/ 
void del_dish(){
	int choice;
	int quit_handle = 0;//是一个退出标志，用来处理"用户找不到菜品后选退出"的场景。
	int pos;
	do{
		system("cls");
		printf("1.热菜\n");
		printf("2.凉菜\n");
		printf("3.主食\n");
		printf("4.饮品\n");
		printf("请选择删除菜品的类型:");
		int type;
		scanf("%d" , &type);
		
		error_check(1,4,&type);
		
		//依据菜品类型打开对应文件
		char filename[20];
		switch(type){
			case 1:strcpy(filename,hot_dish_filename);break; 
			case 2:strcpy(filename,cold_dish_filename);break;
			case 3:strcpy(filename,staple_food_filename);break;
			case 4:strcpy(filename,drink_filename);break;
		}
		 
		FILE *fp;
		dish_menu dm[MAX_LENGTH];//创建一个数组 dm，能装下【所有菜品】
		dish_menu dm_new[MAX_LENGTH];//创建第二个数组 dm_new，装【删除后的新菜单】
		int cnt = 0;
		
		fp = fopen(filename , "r");
		if(fp == NULL){
			printf("错误: 无法打开菜品文件 %s\n", filename);
			getch();
			return;
		}
		while(cnt < MAX_LENGTH && fscanf(fp,"%d",&dm[cnt].no) == 1){
			if(fscanf(fp,"%s",dm[cnt].dish_name) != 1) break;
			if(fscanf(fp,"%lf",&dm[cnt].dish_price) != 1) break;
			if(fscanf(fp,"%d",&dm[cnt].type) != 1) break;
			// --- 新增：读取口味选项标记（兼容旧文件）---
			if(fscanf(fp,"%d",&dm[cnt].has_options) != 1) dm[cnt].has_options = 0;
			cnt++;
		}
		fclose(fp);
		
		if(cnt == 0){
			printf("该分类下暂无菜品！\n");
			getch();
			return;
		}
		
		// 展示当前菜单
		printf("当前菜品列表：\n");
		printf("----------------------------------------------------------\n");
		printf("%-8s %-24s %-10s %-8s\n", "编号", "名称", "价格", "选项");
		int i;
		for(i = 0; i < cnt; i++) {
			const char* opt_str = dm[i].has_options ? "[选]" : "";
			if(is_recommend(dm[i].type, dm[i].no))
				printf("%-8d 【招牌】%-16s %10.2lf %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
			else
				printf("%-8d %-24s %10.2lf %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
		}
		printf("----------------------------------------------------------\n");
		
		printf("请输入需要删除的菜品编号：");
		int del_no;
		scanf("%d",&del_no);
		
		//检索文件 该菜品是否存在 
		int flag = 0;
		pos = -1;
		do{
			int j;
			for(j = 0 ; j < cnt ; j++){
				if(dm[j].no == del_no){
					flag = 1;
					pos = j;
					break;
				}
			}
			if(flag == 0){
				printf("没有该菜品!\n");
				printf("1.退出\n");
				printf("2.重新输入\n");
				
				int _choice;
				scanf("%d",&_choice);
				
				error_check(1,2,&_choice);
				
				if(_choice == 1){
					quit_handle = 1;
					break;
				}
				
				if(_choice == 2) {
					printf("请重新输入菜品编号：");
					scanf("%d",&del_no);
				}
			}
		}while(flag == 0);
		
		printf("是否确认删除<%s>菜品?1.是 2.否",dm[pos].dish_name);
		int del_choice;
		scanf("%d",&del_choice);
		
		error_check(1,2,&del_choice);
		
		//删除菜品
		if(del_choice == 1){
			int i;
			int j = 0;
			for(i = 0 ; i < cnt ; i++){
				if(i == pos){ // 跳过该菜品 
					continue; 
				}
				else{
					dm_new[j].no = dm[i].no;
					strcpy(dm_new[j].dish_name , dm[i].dish_name);
					dm_new[j].dish_price = dm[i].dish_price;
					dm_new[j].type = dm[i].type;
					dm_new[j].has_options = dm[i].has_options; // ← 新增：保留选项标记
					j++;
				}
			}
			
			//将 dm_new 重新覆盖录入到菜单文件中（删除后少一条，共 j 条）
			fp = fopen(filename , "w");
			for(i = 0 ; i < j ; i++){
				fprintf(fp , "%d\n",dm_new[i].no);
				fprintf(fp , "%s\n",dm_new[i].dish_name);
				fprintf(fp , "%lf\n",dm_new[i].dish_price);
				fprintf(fp , "%d\n",dm_new[i].type);
				fprintf(fp , "%d\n",dm_new[i].has_options); // ← 新增：写入选项标记
			}
			fclose(fp);
			
			printf("删除成功！\n");	 
		} 

		printf("是否继续删除？1.是，2否？");
		scanf("%d",&choice);
		
		error_check(1,2,&choice);
	}while(choice != 2);
}



/*
* function_name: view_reviews
* return_type  : void
* param        : NULL
* description  : 查看顾客评价
*/
void view_reviews() {
    system("cls");
    printf("===== 查看顾客评价 =====\n\n");

    // 检查 reviews 目录是否存在
    if (access("reviews", 0) != 0) {
        printf("暂无顾客评价记录！\n");
        getch();
        return;
    }

    // 列出所有评价文件（review_table_雅座.txt）
    struct _finddata_t fileinfo;
    intptr_t handle;
    char pattern[100] = "reviews\\review_table_*.txt";

    handle = _findfirst(pattern, &fileinfo);
    if (handle == -1) {
        printf("暂无顾客评价文件！\n");
        getch();
        return;
    }

    int tables[100];
    int table_cnt = 0;

    printf("已有评价的雅座：\n");
    printf("--------------------------\n");
    do {
        int table_no;
        // 从文件名 "review_table_1.txt" 中提取雅座
        sscanf(fileinfo.name, "review_table_%d.txt", &table_no);
        tables[table_cnt++] = table_no;
        printf("  雅座: %d\n", table_no);
    } while (_findnext(handle, &fileinfo) == 0);
    _findclose(handle);

    printf("--------------------------\n");
    printf("请输入要查看的雅座(0退出)：");
    int choice;
    scanf("%d", &choice);
    if (choice == 0) return;

    // 验证雅座是否存在
    int found = 0, i;
    for (i = 0; i < table_cnt; i++) {
        if (tables[i] == choice) { found = 1; break; }
    }
    if (!found) {
        printf("该雅座暂无评价！\n");
        getch();
        return;
    }

    // 读取并显示评价内容
    char filename[100];
    snprintf(filename, sizeof(filename), "reviews\\review_table_%d.txt", choice);
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("无法打开评价文件！\n");
        getch();
        return;
    }

    system("cls");
    printf("===== %d号桌顾客评价 =====\n\n", choice);
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }
    fclose(fp);

    printf("\n按任意键返回...");
    getch();
}

//以下为c部分


/*
* function_name: is_recommend
* return_type  : int
* param        : int(菜品种类), int(菜品编号)
* description  : 检查菜品是否在招牌菜列表中，是返回1，否返回0
*/
int is_recommend(int type, int no) {
	FILE* fp = fopen("recommend.txt", "r");
	if(fp == NULL) return 0;

	int t, n;
	while(fscanf(fp, "%d %d", &t, &n) == 2) {//每次从文件中读取两个整数，分别存到 t 和 n，如果成功读到了 2 个整数就继续循环，否则退出。
		if(t == type && n == no) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

/*
* function_name: set_recommend
* return_type  : void
* param        : NULL
* description  : 设置招牌菜
*/
void set_recommend() {
	system("cls");
	printf("===== 设置招牌菜 =====\n");
	printf("1.热菜\n");
	printf("2.凉菜\n");
	printf("3.主食\n");
	printf("4.饮品\n");
	printf("请选择菜品种类：");
	int type;
	scanf("%d", &type);
	error_check(1, 4, &type);

	char filename[20];
	switch(type) {
		case 1: strcpy(filename, hot_dish_filename); break;
		case 2: strcpy(filename, cold_dish_filename); break;
		case 3: strcpy(filename, staple_food_filename); break;
		case 4: strcpy(filename, drink_filename); break;
	}

	dish_menu dm[MAX_LENGTH];
	int cnt = 0;
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) {
		printf("错误: 无法打开菜品文件 %s\n", filename);
		getch();
		return;
	}
	while(cnt < MAX_LENGTH && fscanf(fp, "%d", &dm[cnt].no) == 1) {
		if(fscanf(fp, "%s", dm[cnt].dish_name) != 1) break;
		if(fscanf(fp, "%lf", &dm[cnt].dish_price) != 1) break;
		if(fscanf(fp, "%d", &dm[cnt].type) != 1) break;
		cnt++;
	}
	fclose(fp);

	if(cnt == 0) {
		printf("该分类下暂无菜品！\n");
		getch();
		return;
	}

	// 展示菜单
	printf("当前菜品列表：\n");
	printf("----------------------------------------------------------\n");
	printf("%-8s %-24s %-10s\n", "编号", "名称", "价格");
	int i;
	for(i = 0; i < cnt; i++) {
		if(is_recommend(dm[i].type, dm[i].no))
			printf("%-8d 【招牌】%-10s %.2lf\n", dm[i].no, dm[i].dish_name, dm[i].dish_price);
		else
			printf("%-8d %-14s %.2lf\n", dm[i].no, dm[i].dish_name, dm[i].dish_price);
	}
	printf("----------------------------------------------------------\n");

	printf("请输入要设为招牌的菜品编号(0取消)：");
	int rec_no;
	scanf("%d", &rec_no);
	if(rec_no == 0) return;

	// 检索是否存在
	int found = 0;
	for(i = 0; i < cnt; i++) {
		if(dm[i].no == rec_no) {
			found = 1;
			break;
		}
	}
	if(!found) {
		printf("没有该菜品！\n");
		getch();
		return;
	}

	// 检查是否已是招牌
	if(is_recommend(type, rec_no)) {
		printf("该菜品已是招牌菜！\n");
		getch();
		return;
	}

	// 追加写入
	fp = fopen("recommend.txt", "a");
	if(fp == NULL) {
		printf("错误: 无法写入招牌菜文件！\n");
		getch();
		return;
	}
	fprintf(fp, "%d %d\n", type, rec_no);
	fclose(fp);

	printf("设置成功！\n");
	getch();
}

/*
* function_name: cancel_recommend
* return_type  : void
* param        : NULL
* description  : 取消招牌菜
*/
void cancel_recommend() {
	system("cls");
	printf("===== 取消招牌菜 =====\n");

	FILE* fp = fopen("recommend.txt", "r");
	if(fp == NULL) {
		printf("暂无招牌菜记录！\n");
		getch();
		return;
	}

	int types[MAX_LENGTH], nos[MAX_LENGTH];
	int cnt = 0;
	int t, n;
	while(cnt < MAX_LENGTH && fscanf(fp, "%d %d", &t, &n) == 2) {
		types[cnt] = t;
		nos[cnt] = n;
		cnt++;
	}
	fclose(fp);

	if(cnt == 0) {
		printf("暂无招牌菜！\n");
		getch();
		return;
	}

	printf("当前招牌菜：\n");
	printf("----------------------------------------------------------\n");
	printf("序号  分类    编号\n");
	int i;
	char* type_names[] = {"", "热菜", "凉菜", "主食", "饮品"};
	for(i = 0; i < cnt; i++) {
		printf("%-6d %-8s %d\n", i + 1, type_names[types[i]], nos[i]);
	}
	printf("----------------------------------------------------------\n");

	printf("请输入要取消的序号(0退出)：");
	int choice;
	scanf("%d", &choice);
	if(choice == 0) return;
	if(choice < 1 || choice > cnt) {
		printf("无效选择！\n");
		getch();
		return;
	}

	// 重写文件，跳过选中项
	fp = fopen("recommend.txt", "w");
	if(fp == NULL) {
		printf("错误: 无法写入！\n");
		getch();
		return;
	}
	for(i = 0; i < cnt; i++) {
		if(i != choice - 1) {
			fprintf(fp, "%d %d\n", types[i], nos[i]);
		}
	}
	fclose(fp);

	printf("已取消该招牌菜！\n");
	getch();
}





/*
* function_name: price_adjust 
* return_type  : void
* param        : NULL
* description  : 价格调整 
*/ 
void price_adjust(){
	int choice;
	int quit_handle = 0;
	int pos;
	do{
		system("cls");
		printf("1.热菜\n");
		printf("2.凉菜\n");
		printf("3.主食\n");
		printf("4.饮品\n");
		printf("请选择修改菜品的类型:");
		int type;
		scanf("%d" , &type);
		
		error_check(1,4,&type);
		
		//依据菜品类型打开对应文件
		char filename[20];
		switch(type){
			case 1:strcpy(filename,hot_dish_filename);break; 
			case 2:strcpy(filename,cold_dish_filename);break;
			case 3:strcpy(filename,staple_food_filename);break;
			case 4:strcpy(filename,drink_filename);break;
		}
		 
		FILE *fp;
		dish_menu dm[MAX_LENGTH];
		int cnt = 0;
		
		fp = fopen(filename , "r");
		if(fp == NULL){
			printf("错误: 无法打开菜品文件 %s\n", filename);
			getch();
			return;
		}
		while(cnt < MAX_LENGTH && fscanf(fp,"%d",&dm[cnt].no) == 1){
			fscanf(fp,"%s",dm[cnt].dish_name);
			fscanf(fp,"%lf",&dm[cnt].dish_price);
			fscanf(fp,"%d",&dm[cnt].type);
			// --- 新增：读取口味选项标记（兼容旧文件）---
			if(fscanf(fp,"%d",&dm[cnt].has_options) != 1) dm[cnt].has_options = 0;
			cnt++;
		}
		fclose(fp);
		
		if(cnt == 0){
			printf("该分类下暂无菜品！\n");
			getch();
			return;
		}
		
		// 展示当前菜单
		printf("当前菜品列表：\n");
		printf("----------------------------------------------------------\n");
		printf("编号    名称          价格      选项\n");
		int i;
		for(i = 0; i < cnt; i++) {
			const char* opt_str = dm[i].has_options ? "[选]" : "";
			if(is_recommend(dm[i].type, dm[i].no))
				printf("%-8d 【招牌】%-10s %.2lf   %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
			else
				printf("%-8d %-14s %.2lf   %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
		}
		printf("----------------------------------------------------------\n");
		
		printf("请输入需要修改价格的菜品编号：");
		int adjust_no;
		scanf("%d",&adjust_no);
		
		//检索文件 该菜品是否存在 
		int flag = 0;
		pos = -1;
		do{
			int i;
			for(i = 0 ; i < cnt ; i++){
				if(dm[i].no == adjust_no){
					flag = 1;
					pos = i;
					break;
				}
			}
			if(flag == 0){
				printf("没有该菜品!\n");
				printf("1.退出\n");
				printf("2.重新输入\n");
				
				int _choice;
				scanf("%d",&_choice);
				
				error_check(1,2,&_choice);
				
				if(_choice == 1){
					quit_handle = 1;
					break;
				}
				
				if(_choice == 2) {
				printf("请重新输入菜品编号：");
				scanf("%d",&adjust_no);
			}
			}
		}while(flag == 0);
		
		if(quit_handle == 1) break;
		
		//修改菜品 
		printf("是否确认修改<%s>菜品的价格?1.是 2.否",dm[pos].dish_name);
		int adjust_choice;
		scanf("%d",&adjust_choice);
		
		error_check(1,2,&adjust_choice);
		
		if(adjust_choice == 1){
			printf("请输入修改菜品后的价格：");
			double re_price;
			scanf("%lf",&re_price); 
			
			dm[pos].dish_price = re_price;
			
			//将 dm重新覆盖录入到菜单文件中
			fp = fopen(filename , "w");
			int i;
			for(i = 0 ; i < cnt ; i++){
				fprintf(fp , "%d\n",dm[i].no);
				fprintf(fp , "%s\n",dm[i].dish_name);
				fprintf(fp , "%lf\n",dm[i].dish_price);
				fprintf(fp , "%d\n",dm[i].type);
				fprintf(fp , "%d\n",dm[i].has_options); // ← 新增：写入选项标记
			}
			fclose(fp);
			
			printf("修改成功！\n");	 
		} 
		
		
		if(quit_handle == 1) break;
		
		printf("是否继续修改？1.是，2否？");
		scanf("%d",&choice);
		
		error_check(1,2,&choice);
    }while(choice != 2);
}

