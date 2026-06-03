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
    fdate[0] = '\0';
    struct tm* p = get_time(); 
    int mday = p->tm_mday;
    if (p->tm_hour + 8 >= 24) mday -= 1;
    
    char date[20] = "";
    char year[5], month[5], day[5];
    sprintf(year, "%d", p->tm_year + 1900);
    sprintf(month, "%d", p->tm_mon + 1);
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
    fprintf(fp, "%lf\n%lf\n%lf\n%lf\n%lf\n", a, b, c, d, e);
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
                    
    FILE *fp1;
    fp1 = fopen(fdate, "r");
    if(fp1 == NULL) {
        fp1 = fopen(fdate, "w");
        if(fp1 == NULL) {
            printf("错误: 无法创建收入文件 %s\n", fdate);
            return;
        }
        format_input_income(fp1, account, account_hot_dish, account_cold_dish, account_staple_food, account_drink);
        fclose(fp1);
    } else {
        if(!format_output_income(fp1, &all_income, &hot_dish_income, &cold_dish_income, &staple_food_income, &drink_income)) {
            printf("错误: 收入文件 %s 格式错误，本次数据可能丢失！\n", fdate);
            fclose(fp1);
            return;
        }
        fclose(fp1);
        all_income += account;
        hot_dish_income += account_hot_dish;
        cold_dish_income += account_cold_dish;
        staple_food_income += account_staple_food;
        drink_income += account_drink;
        
        fp1 = fopen(fdate, "w");
        if(fp1 == NULL) {
            printf("错误: 无法写入收入文件 %s\n", fdate);
            return;
        }
        format_input_income(fp1, all_income, hot_dish_income, cold_dish_income, staple_food_income, drink_income);
        fclose(fp1); 
    } 
}