#include "init.h"
#ifdef _WIN32
#include <io.h>    // Windows: 用于 _findfirst / _findnext 遍历文件
#else
#include <dirent.h> // macOS/Linux: 目录遍历
#endif

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
void income_check();                      //查看营业额 /*
void change_password();                   //密码修改 
// void change_password_by_sms();             //手机验证码修改密码
// void send_sms_code(char*, char*);          //发送短信验证码
// void generate_code(char*);                 //生成验证码
int input_new_password(char*, const char*); //输入并确认新密码(返回1成功0失败)，第二个参数为旧密码用于比对
void calculate_value(char* , double* , double* , double* , double * , double*);//计算收入 
void create_date_filename(char*);         //生成当日文件名 
void format_input_income(FILE*,double,double,double,double,double);            //收入格式化输入文件
int format_output_income(FILE*,double*,double*,double*,double*,double*);       //收入格式化输出到程序，返回1成功0失败 
void calculate_value(char* , double* , double* , double* , double * , double*);//计算收入   
int record_income(double,double,double,double,double);                        //用于记录单笔订单收入信息至收入文件中，返回1成功0失败
void view_reviews();                           //查看顾客评价


//外部函数声明 
extern void error_check(int,int,int*);
extern void greet(struct tm* p,int);
extern struct tm* get_time();
extern void add_dish();                          
extern void del_dish();                         
extern void set_recommend();                     
extern void cancel_recommend();  
extern int is_recommend(int, int);                 
extern void price_adjust(); 
extern void create_date_filename(char* fdate);
extern void format_input_income(FILE* fp, double a, double b, double c, double d, double e);
extern int format_output_income(FILE* fp, double* a, double* b, double* c, double* d, double* e);
extern int record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink);                   


/*
* function_name: admin_form 
* return_type  : void
* param        : NULL
* description  : 管理员系统的主循环 
*/
void admin_form() {
    CLEAR_SCREEN();//清屏
	/*在管理员界面入口进来时自动创建两个文件夹：
	income/ — 用来存收入/对账相关的文件
	order/ — 用来存订单文件
	*/
#ifdef _WIN32//编译器自带的宏，只要在 Windows 下编译运行，它就自动成立
	system("mkdir income 2>nul");//只屏蔽报错，正常输出照常显示。
	system("mkdir order 2>nul");
#else
	system("mkdir -p income");
	system("mkdir -p order");
#endif
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
#ifdef _WIN32
    COORD pos = {x,y};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);// 获取标准输出设备句柄
    SetConsoleCursorPosition(hOut, pos);//两个参数分别是指定哪个窗体，具体位置
#else
    // macOS/Linux: 使用 ANSI 转义序列移动光标
    printf("\033[%d;%dH", y + 1, x + 1);
