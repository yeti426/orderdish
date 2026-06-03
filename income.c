#include "init.h"

// 函数声明
void create_date_filename(char* fdate);
void format_input_income(FILE* fp, double a, double b, double c, double d, double e);
int format_output_income(FILE* fp, double* a, double* b, double* c, double* d, double* e);
void record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink);

/*
* function_name: create_date_filename
* 自动获取当天日期，生成文件名 如 income/20231027.txt
*/
void create_date_filename(char* fdate) {
    fdate[0] = '\0';//把存文件名的数组清空
    struct tm* p = get_time(); 
    int mday = p->tm_mday;//默认当天日期
    if (p->tm_hour + 8 >= 24) mday -= 1;//UTC 时间 +8 = 北京时间
    
    char date[20] = "";
    char year[5], month[5], day[5];
    sprintf(year, "%d", p->tm_year + 1900);// tm_year 从 1900 起算
    sprintf(month, "%d", p->tm_mon + 1);// tm_mon 从 0 起算
    sprintf(day, "%d", mday);
    
    strcat(date, year);
    strcat(date, month);
    strcat(date, day);
    strcat(date, ".txt");
    
    // 使用文件夹路径
    strcpy(fdate, "income//");
    strcat(fdate, date);
}

/*
* function_name: format_input_income 
* 将5个double数据格式化输入到文件中 
*/
void format_input_income(FILE* fp, double a, double b, double c, double d, double e) {
    if(fprintf(fp, "%lf\n%lf\n%lf\n%lf\n%lf\n", a, b, c, d, e) < 5) {
        printf("警告: 写入收入文件可能不完整！\n");
    }
}

/*
* function_name: format_output_income 
* 将5个double数据从文件中读取，返回1成功0失败 
*/
int format_output_income(FILE* fp, double* a, double* b, double* c, double* d, double* e) {
    if(fscanf(fp, "%lf\n", a) != 1) return 0;
    if(fscanf(fp, "%lf\n", b) != 1) return 0;
    if(fscanf(fp, "%lf\n", c) != 1) return 0;
    if(fscanf(fp, "%lf\n", d) != 1) return 0;
    if(fscanf(fp, "%lf\n", e) != 1) return 0;
    return 1;
}

/*
* function_name: record_income
* 将单笔订单收入信息累加记录至收入文件中
*/
void record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink) {
    double all_income = 0.0, staple_food_income = 0.0, hot_dish_income = 0.0, cold_dish_income = 0.0, drink_income = 0.0;
    char fdate[50] = "";
    create_date_filename(fdate); 
                    
    FILE *fp;
    fp = fopen(fdate, "r");
    if(fp == NULL) {//今天第一次有人买单 → 新建文件 → 写入第一笔收入
        fp = fopen(fdate, "w");
        if(fp == NULL) {
            printf("错误: 无法创建收入文件 %s\n", fdate);
            return;
        }
        format_input_income(fp, account, account_hot_dish, account_cold_dish, account_staple_food, account_drink);
        fclose(fp);
    } else {
        if(!format_output_income(fp, &all_income, &hot_dish_income, &cold_dish_income, &staple_food_income, &drink_income)) {
            printf("错误: 收入文件 %s 格式错误，本次数据可能丢失！\n", fdate);
            fclose(fp);
            return;
        }
        fclose(fp);

        all_income += account;
        hot_dish_income += account_hot_dish;
        cold_dish_income += account_cold_dish;
        staple_food_income += account_staple_food;
        drink_income += account_drink;

        // 先写临时文件，成功后再替换，避免写一半崩溃导致数据全丢
        char tmpname[60];
        strcpy(tmpname, fdate);
        strcat(tmpname, ".tmp");
        fp = fopen(tmpname, "w");
        if(fp == NULL) {
            printf("错误: 无法写入收入文件 %s\n", fdate);
            return;
        }
        format_input_income(fp, all_income, hot_dish_income, cold_dish_income, staple_food_income, drink_income);
        fclose(fp);

        if(rename(tmpname, fdate) != 0) {
            printf("错误: 无法保存收入文件 %s\n", fdate);
        }
    } 
}