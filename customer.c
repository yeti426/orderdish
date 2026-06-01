
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


void customer_form() {
    // 选桌
    system("cls");
    printf("**************************************\n");
    printf("请输入您的餐台号：");
    scanf("%d", &table_no);
    
    // 初始化购物车
    init_cart();
    
    // 欢迎信息
    system("cls");
    int greet_type = 1;
    struct tm* p = get_time();
    greet(p, greet_type);
    
    // 选择服务 - 简化后的菜单
    int choice;
    do {
        system("cls");
        printf("========================================\n");
        printf("         餐台：%d 服务菜单\n", table_no);
        printf("========================================\n");
        printf("1. 点菜\n");
        printf("2. 查看购物车\n");
        printf("3. 查看账单\n");
        printf("4. 结账（呼叫服务员）\n");
        printf("5. 退出\n");
        printf("========================================\n");
        printf("请选择: ");
        scanf("%d", &choice);
        
        error_check(1, 5, &choice);
        
        switch(choice) {
            case 1: 
                ordering_menu();  // 点菜子菜单
                break;
            case 2: 
                display_cart();   // 查看购物车
                break;
            case 3: 
                view_bill();      // 查看账单（已点菜品）
                break;
            case 4: 
                checkout();       // 结账
                break;
            case 5: 
                break;
        }
    } while(choice != 5);
}
////////////////////////////////////////////////////////////////////
/*
 * 函数功能：点菜子菜单（整合四类菜品），把点菜功能独立出来
 */
void ordering_menu() {
    int choice;
    do {
        system("cls");
        printf("========================================\n");
        printf("         点菜菜单\n");
        printf("========================================\n");
        printf("1. 热菜\n");
        printf("2. 凉菜\n");
        printf("3. 主食\n");
        printf("4. 饮品\n");
        printf("5. 返回主菜单\n");
        printf("========================================\n");
        printf("请选择: ");
        scanf("%d", &choice);
        
        error_check(1, 5, &choice);
        
        switch(choice) {
            case 1: hot_dish(); break;
            case 2: cold_dish(); break;
            case 3: staple_food(); break;
            case 4: drink(); break;
            case 5: break;
        }
    } while(choice != 5);
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
 * 函数功能：查看已点菜品（显示所有已点菜品）
 */
void view_bill() {
    char filename[50] = "order//";
    create_order_filename(table_no, filename, sizeof(filename));
    
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        system("cls");
        printf("========================================\n");
        printf("         账单 (桌号: %d)\n", table_no);
        printf("========================================\n");
        printf("\n您还没有点菜！\n");
        printf("\n========================================\n");
        printf("1. 去点菜\n");
        printf("2. 返回主菜单\n");
        printf("请选择: ");
        
        int choice;
        scanf("%d", &choice);
        error_check(1, 2, &choice);
        
        if (choice == 1) {
            ordering_menu();
        }
        return;
    }
    
    // 读取订单状态
    int order_status;
    fscanf(fp, "%d", &order_status);
    
    dish_order orders[MAX_LENGTH];
    int count = 0;
    double total = 0;
    
    // 读取所有订单项
    while (count < MAX_LENGTH && 
           fscanf(fp, "%d %s %lf %d %d",
                  &orders[count].no,
                  orders[count].dish_name,
                  &orders[count].dish_price,
                  &orders[count].type,
                  &orders[count].nums) == 5) {
        
        orders[count].subtotal = orders[count].dish_price * orders[count].nums;
        total += orders[count].subtotal;
        count++;
    }
    fclose(fp);
    
    // 获取订单状态文字
    char* status_text;
    switch(order_status) {
        case 1: status_text = "待支付"; break;
        case 2: status_text = "已支付，等待确认"; break;
        case 3: status_text = "商家已确认，制作中"; break;
        default: status_text = "未知状态"; break;
    }
    
    system("cls");
    printf("========================================\n");
    printf("         账单 (桌号: %d)\n", table_no);
    printf("========================================\n");
    printf("订单状态: %s\n", status_text);
    printf("\n");
    
    if (count == 0) {
        printf("暂无已点菜品\n");
    } else {
        printf("%-4s %-6s %-10s %-8s %-6s %-10s\n", 
               "序号", "编号", "菜品名称", "单价", "数量", "小计");
        printf("----------------------------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf\n",
                   i + 1,
                   orders[i].no,
                   orders[i].dish_name,
                   orders[i].dish_price,
                   orders[i].nums,
                   orders[i].subtotal);
        }
        
        printf("----------------------------------------------------------\n");
        printf("\n总金额: %.2lf 元\n", total);
    }
    
    printf("\n========================================\n");
    printf("1. 去结账\n");
    printf("2. 继续点菜\n");
    printf("3. 返回主菜单\n");
    printf("请选择: ");
    
    int choice;
    scanf("%d", &choice);
    error_check(1, 3, &choice);
    
    switch(choice) {
        case 1:
            checkout();  // 去结账
            break;
        case 2:
            ordering_menu();  // 继续点菜
            break;
        case 3:
            break;
    }
}


/*
 * 函数功能：结账（呼叫服务员）
 */
void checkout() {
    char filename[50] = "order//";
    create_order_filename(table_no, filename, sizeof(filename));
    
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        system("cls");
        printf("========================================\n");
        printf("         结账 (桌号: %d)\n", table_no);
        printf("========================================\n");
        printf("\n您还没有点菜，无法结账！\n");
        printf("\n按任意键返回...");
        getch();
        return;
    }
    
    // 读取订单状态
    int order_status;
    fscanf(fp, "%d", &order_status);
    
    dish_order orders[MAX_LENGTH];
    int count = 0;
    double total = 0;
    
    // 读取所有订单项
    while (count < MAX_LENGTH && 
           fscanf(fp, "%d %s %lf %d %d",
                  &orders[count].no,
                  orders[count].dish_name,
                  &orders[count].dish_price,
                  &orders[count].type,
                  &orders[count].nums) == 5) {
        
        orders[count].subtotal = orders[count].dish_price * orders[count].nums;
        total += orders[count].subtotal;
        count++;
    }
    fclose(fp);
    
    system("cls");
    printf("========================================\n");
    printf("         结账 (桌号: %d)\n", table_no);
    printf("========================================\n");
    printf("\n");
    
    if (count == 0) {
        printf("账单为空！\n");
    } else {
        printf("%-4s %-6s %-10s %-8s %-6s %-10s\n", 
               "序号", "编号", "菜品名称", "单价", "数量", "小计");
        printf("----------------------------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf\n",
                   i + 1,
                   orders[i].no,
                   orders[i].dish_name,
                   orders[i].dish_price,
                   orders[i].nums,
                   orders[i].subtotal);
        }
        
        printf("----------------------------------------------------------\n");
        printf("\n总金额: %.2lf 元\n", total);
    }
    
    printf("\n========================================\n");
    printf("已呼叫服务员，请稍候...\n");
    printf("服务员将为您办理结账手续\n");
    printf("\n按任意键返回主菜单...");
    getch();
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