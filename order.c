#include "init.h"
#include "tools.c"
#include <stdio.h>
#include <windows.h>



/*
 * 功能：根据桌号生成订单文件名
 */
void create_order_filename(int table_no, char* fstr,int size) {
    snprintf(fstr, size, "order/%d.txt", table_no);
}



/*
 * 功能：检查订单文件完整性
 */
int check_order_file(char* fstr) {
    FILE* fp = fopen(fstr, "r");
    if (!fp) return 0;

    int flag;
    if (fscanf(fp, "%d", &flag) != 1) {
        fclose(fp);
        return 0;
    }

    dish_order o;
    while (!feof(fp)) {
        int ret = fscanf(fp, "%d %s %lf %d %d",
                         &o.no,
                         o.dish_name,
                         &o.dish_price,
                         &o.type,
                         &o.nums);

        if (ret != 5) {
            fclose(fp);
            return 0;
        }

        if (o.dish_price <= 0) return 0;
        if (o.nums <= 0) return 0;
        if (o.type < 1 || o.type > 4) return 0;
    }

    fclose(fp);
    return 1;
}




/*
 * 功能：顾客结账（含优惠）
 */
void check_bill() {
    system("cls");

    int table_no;
    printf("请输入桌号：");
    scanf("%d", &table_no);

    char fstr[50];
    create_order_filename(table_no, fstr);

    if (!check_order_file(fstr)) {
        printf("订单文件异常，无法支付！\n");
        getch();
        return;
    }

    FILE* fp = fopen(fstr, "r");
    int flag;
    fscanf(fp, "%d", &flag);
    fclose(fp);

    if (flag != 1) {
        printf("订单不可支付！\n");
        getch();
        return;
    }

    double total = 0;
    dish_order order[MAX_LENGTH];
    int cnt = 0;

    fp = fopen(fstr, "r");
    fscanf(fp, "%d", &flag);

    while (cnt < MAX_LENGTH &&
           fscanf(fp, "%d %s %lf %d %d",
               &order[cnt].no,
               order[cnt].dish_name,
               &order[cnt].dish_price,
               &order[cnt].type,
               &order[cnt].nums) == 5) {
        total += order[cnt].dish_price * order[cnt].nums;
        cnt++;
    }
    fclose(fp);

    if(cnt == 0) {
         printf("订单为空！\n");
        getch();
        return;
    }

    // ================= 优惠计算 =================
    double discount_rate = 0.88;
    double discounted_total = total * discount_rate;
    int final_total = (int)discounted_total;
    int reduction = (int)(discounted_total - final_total);

    printf("========== 账单明细 ==========\n");
    printf("原价：%.2lf 元\n", total);
    printf("折扣：8.8 折\n");
    printf("折后价：%.2lf 元\n", discounted_total);
    printf("抹零：-%d 元\n", reduction);
    printf("实收金额：%d 元\n", final_total);
    printf("==============================\n");

    double pay;
    scanf("%lf", &pay);

    if (pay < final_total) {
        printf("金额不足！\n");
        getch();
        return;
    }

    fp = fopen(fstr, "w");
    if (!fp) {
        printf("无法更新订单状态！\n");
        getch();
        return;
    }
    fprintf(fp, "2\n");
    for (int i = 0; i < cnt; i++) { 
        fprintf(fp, "%d %s %.2lf %d %d\n",
                order[i].no,
                order[i].dish_name,
                order[i].dish_price,
                order[i].type,
                order[i].nums);
    }
    fclose(fp);

    printf("支付成功！\n");
    getch();
}



/*
 * 功能：查询订单状态
 */
void order_status() {
    system("cls");

    int table_no;
    printf("请输入桌号：");
    scanf("%d", &table_no);

    char fstr[50];
    create_order_filename(table_no, fstr);

    FILE* fp = fopen(fstr, "r");
    if (!fp) {
        printf("订单不存在！\n");
        getch();
        return;
    }

    int flag;
    fscanf(fp, "%d", &flag);
    fclose(fp);

    printf("订单状态：");
    switch (flag) {
        case 1: printf("已下单\n"); break;
        case 2: printf("已支付\n"); break;
        case 3: printf("已完成\n"); break;
        default: printf("未知状态\n");
    }

    getch();
}



/*
 * 功能：计算订单金额
 */
void calculate_value(char* fstr, double* all,
                     double* hot, double* cold,
                     double* staple, double* drink) {

    FILE* fp = fopen(fstr, "r");
    int flag;
    fscanf(fp, "%d", &flag);

    dish_order o;
    while (!feof(fp)) {
        fscanf(fp, "%d %s %lf %d %d",
               &o.no,
               o.dish_name,
               &o.dish_price,
               &o.type,
               &o.nums);

        *all += o.dish_price * o.nums;
        switch (o.type) {
            case 1: *hot += o.dish_price * o.nums; break;
            case 2: *cold += o.dish_price * o.nums; break;
            case 3: *staple += o.dish_price * o.nums; break;
            case 4: *drink += o.dish_price * o.nums; break;
        }
    }
    fclose(fp);
}



/*
 * 功能：商家确认订单
 */
void order_check() {
    system("cls");

    int table_no;
    printf("请输入要确认的桌号：");
    scanf("%d", &table_no);

    char fstr[50];
    create_order_filename(table_no, fstr);

    if (!check_order_file(fstr)) {
        printf("订单文件异常！\n");
        return;
    }

    FILE* fp = fopen(fstr, "r");
    if (!fp) {
        printf("订单不存在！\n");
        getch(); // 防止闪退
        return;
    }
    
    int flag;
    fscanf(fp, "%d", &flag);
    fclose(fp);

    if (flag != 2 && flag != 3) {
        printf("订单状态不正确，无法完成（当前状态：%d）！\n", flag);
        getch();
        return;
    }

    fp = fopen(fstr, "r+");
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "3");
    fclose(fp);

    printf("订单已确认！\n");
}




/*
 * 功能：完成订单
 */
void order_complete() {
    system("cls");

    int table_no;
    printf("请输入要完成的桌号：");
    scanf("%d", &table_no);

    char fstr[50];
    create_order_filename(table_no, fstr);

    if (!check_order_file(fstr)) {
        printf("订单文件异常！\n");
        return;
    }

    FILE* fp = fopen(fstr, "r");
    int flag;
    fscanf(fp, "%d", &flag);
    fclose(fp);

    if (flag != 2) {
        printf("订单尚未支付！\n");
        return;
    }

    double all = 0, hot = 0, cold = 0, staple = 0, drink = 0;
    calculate_value(fstr, &all, &hot, &cold, &staple, &drink);

    record_income(all, hot, cold, staple, drink);

    remove(fstr);
    printf("订单已完成！\n");
}