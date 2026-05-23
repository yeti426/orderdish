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


