#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <direct.h>


/*
 * 功能：根据桌号生成订单文件名
 */
void create_order_filename(int table_no, char* fstr,int size) {
// 确保 order 目录存在
if (access("order", 0) != 0) { // 需要 #include <io.h> 或 <unistd.h>
        system("mkdir order");
    }
// 使用反斜杠适应 Windows
    snprintf(fstr, size, "order\\%d.txt", table_no);
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
    // 检查每一行数据是否合法
    while (!feof(fp)) {
        int ret = fscanf(fp, "%d %s %lf %d %d",
                         &o.no,
                         o.dish_name,
                         &o.dish_price,
                         &o.type,
                         &o.nums);

        // 如果读到末尾或格式不对，跳出循环
        if (ret != 5) break; 

        if (o.dish_price <= 0) { fclose(fp); return 0; }
        if (o.nums <= 0) { fclose(fp); return 0; }
        if (o.type < 1 || o.type > 4) { fclose(fp); return 0; }
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
    create_order_filename(table_no, fstr, 50);

    if (!check_order_file(fstr)) {
        printf("订单文件异常或不存在，无法支付！\n");
        getch();
        return;
    }

    FILE* fp = fopen(fstr, "r");
    int flag;
    fscanf(fp, "%d", &flag);

    // 只有状态为 1 (已下单/点餐中) 才能结账
    if (flag != 1) {
        printf("订单状态不可支付（当前状态：%d）！\n", flag);
        getch();
        return;
    }

    double total = 0;
    dish_order order[MAX_LENGTH];
    int cnt = 0;

    // 读取所有菜品
    // 注意：这里重新打开文件以确保从头读取，虽然上面已经打开过一次，但为了逻辑清晰通常重新打开或seek

    fclose(fp);
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
    int final_total = (int)discounted_total;// 抹零取整
    int reduction = (int)(discounted_total - final_total);

    printf("========== 账单明细 ==========\n");
    printf("桌号：%d\n", table_no);
    printf("菜品数量：%d 项\n", cnt);
    printf("原价：%.2lf 元\n", total);
    printf("折扣：8.8 折\n");
    printf("折后价：%.2lf 元\n", discounted_total);
    printf("抹零：-%d 元\n", reduction);
    printf("实收金额：%d 元\n", final_total);
    printf("==============================\n");


    // ================= 选择支付方式 =================
    int pay_method = 0;
    printf("\n请选择支付方式：\n");
    printf("1. 微信\n");
    printf("2. 支付宝\n");
    printf("3. 现金\n");
    printf("请输入选项 (1-3)：");
    
    // 简单的输入验证循环
    while (1) {
        if (scanf("%d", &pay_method) != 1) {
            // 如果输入非数字，清空缓冲区
            while(getchar() != '\n'); 
            printf("输入无效，请重新输入 (1-3)：");
            continue;
        }
        if (pay_method >= 1 && pay_method <= 3) {
            break;
        } else {
            printf("选项不存在，请重新输入 (1-3)：");
        }
    }


    double pay;
    while (1) {
        printf("\n请输入支付金额：");
        if (scanf("%lf", &pay) != 1) {
            // 防止输入非数字导致死循环
            while(getchar() != '\n');
            printf("输入无效，请输入数字！\n");
            continue;
        }

        if (pay < final_total) {
            // 金额不足时，提示差额并允许重新输入，不退出
            printf("金额不足！还差 %.2lf 元，请重新输入。\n", final_total - pay);
        } else {
            // 金额足够，计算找零并跳出循环
            double change = pay - final_total;
            if (change > 0.001) { // 浮点数比较容差
                printf("支付成功！应收：%d 元，实付：%.2lf 元，找零：%.2lf 元\n", 
                       final_total, pay, change);
            } else {
                printf("支付成功！ exact payment.\n");
            }
            break; // 支付成功，跳出支付循环
        }
    }

    // 支付成功，更新状态为 2 (已支付)
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

    // 根据选择显示支付方式名称
    char* method_str = "";
    switch(pay_method) {
        case 1: method_str = "微信"; break;
        case 2: method_str = "支付宝"; break;
        case 3: method_str = "现金"; break;
    }


    printf("支付成功！支付方式：%s\n", method_str);
    printf("请等待商家确认。\n");
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
    create_order_filename(table_no, fstr, 50);

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

        case 1: printf("已下单(点餐中/待支付)\n"); break;
        case 2: printf("已支付(待商家确认)\n"); break;
        case 3: printf("已完成(待归档)\n"); break;
        default: printf("未知状态 (%d)\n", flag);
    }

    getch();
}




/*
 * 功能：计算订单金额 (供 admin.c 调用)
 */
void calculate_value(char* fstr, double* all,
                     double* hot, double* cold,
                     double* staple, double* drink) {

    FILE* fp = fopen(fstr, "r");
     if(!fp) return;

    int flag;
    fscanf(fp, "%d", &flag);

    dish_order o;
    while (!feof(fp)) {
    // 必须检查返回值，防止最后一行重复读取
         if (fscanf(fp, "%d %s %lf %d %d",
               &o.no,
               o.dish_name,
               &o.dish_price,
               &o.type,
               &o.nums) != 5) break;

        double sum = o.dish_price * o.nums;
        *all += sum;
        switch (o.type) {
            case 1: *hot += sum; break;
            case 2: *cold += sum; break;
            case 3: *staple += sum; break;
            case 4: *drink += sum; break;
        }
    }
    fclose(fp);
}



/*
 * 功能：商家确认订单
 * 逻辑：将状态从 2 (已支付) 改为 3 (已完成/待归档)
 */
void order_check() {
    system("cls");

    int table_no;
    printf("请输入要确认的桌号：");
    scanf("%d", &table_no);

    char fstr[50];
    create_order_filename(table_no, fstr, 50);

    if (!check_order_file(fstr)) {
        printf("订单文件异常！\n");
        getch();
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

    // 只有已支付的订单才能被确认
    if (flag != 2) {
        printf("订单状态不正确，无法完成（当前状态：%d）！\n", flag);
        getch();
        return;
    }

     // 修改状态为 3
    fp = fopen(fstr, "r+");
    if(fp) {
        fseek(fp, 0, SEEK_SET);
        fprintf(fp, "3");
        fclose(fp);
        printf("订单已确认！状态变更为：已完成。\n");
    } else {
        printf("文件打开失败！\n");
    }
    getch();
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
    create_order_filename(table_no, fstr, 50);

    if (!check_order_file(fstr)) {
        printf("订单文件异常！\n");
        getch();
        return;
    }

    FILE* fp = fopen(fstr, "r");
    int flag;
    fscanf(fp, "%d", &flag);
    fclose(fp);

    // 只有商家确认过（状态3）的订单才能归档
     if (flag != 3) {
        printf("订单尚未被商家确认（当前状态：%d ）！请先执行“确认订单”操作。\n", flag);
        getch();
        return;
    }

    double all = 0, hot = 0, cold = 0, staple = 0, drink = 0;
    calculate_value(fstr, &all, &hot, &cold, &staple, &drink);

    // 调用 admin.c 中定义的 record_income 函数
    // 注意：这需要在编译时链接 admin.c 或者在 init.h 中声明
    record_income(all, hot, cold, staple, drink);

    remove(fstr);
    printf("订单已完成！营业额已记录，订单文件已删除。\n");
    getch();
}


