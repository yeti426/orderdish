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





