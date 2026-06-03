#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// --- macOS/Linux 兼容性处理 ---
#ifdef _WIN32
    #include <conio.h>   // Windows: getch()
    #include <windows.h> // Windows: Sleep(), system("cls")
#else
    #include <termios.h> // macOS/Linux: 模拟 getch()
    #include <unistd.h>  // macOS/Linux: sleep()

    // 模拟 Windows 的 getch()
    static inline int getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // 禁用缓冲和回显
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    // 模拟 Windows 的 Sleep() (毫秒)
    static inline void Sleep(unsigned int milliseconds) {
        usleep(milliseconds * 1000);
    }
#endif

// 跨平台清屏宏
#ifdef _WIN32
    #define CLEAR_SCREEN() system("cls")
#else
    #define CLEAR_SCREEN() system("clear")
#endif

#define MAX_LENGTH 100
#define MAX_TABLES 20 

// 定义菜品制作状态
#define ORDER_STATUS_PENDING 0   // 待制作
#define STATUS_DONE 1      // 已制作

//菜单数据结构
typedef struct {
	int no; //编号
	char dish_name[20]; //菜名
	double dish_price; //价格
	int type; //种类：1-热菜、2-凉菜、3-主食、4-饮品 
    int has_options; // 是否有口味选项 (0.否 1.是)
} dish_menu;

// 购物车项结构（也是订单项结构）
typedef struct {
    int no;              // 菜品编号
    char dish_name[20];  // 菜品名称
    double dish_price;   // 单价
    int type;            // 类型
    int nums;            // 数量
    double subtotal;     // 小计价格
    int status;          // 制作状态：0-待制作, 1-已制作
    char remark[50];     // 备注信息
} cart_item;

// 厨房队列项结构（用于全局排序）
typedef struct {
    int table_no;        // 桌号
    int dish_no;         // 菜品编号
    char dish_name[20];  // 菜名
    int nums;            // 数量
    int status;          // 状态：0-待做, 1-已做
    char remark[50];     // 备注信息
} kitchen_item;

// 购物车结构
typedef struct {
    cart_item items[MAX_LENGTH];  // 购物车项数组
    int count;                     // 当前菜品数量
    double total_amount;           // 总金额
    int table_no;   
    int kitchen_received;          // 标记厨房是否已接收订单               // 雅座
} shopping_cart;

// 评价结构
typedef struct {
    char level[20];   // 评价等级:夯, 顶级, 人上人, NPC, 拉完了
    char comment[200]; // 评价留言
    char time_str[50]; // 评价时间
    int table_no;      // 雅座
} review;


// 订单模块（C 同学）
void create_order_filename(int table_no, char* fstr, int size);
void check_bill();
void order_status();
void calculate_value(char*, double*, double*, double*, double*, double*);
int record_income(double account, double account_hot_dish, double account_cold_dish, double account_staple_food, double account_drink);

void save_review(int table_no); // 保存评价
// void add_remark_to_order(int table_no); // 在下单前添加备注 [已禁用] 

// 公共工具函数
void error_check(int, int, int*);
void greet(struct tm* p, int user_role);
struct tm* get_time(void);


// 购物车相关
void init_cart(int table_no);
void display_cart(void);
void remove_from_cart(int index);
void submit_order(void);
void clear_stdin_buffer(void);
void clear_cart_items(void);
void ordering_menu(void);
int  is_recommend(int type, int no);


// 厨房模块相关函数声明
void kitchen_form();
int load_kitchen_queue(kitchen_item* queue);
void complete_dish_in_queue(int index);
int load_pending_orders(int table_no, cart_item* orders);

#endif