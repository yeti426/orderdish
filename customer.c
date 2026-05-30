
/*
补个login，区分admin和customer
*/

#include "tools.c"

//变量声明 
int table_no;                 //餐台号 
extern char hot_dish_filename[20];
extern char cold_dish_filename[20];
extern char staple_food_filename[20];
extern char drink_filename[20]; 
//函数声明 
int customer_menu();          //菜单界面
void cold_dish();             //凉菜 
void hot_dish();              //热菜 
void staple_food();           //主食 
void drink();                 //饮品 
void over_view();             //总览 
void check_bill();            //支付订单 
void order_status();          //订单状态
void page_controller(dish_menu* , int);       //菜单分页 
int display_menu(dish_menu* , int , int);     //显示菜单信息
void create_order(dish_menu* , int ,int);     //生成订单 
void read_menu(char* , dish_menu* , int*);             //从文件中读取菜单 

//外部函数声明   
extern void error_check(int,int,int*);
extern void greet(struct tm* p,int);
extern struct tm* get_time();
extern void create_order_filename(int,char*); 

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
	
    //选择服务
    int choice;
	printf("餐台：%d 请选择您的操作\n",table_no);
	printf("1.热菜\n"); 
	printf("2.凉菜\n"); 
	printf("3.主食\n");
	printf("4.饮品\n"); 
	printf("5.总览\n"); 
	printf("6.结账\n"); 
	printf("7.订单状态\n");
	printf("8.退出\n");
	printf("请在输入想进行的操作:");
	scanf("%d",&choice);

	error_check(1,8,&choice);

	//打开菜单界面 
	do{
		switch(choice){
			case 1:hot_dish();break;
			case 2:cold_dish();break;
			case 3:staple_food();break;
			case 4:drink();break;
			case 5:over_view();break;
			case 6:check_bill();break;
			case 7:order_status();break;
			default:break;		
		}
	}while(choice != 8);//输入8退出
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


/////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void cold_dish(){
	//采用结构体数组保存读取到的菜单品信息
	dish_menu cold_dish_menu[MAX_LENGTH]; 
	
	//设置一个计数器保存菜品实际数量
	int cnt = 0 ; 
	
	//打开 cold_dish.txt 并读取数据进行显示
	read_menu(cold_dish_filename , cold_dish_menu , &cnt);
	
	//采用分页控制显示菜单并执行点菜操作
	page_controller(cold_dish_menu,cnt); 
	 
}

void hot_dish(){
	dish_menu hot_dish_menu[MAX_LENGTH]; 
	int cnt = 0 ; 
	read_menu(hot_dish_filename , hot_dish_menu , &cnt);
	page_controller(hot_dish_menu,cnt); 
}
void staple_food(){
	
	dish_menu staple_food_menu[MAX_LENGTH]; 
	int cnt = 0 ; 
	
	read_menu(staple_food_filename , staple_food_menu , &cnt);
	page_controller(staple_food_menu, cnt); 
}
void drink(){
	
	dish_menu drink_menu[MAX_LENGTH]; 
	int cnt = 0 ; 
	read_menu(drink_filename , drink_menu , &cnt);
	page_controller(drink_menu,cnt); 
}


void page_controller(dish_menu *dm , int cnt){
	//在这里设定每页显示的菜品数量 
	int record_num = 10;
	//设定部分显示相关变量 
	int base = 0;
	int start,end;
	start = base;
	end = start + record_num > cnt ? cnt-1 : start + record_num; 
	
	int choice;
	do{
		choice = display_menu(dm , start , end);
		switch(choice){
			case 1:{
				if(end - start < record_num){
					end = end - (end - start);
				}else{
					end -= record_num;	
				}
				start -= record_num;
				break;
			}
			case 2:{
				start += record_num;
				end = start + record_num > cnt ? cnt-1 : start+record_num;
				break;
			}
            case 3:{
				int mode = dm[0].type;
				create_order(dm,mode,cnt);
				break;
			}
			case 4:break;
		}	
	} while(choice != 4);
} 

