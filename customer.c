
/**
 *dyn留：error_check()在哪里？干嘛用的
 */
/**
 * *mzy:在mian.c里 用于检查输入是否合法
 */

//#include "tools.c"
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



/*
 *函数功能：顾客结账
 *流程：读取订单
 * 1. 读取订单
 * 2. 计算总金额
 * 3. 模拟支付
 * 4. 修改订单状态为“已支付”
 */
 
void check_bill(){
    system("cls");

    double accout =0;
    int cut =0;
    dish_order my_order[MAX_LENGTH];

    char fstr[50] ="order//";
    create_order_name(table_no,fstr);

    FILE*fp =fopen(fstr,"r");
    if(!fp){
        printf("您还没有下单!!!\n");
        getch();
        return ;

    }
    int flag_temp;
    fscanf(fp,"%d",&flag_temp);

    //防止重复付款
    if(flag_temp !=1){
        printf("订单已支付!\n");
        fclose(fp);
        getch();
        return;
    }

    //计算总金额
    while(!feof(fp)){
        facanf(fp,"%d %s %lf %d %d",
            &my_order[cnt].no,
             my_order[cnt].dish_name,
            &my_order[cnt].dish_price,
            &my_order[cnt].type,
            &my_order[cnt].nums);
        accout +=my_order[cnt].dish_price *my_order[cnt].nums;
        cnt++;

    }
    fclose(fp);

    printf("您需要支付：%.2lf 元\n",accout);

    double pay;
    printf("请输入支付金额： ");
    scanf("%lf",&pay);

    while(pay <accout){
        printf("金额不足，请补：");
        scanf("%lf",&pay);
    }
    if(pay >accout){
        printf("找零：%.2lf 元\n",pay-accout);
    }

    //将订单状态更新为“已支付”
    fp =fopen(fstr,"w");
    fprintf(fp,"2\n");  //2 =已支付

    for(int i=0;i<cnt-1;i++){
        fprintf(fp,"%d %s %.2lf %d %d\n",
             my_order[i].no,
             my_order[i].dish_name,
             my_order[i].dish_price,
             my_order[i].type,
             my_order[i].nums);
    
    }
    fclose(fp);

    printf("支付成功，等待商家确认！！\n");
    getch();
}




/*
 * 功能：查询订单当前状态
 */
void order_status(){
    system("cls");

    char fstr[50] ="order//";
    create_order_name(table_no,fstr);

    FILE*fp =fopen(fstr,"r");
    if(!fp){
        printf("您还没有下单!!!\n");
        getch();
        return ;
    }

    int flag;
    fscanf(fp,"%d",&flag);
    fclose(fp);

    printf("订单状态：");
    switch(flag){
        case 1:printf("已下单，待支付\n");break;
        case 2:printf("已支付，等待商家确认\n");break;
        case 3:printf("商家已确认，正在备餐\n");break;
        default:printf("未知状态\n");
    }

    getch();
}
