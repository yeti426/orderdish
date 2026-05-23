
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

    printf("订单状态：")；
    switch(flag){
        case 1:printf("已下单，待支付\n");break;
        case 2:printf("已支付，等待商家确认\n");break;
        case 3:printf("商家已确认，正在备餐\n");break;
        default:printf("未知状态\n");
    }

    getch();
}
