#include "init.h"
#include <sys/stat.h>

// 函数声明
void create_date_filename(char* fdate);
void format_input_income(FILE* fp, double a, double b, double c, double d, double e);
int format_output_income(FILE* fp, double* a, double* b, double* c, double* d, double* e);
int record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink);

/* 确保目录存在（跨平台） */
static void ensure_dir(const char* dir) {//确保指定目录存在，不存在就创建，是一个跨平台的防御性目录初始化工具。
   //static 表示该函数仅在本文件（income.c）内可见，不会被其他 .c 文件链接调用。
    struct stat st; //struct stat 是操作系统（Linux/Windows）系统头文件自带的、专门存放【文件 / 文件夹全部属性】的系统结构体。
    if(stat(dir, &st) != 0) {//stat(...) != 0 → 文件夹不存在。
#ifdef _WIN32
        mkdir(dir);//mkdir = 创建文件夹。
#else
        mkdir(dir, 0755);
#endif
    }
}

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
int record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink) {
    double all_income = 0.0, staple_food_income = 0.0, hot_dish_income = 0.0, cold_dish_income = 0.0, drink_income = 0.0;
    char fdate[50] = "";
    create_date_filename(fdate); 

    // 确保 income 目录存在，防止 fopen 失败
    ensure_dir("income");
                    
    FILE *fp;
    fp = fopen(fdate, "r");
    if(fp == NULL) {//今天第一次有人买单 → 新建文件 → 写入第一笔收入
        fp = fopen(fdate, "w");
        if(fp == NULL) {
            printf("错误: 无法创建收入文件 %s\n", fdate);
            return 0;
        }
        format_input_income(fp, account, account_hot_dish, account_cold_dish, account_staple_food, account_drink);
        fclose(fp);
        return 1;
    } else {
        if(!format_output_income(fp, &all_income, &hot_dish_income, &cold_dish_income, &staple_food_income, &drink_income)) {
            printf("错误: 收入文件 %s 格式错误，本次数据可能丢失！\n", fdate);
            fclose(fp);
            return 0;
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
            return 0;
        }
        format_input_income(fp, all_income, hot_dish_income, cold_dish_income, staple_food_income, drink_income);
        fclose(fp);

        // Windows 下 rename 不能覆盖已存在文件，需先删除；Linux/macOS 可直接覆盖
#ifdef _WIN32
        remove(fdate);
#endif
        if(rename(tmpname, fdate) != 0) {
            printf("错误: 无法保存收入文件 %s\n", fdate);
            return 0;
        }
        return 1;
    } 
}