/*

dyn:最好是删掉翻页功能，如果我们菜不多的话

*/
int display_menu(dish_menu *dm, int start , int end){
	int i;
	int choice;
	system("cls");
	printf("----------------------------------------------------------\n");
	printf("序号  菜品编号    菜品名称      价格\n");
	for( i = start ; i < end ; i++){
		printf(" %d\t %d\t  %s\t%.2lf\n",i+1,dm[i].no,dm[i].dish_name,dm[i].dish_price);
	}
	printf("----------------------------------------------------------\n"); 
	
	//添加控制逻辑，防止误判 
	if(start == 0 && end-start < 10){
		//printf("1.上一页\n");
		//printf("2.下一页\n");
		printf("3.点菜\n");
		printf("4.退出\n");
		scanf("%d",&choice);
		//输入异常检测 
		while(choice<3 || choice > 4){
			printf("输入错误，请重新输入：");
			scanf("%d",&choice);
		} 
	}
	else if(start == 0){//无上一页选项 
		//printf("1.上一页\n");
		printf("2.下一页\n");
		printf("3.点菜\n");
		printf("4.退出\n");
		scanf("%d",&choice);
		//输入异常检测 
		while(choice<2 || choice > 4){
			printf("输入错误，请重新输入：");
            scanf("%d",&choice);
		} 
	}
	else if(end - start != 10){//无下一页选项 
		printf("1.上一页\n");
		//printf("2.下一页\n");
		printf("3.点菜\n");
		printf("4.退出\n");
		scanf("%d",&choice);
		//输入异常检测 
		while(choice<1 || choice > 4 || choice == 2){
			printf("输入错误，请重新输入：");
			scanf("%d",&choice);
		} 
	}
	else{//正常显示 
		printf("1.上一页\n");
		printf("2.下一页\n");
		printf("3.点菜\n");
		printf("4.退出\n");
		scanf("%d",&choice);
		//输入异常检测 
		while(choice<1 || choice > 4){
			printf("输入错误，请重新输入：");
			scanf("%d",&choice);
		} 
	}
	return choice; 
}

void create_order(dish_menu *dm , int mode,int count){
	//采用点菜数据结构保存 点菜信息 
	int cnt = 0;
	dish_order new_order[MAX_LENGTH];  
	
	//根据mode打开文件
	char filename[20];
	switch(mode){
		case 1:strcpy(filename,hot_dish_filename);break;
		case 2:strcpy(filename,cold_dish_filename);break;
		case 3:strcpy(filename,staple_food_filename);break;
		case 4:strcpy(filename,drink_filename);break;
	} 
	FILE* fp;
	fp = fopen(filename,"r");
	
	int choice;
	int temp_no;
	int temp_num;
	do{
		printf("请输入菜品编号：");
		scanf("%d",&temp_no);
		//检测菜品编号是否存在
		int flag = 0;
		do{
			int i;
			for( i = 0 ; i < count ; i++){
				if(dm[i].no == temp_no){
					flag = 1;
					new_order[cnt].no = dm[i].no;
					strcpy(new_order[cnt].dish_name , dm[i].dish_name);
					new_order[cnt].dish_price = dm[i].dish_price;
					new_order[cnt].type = dm[i].type;
					break;
                    	}
			}
			
			if(flag == 0){
				printf("输入的菜品编号不存在，请输入正确的菜品编号：");
				scanf("%d",&temp_no);
			}
		} while(flag == 0);
		
		//菜品编号存在时输入点菜数量
		printf("请输入点菜数量：");
		scanf("%d",&temp_num); 
		new_order[cnt].nums = temp_num;
		cnt++;
		
		//判断是否继续点菜
		printf("是否继续点菜？ 1.是 2.否 ：");
		scanf("%d",&choice);

		error_check(1,2,&choice);
		
	} while(choice != 2);	
	fclose(fp);
	
}