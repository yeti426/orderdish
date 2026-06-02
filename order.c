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

    //跳过备注行，否则下面的循环会读错位置
    char remark[200];
    fgets(remark, sizeof(remark), fp);

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
    if (fscanf(fp, "%d", &flag) != 1) {
         printf("订单文件格式错误！\n");
         fclose(fp);
         getch();
         return;
    }

    int ch;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF);

    // 跳过备注行
    char remark[200];
     // 使用 fgets 读取整行直到换行符
    if (fgets(remark, sizeof(remark), fp) == NULL) {
        printf("错误：读取备注行失败！\n");
        fclose(fp);
        getch();
        return;
    }

    // 打印出来看看读到了什么
    printf("备注: [%s]\n", remark);

    while (cnt < MAX_LENGTH) {
        // 检查 fscanf 的返回值
        int ret = fscanf(fp, "%d %s %lf %d %d",
               &order[cnt].no,
               order[cnt].dish_name,
               &order[cnt].dish_price,
               &order[cnt].type,
               &order[cnt].nums);
        
        if (ret != 5) {
            // 如果读取失败，打印一下当前读到了什么，方便排查
            // printf("DEBUG: 读取菜品失败，返回值: %d\n", ret);
            break; 
        }
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


    //直接视为支付成功，不再要求输入金额
    printf("\n正在处理支付...\n");
    Sleep(1000); // 模拟支付过程等待1秒
    printf("支付成功！应收：%d 元\n", final_total);

    double all_income = 0;
    double hot_income = 0;
    double cold_income = 0;
    double staple_income = 0;
    double drink_income = 0;

    for(int i = 0; i < cnt; i++) {
        double item_total = order[i].dish_price * order[i].nums;
        all_income += item_total;
        switch(order[i].type) {
            case 1: hot_income += item_total; break;
            case 2: cold_income += item_total; break;
            case 3: staple_income += item_total; break;
            case 4: drink_income += item_total; break;
        }
    }

if (all_income > 0) {
        double scale = (double)final_total / all_income;
        hot_income *= scale;
        cold_income *= scale;
        staple_income *= scale;
        drink_income *= scale;
        all_income = (double)final_total; // 确保总收入等于实收
    } else {
        all_income = 0;
    }

    // 3. 记录收入
    record_income(all_income, hot_income, cold_income, staple_income, drink_income);

    // 4. 删除订单文件 (相当于订单完成并归档)
    remove(fstr);

    // 根据选择显示支付方式名称
    char* method_str = "";
    switch(pay_method) {
        case 1: method_str = "微信"; break;
        case 2: method_str = "支付宝"; break;
        case 3: method_str = "现金"; break;
    }

    printf("支付成功！支付方式：%s\n", method_str);
    printf("订单已自动归档，营业额已更新。\n");

    // 支付成功后停留3秒，让用户看清账单
    printf("\n3秒后进入评价环节...");
    Sleep(3000); 

    // 调用评价功能
    save_review(table_no);

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
        printf("========================================\n");
        printf("         订单状态查询\n");
        printf("========================================\n");
        printf("桌号 %d 当前没有待支付的订单。\n", table_no);
        printf("(可能尚未点餐，或订单已支付并归档)\n");
        printf("========================================\n");
        getch();
        return;
    }

    int flag;
     fscanf(fp, "%d", &flag);
    fclose(fp);

    printf("========================================\n");
    printf("         订单状态查询\n");
    printf("========================================\n");
    printf("桌号: %d\n", table_no);
    
    switch (flag) {
        case 1: 
            printf("状态: 【待支付】\n");
            printf("说明: 订单已提交厨房，请前往结账。\n");
            break;
        default: 
            printf("状态: 未知 (%d)\n", flag);
            printf("说明: 订单文件可能存在异常。\n");
            break;
    }
    printf("========================================\n");

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

    //跳过备注行
    char remark[200];
    fgets(remark, sizeof(remark), fp);

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
 * 功能：保存顾客评价
 */
void save_review(int table_no) {
    system("cls");
    printf("========================================\n");
    printf("         感谢您的用餐！请评价\n");
    printf("========================================\n");
    printf("请选择您的满意等级：\n");
    printf("1. 夯 (超级满意)\n");      
    printf("2. 顶级 (非常满意)\n");     
    printf("3. 人上人 (满意)\n");      
    printf("4. NPC (一般)\n");         
    printf("5. 拉完了 (不满意)\n");    
    printf("请选择 (1-5): ");           

    int choice;
    scanf("%d", &choice);
    error_check(1, 5, &choice);
    
    char level[20];
    switch(choice) {
        case 1: strcpy(level, "夯"); break;   
        case 2: strcpy(level, "顶级"); break;
        case 3: strcpy(level, "人上人"); break;
        case 4: strcpy(level, "NPC"); break;
        case 5: strcpy(level, "拉完了"); break;
    }
    
    printf("请输入您的留言 (最多100字): \n");
    clear_stdin_buffer(); // 清空缓冲区
    char comment[200];
    fgets(comment, sizeof(comment), stdin);
    // 去除换行符
    size_t len = strlen(comment);
    if (len > 0 && comment[len - 1] == '\n') {
        comment[len - 1] = '\0';
    }
    
    // 获取当前时间
    struct tm* p = get_time();
    char time_str[50];
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", 
            p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
            p->tm_hour + 8, p->tm_min, p->tm_sec); // 简单处理时区
            
    // 确保 reviews 目录存在
    if (access("reviews", 0) != 0) {
        system("mkdir reviews");
    }
    
    // 生成评价文件名: reviews/review_桌号_时间戳.txt (简化为追加到总文件或者按桌号存)
    
    char filename[100];
    snprintf(filename, sizeof(filename), "reviews\\review_table_%d.txt", table_no);
    
    FILE* fp = fopen(filename, "a"); // 追加模式
    if (fp) {
        fprintf(fp, "--------------------------\n");
        fprintf(fp, "桌号: %d\n", table_no);
        fprintf(fp, "时间: %s\n", time_str);
        fprintf(fp, "等级: %s\n", level);
        fprintf(fp, "留言: %s\n", comment);
        fprintf(fp, "--------------------------\n");
        fclose(fp);
        
        printf("\n评价提交成功！感谢您的反馈！\n");
    } else {
        printf("\n评价保存失败。\n");
    }
    getch();
}

