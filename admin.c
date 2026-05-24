/*
 * 函数功能：根据桌号生成订单文件名
 * 示例：1.txt → order/1.txt
 */

void create_order_name(int table_num,char* fstr){
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
        fscanf(fp,"%d %s %lf %d %d",%no,name,&price,%type,%nums);

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
void orser_check(){
    system("cls");

    char fstr[50] ="order//";
    create_order_name(table_np,fstr);

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

    char fstr[50] ="order//";
    create_order_name(table_no,fstr);


    double all =0,hot =0,cold =0,staple =0,drink =0;
    calculate_value(fstr,&all,&hot,&cold,&staple,&drink);

    //记录收入（调用D的函数
    record_income(all,hot,cold,staple,drink);

    remove(fstr);
    printf("订单已完成！\n");
  }