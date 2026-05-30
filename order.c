//C部分：订单与支付管理

/*
 * 函数功能：根据桌号生成订单文件名
 * 示例：1.txt → order/1.txt
 */

void create_order_filename(int table_num,char* fstr){
    char str[5];

    //讲桌号转为字符串
    itoa(table_num,str,10);

    //拼接路径和文件名
    strcpy(fstr,"order//");
    strcat(fstr,str);
    strcat(fstr,".txt");

}



/*
 * 功能：计算订单总金额及各分类金额
 */
void calculate_value(char* fstr,double* all,
                     double* hot,double* cold,
                     double* staple,double* drink){

    FILE* fp =fopen(fstr,"r");
    int temp;
    fscanf(fp,"%d",&temp);

    int no,type,nums;
    char name[20];
    double price;

    while(!feof(fp)){
        fscanf(fp,"%d %s %lf %d %d",&no,name,&price,&type,&nums);

        *all +=price *nums;

        switch(type){
            case 1: *hot +=price *nums;break;
            case 2: *cold +=price *nums;break;
            case 3: *staple +=price *nums;break;
            case 4: *drink +=price *nums;break;

        }
    }
    fclose(fp);
}




/*
 * 功能：管理员确认订单
 * 状态从 2 → 3
 */
void order_check(){
    system("cls");
	int table_no;
    printf("请输入要确认的桌号：");
    scanf("%d", &table_no);

    char fstr[50] ="order//";
    create_order_filename(table_no,fstr);

    FILE* fp =fopen(fstr,"r");
    if(!fp) return;

    int flag;
    fscanf(fp,"%d",&flag);
    fclose(fp);

    if(flag ==2){
        fp =fopen(fstr,"r+");
        fseek(fp,0,SEEK_SET);
        fprintf(fp,"3");
        fclose(fp);
        printf("订单已确认！！\n");
    }
}




/*
 * 功能：完成订单
 * 1. 计算收入
 * 2. 记录收入
 * 3. 删除订单文件
 */
void order_complete(){
    system("cls");
	int table_no;
    printf("请输入要确认的桌号：");
    scanf("%d", &table_no);

    char fstr[50] ="order//";
    create_order_name(table_no,fstr);


    double all =0,hot =0,cold =0,staple =0,drink =0;
    calculate_value(fstr,&all,&hot,&cold,&staple,&drink);

    //记录收入（调用D的函数
    record_income(all,hot,cold,staple,drink);

    remove(fstr);
    printf("订单已完成！\n");
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
    int cnt =0;
    dish_order my_order[MAX_LENGTH];

    char fstr[50] ="order//";
    create_order_filename(table_no,fstr);

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
        fscanf(fp,"%d %s %lf %d %d",
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