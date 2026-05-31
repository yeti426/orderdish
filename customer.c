
#include "init.h"

//变量声明 
int table_no;                 //餐台号 
extern char hot_dish_filename[20];
extern char cold_dish_filename[20];
extern char staple_food_filename[20];
extern char drink_filename[20]; 
extern shopping_cart cart;     //购物车全局变量
//函数声明 
void cold_dish();             //凉菜 
void hot_dish();              //热菜 
void staple_food();           //主食 
void drink();                 //饮品 

void menu_controller(dish_menu* , int);       //菜单交互 
void display_menu(dish_menu* , int);          //显示菜单信息
void create_order(dish_menu* , int , int);    //生成订单
void read_menu(char* , dish_menu* , int*);    //从文件中读取菜单 

//外部函数声明   
extern void error_check(int,int,int*);
extern void greet(struct tm* p,int);
extern struct tm* get_time();
extern void create_order_filename(int,char*,int); 
extern void check_bill();            //支付订单 
extern void order_status();          //订单状态
extern void display_cart();          //查看已点菜品


void customer_form(){
    //选桌
	system("cls");
	printf("**************************************\n");
	printf("请输入您的餐台号：");
	scanf("%d",&table_no);

    //欢迎信息
	system("cls");
	int greet_type = 1;
	struct tm* p = get_time();
	greet(p,greet_type);

	//询问顾客是否点餐 
    char start_choice;
    printf("是否开始点餐？(Y/N): ");
    scanf(" %c", &start_choice);
    if(start_choice == 'N' || start_choice == 'n'){
       display_cart(); //查看已点菜品
       check_bill(); //查看账单
       return;
    }
	
    //选择服务
	while(1){
		int choice;
	    printf("餐台：%d 请选择您的操作\n",table_no);
	    printf("1.热菜\n"); 
	    printf("2.凉菜\n"); 
	    printf("3.主食\n");
	    printf("4.饮品\n"); 
	    printf("5.结账\n"); 
	    printf("6.订单状态\n");
	    printf("7.退出\n");
	    printf("请在输入想进行的操作:");
	    scanf("%d",&choice);

	    error_check(1,7,&choice);

        //打开菜单界面 
		switch(choice){
			case 1:hot_dish();break;
			case 2:cold_dish();break;
			case 3:staple_food();break;
			case 4:drink();break;
			case 5:check_bill();break;
			case 6:order_status();break;
			case 7:break;		
		}
	}
}


void read_menu(char* filename , dish_menu* dm , int* cnt){
	FILE* fp;
	fp = fopen(filename , "r");
	while(!feof(fp)){
		fscanf(fp,"%d",&dm[*cnt].no);
		fscanf(fp,"%s",dm[*cnt].dish_name);
		fscanf(fp,"%lf",&dm[*cnt].dish_price);
		fscanf(fp,"%d",&dm[*cnt].type);
		(*cnt)++;
	}
	fclose(fp);
} 


/*
* 名称：cold_dish、hot_dish、staple_food、drink
* 功能：定义四个种类的菜品及相关操作
*/
void cold_dish(){
	//采用结构体数组保存读取到的菜单品信息
	dish_menu cold_dish_menu[MAX_LENGTH];
	//设置一个计数器保存菜品实际数量 
	int cnt = 0;
	//打开 cold_dish.txt 并读取数据进行显示
	read_menu(cold_dish_filename,cold_dish_menu,&cnt);
	//显示菜单并执行点菜操作
	menu_controller(cold_dish_menu,cnt); 
}
void hot_dish(){
	dish_menu hot_dish_menu[MAX_LENGTH]; 
	int cnt = 0; 
	read_menu(hot_dish_filename,hot_dish_menu,&cnt);
	menu_controller(hot_dish_menu,cnt); 
}
void staple_food(){
	dish_menu staple_food_menu[MAX_LENGTH]; 
	int cnt = 0; 
	read_menu(staple_food_filename,staple_food_menu,&cnt);
	menu_controller(staple_food_menu,cnt); 
}
void drink(){
	dish_menu drink_menu[MAX_LENGTH]; 
	int cnt = 0; 
	read_menu(drink_filename,drink_menu,&cnt);
	menu_controller(drink_menu,cnt); 
}