#endif
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
		if(ch == 13 || ch == 10){//回车键 (Windows:13, Mac:10) 
			;
		}
		else if(ch == 8 || ch == 127){//退格键 (Windows:8, Mac:127) 
			if(i>0){
				i--;
				cnt--;
				x--;// 光标左移一格
				gotoxy(x,y);// 移过去
				printf(" "); // 用空格覆盖掉 *
				gotoxy(x,y);// 再移回来，让光标停在正确位置
			}
		}
		else if(ch == 27){//ESC序列起始（方向键、删除键等），吞掉后续字节
			int ch2 = getch();
			if(ch2 == '[') {
				int ch3 = getch();
				if(ch3 >= '0' && ch3 <= '9') {
					getch(); // 再读一个（如 '~'）
				}
			}
			continue; // 忽略整个ESC序列，重新循环
		}
		else if (i >= 19) {
			// 密码已达最大长度，不允许再输入
			printf("\n");
			printf("密码最长只能输入19位。");
			continue;
		}
		else if(ch >= 32 && ch <= 126){//普通可打印字符 
			temp_input[i] = ch;
			printf("*");
			x++;
			i++;
			cnt++;
		}
	}while(ch != 13 && ch != 10);//直到按回车，才停止输入。
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

    CLEAR_SCREEN();//清屏
	printf("**************************************\n");
	printf("请输入旧密码：");
	scanf("%19s", pw_old_input);
	
	int try_count = 0;
	while (strcmp(pw_old, pw_old_input) != 0) {
		try_count++;
		if (try_count >= 3) {
			printf("错误次数过多，返回主菜单！\n");
			// printf("是否通过手机验证码修改？(1.是 2.否)：");
			// int sms_choice;
			// scanf("%d", &sms_choice);
			// error_check(1, 2, &sms_choice);
			// if (sms_choice == 1) {
			// 	change_password_by_sms();
			// } else {
			// 	printf("修改取消。\n");
			// }
			getch();
			return;
		}
		printf("输入错误，请重新输入(剩余%d次):", 3 - try_count);
		scanf("%19s", pw_old_input);
	}
	
	if (!input_new_password(pw_new, pw_old)) {
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
* return_type  : int (1成功, 0失败)
* param        : char* pw_new(新密码), const char* pw_old(旧密码)
* description  : 输入新密码并确认一致，同时校验是否与旧密码相同
*/
int input_new_password(char* pw_new, const char* pw_old) {
	char pw_check[20];
	int retry = 0;

	printf("请输入新密码：");
	if(scanf("%19s", pw_new) != 1) {
		printf("输入无效，操作取消。\n");
		return 0;
	}

	// 首次输入就检查是否与旧密码相同
	if(strcmp(pw_new, pw_old) == 0) {
		printf("新密码与旧密码相同，修改无效！\n");
		return 0;
	}

	printf("请确认新密码：");
	if(scanf("%19s", pw_check) != 1) {
		printf("输入无效，操作取消。\n");
		return 0;
	}

	while (strcmp(pw_new, pw_check) != 0) {
		retry++;
		if (retry >= 3) {
			printf("两次输入不一致次数过多，操作取消。\n");
			return 0;
		}
		printf("两次输入不一致！(剩余%d次)\n", 3 - retry);
		printf("请输入新密码：");
		if(scanf("%19s", pw_new) != 1) {
			printf("输入无效，操作取消。\n");
			return 0;
		}
		// 重输时也要检查是否与旧密码相同
		if(strcmp(pw_new, pw_old) == 0) {
			printf("新密码与旧密码相同，修改无效！\n");
			return 0;
		}
		printf("请确认新密码：");
		if(scanf("%19s", pw_check) != 1) {
			printf("输入无效，操作取消。\n");
			return 0;
		}
	}
	return 1;
}

// /*
// * function_name: generate_code
// * return_type  : void
// * param        : char*(验证码)
// * description  : 生成6位随机数字验证码
// */
// void generate_code(char* code) {
// 	srand((unsigned int)time(NULL));
// 	int i;
// 	for (i = 0; i < 6; i++) {
// 		code[i] = '0' + rand() % 10;
// 	}
// 	code[6] = '\0';
// }

// /*
// * function_name: send_sms_code
// * return_type  : void
// * param        : char*(手机号), char*(验证码)
// * description  : 模拟发送短信验证码(正式部署请接入短信API)
// */
// void send_sms_code(char* phone, char* code) {
// 	printf("【模拟短信】验证码已发送至手机号 %s，验证码为：%s\n", phone, code);
// 	printf("（正式部署时请接入短信服务商API）\n");
// }

// /*
// * function_name: change_password_by_sms
// * return_type  : void
// * param        : NULL
// * description  : 通过手机短信验证码修改密码
// */
// void change_password_by_sms() {
// 	char phone[12];
// 	char sms_code[7];
// 	char input_code[7];
// 	char pw_new[20];

// 	CLEAR_SCREEN();
// 	printf("**************************************\n");
// 	printf("         手机验证码修改密码\n");
// 	printf("**************************************\n");

// 	printf("请输入绑定的手机号(11位)：");
// 	scanf("%11s", phone);

// 	if (strlen(phone) != 11) {
// 		printf("手机号格式不正确！\n");
// 		getch();
// 		return;
// 	}

// 	generate_code(sms_code);
// 	send_sms_code(phone, sms_code);

// 	int try_count = 0;
// 	while (try_count < 3) {
// 		printf("请输入6位短信验证码：");
// 		scanf("%6s", input_code);

// 		if (strcmp(sms_code, input_code) == 0) {
// 			printf("验证通过！\n");
// 			break;
// 		}

// 		try_count++;
// 		if (try_count >= 3) {
// 			printf("验证码错误次数过多，操作取消。\n");
// 			getch();
// 			return;
// 		}
// 		printf("验证码错误，请重新输入(剩余%d次)：", 3 - try_count);
// 	}

// 	if (!input_new_password(pw_new)) {
// 		getch();
// 		return;
// 	}

// 	FILE* fp = fopen("password.txt", "w");
// 	if (fp == NULL) {
// 		printf("密码文件写入失败！\n");
// 		getch();
// 		return;
// 	}
// 	fprintf(fp, "%s", pw_new);
// 	fclose(fp);

// 	printf("\n密码修改成功！\n");
// 	printf("**************************************\n");
// 	getch();
// }


/*
* function_name: admin_menu 
* return_type  : int
* param        : NULL
* description  : 管理员主界面显示，并返回选择 
*/ 
int admin_menu(){
	CLEAR_SCREEN();
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
* function_name: income_check
* return_type  : void
* param        : NULL
* description  : 查看收入 
*/ 
void income_check(){
	//查看当天营业额
	CLEAR_SCREEN(); 
	
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
* function_name: view_reviews
* return_type  : void
* param        : NULL
* description  : 查看顾客评价
*/
void view_reviews() {
    CLEAR_SCREEN();
    printf("===== 查看顾客评价 =====\n\n");

    // 检查 reviews 目录是否存在
    if (access("reviews", 0) != 0) {
        printf("暂无顾客评价记录！\n");
        getch();
        return;
    }

    // 列出所有评价文件（review_table_雅座.txt）
#ifdef _WIN32
    // Windows 版本
    struct _finddata_t fileinfo;//存一个文件的信息
    intptr_t handle;//创建一个 “文件查找手柄”
    /*intptr_t = 系统自带的，专门用来存【指针 / 句柄 / 遥控器】的整数类型
	 Windows 用【数字编号】管理所有资源
	*/
	
	char pattern[100] = "reviews\\review_table_*.txt";
	// 是通配符，匹配任意字符，代替任意内容、任意字符、任意长度。
    handle = _findfirst(pattern, &fileinfo);
	//_findfirst = Windows 系统自带的【按规则找文件】函数。
	//成功找到文件 → 返回一个 >= 0 的整数（句柄 / 编号）
	//失败找到文件 → 返回 -1
    if (handle == -1) {//文件未找到。
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
        sscanf(fileinfo.name, "review_table_%d.txt", &table_no);//从字符串里 “抠出” 想要的数据
        tables[table_cnt++] = table_no;
        printf("  雅座: %d\n", table_no);
    } while (_findnext(handle, &fileinfo) == 0);//返回 0 → 成功找到下一个文件， 非 0 → 没有更多文件了。
	/*handle 里 “藏着”：
	要查哪个文件夹
	规则是什么
	查到第几个文件了
	*/
    _findclose(handle);//关闭查找句柄 / 清理资源
	/*调用 _findfirst 时：
	系统专门开辟一块内存，记录查找规则、查到哪了
	给handle 编号
	*/
#else
    // macOS/Linux 版本
    DIR* dir;//文件夹“指针” = 文件夹句柄（类似Windows的handle）
    //DIR* = Linux / Mac 系统自带的【文件夹结构体指针】
	struct dirent* entry;// 存放“单个文件信息”（类似Windows的fileinfo）
    int tables[100];
    int table_cnt = 0;

    dir = opendir("reviews");
	/*打开名叫 reviews 的评价文件夹
	→ 打开成功，返回文件夹指针
	→ 打开失败，返回 NULL
	*/
    if (dir == NULL) {
        printf("无法打开评价目录！\n");
        getch();
        return;
    }

    printf("已有评价的雅座：\n");
    printf("--------------------------\n");
    
    while ((entry = readdir(dir)) != NULL) {//读取文件夹中的每个条目，直到没有更多条目
        if (entry->d_type == DT_REG || entry->d_type == DT_UNKNOWN) {//只看普通文件，排除文件夹
            // 检查文件名是否匹配 review_table_*.txt 格式
            if (strncmp(entry->d_name, "review_table_", 13) == 0 && 
                strstr(entry->d_name, ".txt") != NULL) {//strstr = 在一个字符串里，查找【有没有包含】另一个字符串
                int table_no;
                if (sscanf(entry->d_name, "review_table_%d.txt", &table_no) == 1) {
                    tables[table_cnt++] = table_no;
                    printf("  雅座: %d\n", table_no);
                }
            }
        }
    }
    closedir(dir);

    if (table_cnt == 0) {
        printf("暂无顾客评价文件！\n");
        getch();
        return;
    }
#endif

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
#ifdef _WIN32//拼出要打开的评价文件路径
    snprintf(filename, sizeof(filename), "reviews\\review_table_%d.txt", choice);
#else
    snprintf(filename, sizeof(filename), "reviews/review_table_%d.txt", choice);
#endif
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("无法打开评价文件！\n");
        getch();
        return;
    }

    CLEAR_SCREEN();
    printf("===== %d号桌顾客评价 =====\n\n", choice);
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
		//fgets = 从文件里读取一行，并存到 line 里。
		//返回值 = 读取到的字符串（不包括换行符）
		//返回值 = NULL → 读取失败 / 读到文件末尾
        printf("%s", line);
    }
    fclose(fp);

    printf("\n按任意键返回...");
    getch();
}

