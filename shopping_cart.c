#include "init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>

//变量声明 
extern int table_no;          //餐台号 
extern char hot_dish_filename[20];
extern char cold_dish_filename[20];
extern char staple_food_filename[20];
extern char drink_filename[20]; 

//外部函数声明   
extern void error_check(int,int,int*);
extern void create_order_filename(int,char*,int);

// 购物车全局变量
shopping_cart cart;

// 函数声明
void init_cart(int);                              // 初始化购物车
void add_to_cart(dish_menu* dm, int index, int nums);  // 添加到购物车
void display_cart();                           // 显示购物车
void remove_from_cart(int index);              // 从购物车删除
void update_total();                      // 更新总金额
void submit_order();                           // 提交订单到厨房



 
/*
 * 函数功能：显示购物车内容
 */
void display_cart() {
    system("cls");
    printf("========================================\n");
    printf("         选膳筐 (雅座: %d)\n", cart.table_no);
    printf("========================================\n");
    
    if (cart.count == 0) {
        printf("\n选膳筐为空！\n");
    } else {
        printf("\n%-4s %-6s %-10s %-8s %-6s %-10s %-8s\n", 
               "序号", "编号", "菜品名称", "单价", "数量", "小计", "状态");
        printf("----------------------------------------------------------\n");
        
        for (int i = 0; i < cart.count; i++) { 
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf %-8s\n",
                   i + 1,
                   cart.items[i].no,
                   cart.items[i].dish_name,
                   cart.items[i].dish_price,
                   cart.items[i].nums,
                   cart.items[i].subtotal,
                   "未提交");
        }
        printf("----------------------------------------------------------\n");
        printf("总金额：%.2lf 元 | 厨房状态：已接收\n",
		       cart.total_amount);
    }
    
    printf("\n========================================\n");
    
    // 如果购物车为空或已提交，只显示返回选项
    if (cart.count == 0 || cart.kitchen_received) {
        printf("1. 返回主菜单\n");
    } else {
    	// 否则显示完整选项
    	printf("1. 提交，报送厨灶\n");
        printf("2. 删除菜品\n");
        printf("3. 继续加菜\n");
        printf("4. 返回\n");
	}
    printf("请选择: ");
        
    int choice;
    scanf("%d", &choice);
    // error_check(1, cart.kitchen_received || cart.count == 0 ? 1 : 3, &choice);
    // if (cart.kitchen_received || cart.count == 0){
    // 	return;
	// }
    
    switch(choice) {
        case 1:  {
            submit_order();  // 提交后会显示已点菜品
            break;
        }
        case 2:
        {
            printf("请输入要删除的序号: ");
            int del_index;
            scanf("%d", &del_index);
            
            if (del_index < 1 || del_index > cart.count) {
                printf("无效的序号！\n");
                getch();
                break;
            }
            
            // 确认删除
            printf("确认删除 %s 吗？(1.是 2.否): ", 
                   cart.items[del_index - 1].dish_name);
            int confirm;
            scanf("%d", &confirm);
            
            if (confirm == 1) {
                remove_from_cart(del_index - 1);
                printf("删除成功！\n");
            }
            getch();
            break;
        }
        case 3:
           ordering_menu();  // 继续点菜
            break;
        case 4: 
            break;
        
    }
}


/*
 *函数功能：从购物车删除菜品 
 */
void remove_from_cart(int index) {
    if (index < 0 || index >= cart.count) {
        return;
    }

    for (int i = index; i < cart.count - 1; i++) {
        cart.items[i] = cart.items[i + 1];
    }

    // 清空最后一个位置
    cart.items[cart.count - 1].no = 0;
    cart.items[cart.count - 1].nums = 0;
    cart.items[cart.count - 1].subtotal = 0;
    cart.items[cart.count - 1].status = 0;
    memset(cart.items[cart.count - 1].dish_name, 0, sizeof(cart.items[cart.count - 1].dish_name));

    cart.count--;
    update_total();
}

/*
 * 函数功能：提交订单到厨房（智能合并同类菜品并累加到订单文件）
 */