/*
* 名称：display_menu
* 功能：打印并展示菜单
*/ 
void display_menu(dish_menu *dm, int cnt){
	int i;
	
	system("cls");
	printf("----------------------------------------------------------\n");
    printf("序号      菜品编号      菜品名称      价格\n");
    printf("----------------------------------------------------------\n");
    
    for(i = 0; i < cnt; i++){
    	printf(" %d\t %d\t %s\t %.2lf\n", i+1, dm[i].no, dm[i].dish_name, dm[i].dish_price);
	}
	
	printf("----------------------------------------------------------\n");
}


/*
*名称：menu_controller
*功能：实现菜单内容相关交互 
*/ 
void menu_controller(dish_menu *dm, int cnt){
	int choice;
	int index;
	int nums;
	
	while(1){
		display_menu(dm, cnt);
		
		printf("1.加入购物车\n");
		printf("2.查看购物车\n");
		printf("3.返回\n");
		printf("请选择：");
		scanf("%d",&choice); 
		error_check(1, 3, &choice);
		
		switch(choice){
			case 1:
				if(cart.kitchen_received){
					printf("订单已提交，不能继续修改购物车！\n");
					getch();
					break;
				}
				
				printf("请输入菜品序号：");
				scanf("%d",&index);
				
				if(index < 1 || index > cnt){
					printf("无效的菜品序号!\n");
					getch();
					break;
				}
				
				printf("请输入数量：");
				scanf("%d",&nums);
				
				if(nums <= 0){
					printf("数量必须大于0！\n");
					getch();
					break;
				} 
				
				add_to_cart(dm, index-1, nums);
				break;
				
			case 2:
				display_cart();
				break;
			
			case 3:
				return;
		}
	}
} 


/*
*名称：create_order 
*功能：根据顾客选择的菜品生成订单文件
*/ 
void create_order(dish_menu *dm, int type, int count){
	//采用点菜订单的数据结构保存点菜信息 
	int cnt = 0;
	dish_order new_order[MAX_LENGTH];  
	
	int choice = 0;
	int temp_no = 0;
	int temp_num = 0;
	
	if(dm == NULL || count <= 0){
		printf("菜单数据无效！\n");
		return;
	}
	
	do{
		int flag = 0;
		int i;
		
		//检测菜品编号是否存在
		printf("请输入菜品编号：");
		scanf("%d", &temp_no);
		
		for(i = 0; i < count; i++){
			if(dm[i].no == temp_no){
				flag = 1;
				new_order[cnt].no = dm[i].no;
				strcpy(new_order[cnt].dish_name, dm[i].dish_name);
				new_order[cnt].dish_price = dm[i].dish_price;
				new_order[cnt].type = dm[i].type;
				break;
			}
		}
			
		if(flag == 0){
			printf("输入的菜品编号不存在，请重新输入。\n");
			continue;
		}
		
		//菜品编号存在时输入点菜数量
		printf("请输入点菜数量：");
		scanf("%d", &temp_num);
		
		if(temp_num <= 0){
			printf("数量必须大于0，请重新输入。\n");
			continue;
		}
		
		new_order[cnt].nums = temp_num;
		cnt++;
		
		//判断是否继续点菜
		printf("是否继续点菜？ 1.是 2.否 ：");
		scanf("%d",&choice);

		error_check(1, 2, &choice);
		
	}while(choice != 2 && cnt < MAX_LENGTH);	
	
	//end test
	char fstr[50] = "order//";
	create_order_filename(table_no, fstr, 50);
	//首先判断该文件是否存在，如不存在则创建一个，并设置标志位为1
	FILE *fp = fopen(fstr, "r");
	if(fp == NULL){ 
		fp = fopen(fstr, "w");
		if(fp == NULL){
			printf("订单文件创建失败！\n");
			return;
		}
		fprintf(fp, "1\n");
		fclose(fp);
	}else{
		int flag;
		if(fscanf(fp,"%d",&flag) != 1){
			fclose(fp);
			printf("订单文件格式错误!\n");
			return;
		}
		fclose(fp);
		
		if(flag != 1){
			printf("订单已提交或已完成，不能继续修改！\n");
			return; 
		}
	}
	
	//而后以追加模式重新打开文件，将订单信息添加进去 
	fp = fopen(fstr, "a");
	if(fp == NULL){
		printf("订单文件打开失败！\n");
		return; 
	}
	
	int i = 0 ;
	for(i = 0; i < cnt; i++){
		fprintf(fp, "%d %s %.2lf %d %d\n", new_order[i].no, new_order[i].dish_name, new_order[i].dish_price, new_order[i].type,new_order[i].nums); 
	}
	fclose(fp); 
}