void submit_order() {
    if (cart.count == 0) {
        printf("购物车为空，无法提交！\n");
        getch();
        return;
    }
    
    char filename[50];
    create_order_filename(cart.table_no, filename, sizeof(filename));
    
    // 1. 读取已有的订单数据
    FILE* fp = fopen(filename, "r");
    int total_count = 0;
    cart_item final_orders[MAX_LENGTH];
    
    if (fp) {
        while (total_count < MAX_LENGTH && 
               fscanf(fp, "%d %s %lf %d %d", 
                      &final_orders[total_count].no,
                      final_orders[total_count].dish_name,
                      &final_orders[total_count].dish_price,
                      &final_orders[total_count].type,
                      &final_orders[total_count].nums) == 5) {
            total_count++;
        }
        fclose(fp);
    }

    // 2. 将购物车的新菜品合并进去
    for (int i = 0; i < cart.count; i++) {
        int found = 0;
        // 在已有订单中查找是否已存在该菜品
        for (int j = 0; j < total_count; j++) {
            if (final_orders[j].no == cart.items[i].no) {
                final_orders[j].nums += cart.items[i].nums; // 找到则累加数量
                found = 1;
                break;
            }
        }
        
        // 如果没找到，则作为新菜品添加
        if (!found) {
            if (total_count < MAX_LENGTH) {
                final_orders[total_count] = cart.items[i];
                total_count++;
            }
        }
    }

    // 3. 将合并后的完整清单写回文件
    fp = fopen(filename, "w");
    if (!fp) {
        printf("订单提交失败！\n");
        getch();
        return;
    }
    
    for (int i = 0; i < total_count; i++) {
        fprintf(fp, "%d %s %.2lf %d %d\n",
                final_orders[i].no,
                final_orders[i].dish_name,
                final_orders[i].dish_price,
                final_orders[i].type,
                final_orders[i].nums);
    }  
    
    fclose(fp);
    
    // 4. 提示用户并清空内存购物车
    system("cls");
    printf("========================================\n");
    printf("         订单提交成功！\n");
    printf("========================================\n");
    printf("\n菜品已加入总账单！\n");
    printf("总金额: %.2lf 元\n", cart.total_amount);
    printf("\n正在返回主菜单...\n");
    
    // --- 新增：同步到厨房总队列 (kitchen_queue.txt) ---
    FILE* kfp = fopen("kitchen_queue.txt", "a"); // 使用追加模式，保证时间顺序
    if (kfp) {
        for (int i = 0; i < cart.count; i++) {
            fprintf(kfp, "%d %d %s %d %d\n", 
                    cart.table_no,             // 桌号
                    cart.items[i].no,          // 菜品编号
                    cart.items[i].dish_name,   // 菜名
                    cart.items[i].nums,        // 数量
                    STATUS_PENDING);           // 初始状态为待做
        }
        fclose(kfp);
    }
    
    Sleep(1500);
    clear_cart_items(); 
    getch();
}


/*
 *函数功能：在购物车查找菜品，用于累计数量
 */
int find_item_in_cart(int dish_no) {
    for (int i = 0; i < cart.count; i++) {
        if (cart.items[i].no == dish_no) return i;
    }
    return -1;
}


/*
 *函数功能：更新购物车总金额 
 */ 
void update_total() {
    cart.total_amount = 0;
    for (int i = 0; i < cart.count; i++) {
        cart.items[i].subtotal = cart.items[i].dish_price * cart.items[i].nums;
        cart.total_amount += cart.items[i].subtotal;
    }
}


/*
 * 函数功能：初始化购物车
 */
void init_cart(int table_no) {
    cart.count = 0;
    cart.total_amount = 0;
    cart.kitchen_received = 0;
    cart.table_no = table_no;
    
    // 清空购物车数组
    for (int i = 0; i < MAX_LENGTH; i++) {
        cart.items[i].no = 0;
        cart.items[i].nums = 0;
        cart.items[i].subtotal = 0;
        cart.items[i].status = 0;
        memset(cart.items[i].dish_name, 0, sizeof(cart.items[i].dish_name));
    }
} 



//函数功能：清空菜品
void clear_cart_items() {
    cart.count = 0;
    cart.total_amount = 0;
    for (int i = 0; i < MAX_LENGTH; i++) {
        cart.items[i].no = 0;
        cart.items[i].nums = 0;
        cart.items[i].subtotal = 0;
        memset(cart.items[i].dish_name, 0, sizeof(cart.items[i].dish_name));
    }
}
