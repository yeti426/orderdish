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
 *函数功能：在购物车查找菜品 
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


/*
 * 函数功能：添加菜品到购物车
 * 参数：dm - 菜单数组, index - 菜品索引, nums - 数量
 */
void add_to_cart(dish_menu* dm, int index, int nums) {
	if (cart.kitchen_received) {
		printf("订单已提交，无法修改购物车！\n");
		getch();
		return;
	}
	if (nums <= 0) {
		printf("数量必须 > 0!\n");
		getch();
		return;
	}
	if(index < 0 || index >= MAX_LENGTH || dm[index].no == 0) {
		printf("无效菜品序号！\n");
		getch();
		return;
	}
	
	int i = find_item_in_cart(dm[index].no);
	if (i != -1) {
		// 已存在，增加数量
		cart.items[i].nums += nums;
		printf("已增加 %s ×%d！\n", dm[index].dish_name, nums);
		cart.items[i].subtotal = cart.items[i].dish_price * cart.items[i].nums;
		update_total();
		getch();
		return;
	} else {
		// 不存在，添加新项
		if (cart.count >= MAX_LENGTH) {
            printf("购物车已满！\n");
            getch();
            return;
        }
        cart.items[cart.count].no = dm[index].no;
        strcpy(cart.items[cart.count].dish_name, dm[index].dish_name);
        cart.items[cart.count].dish_price = dm[index].dish_price;
        cart.items[cart.count].type = dm[index].type;
        cart.items[cart.count].nums = nums;
        cart.items[cart.count].subtotal = dm[index].dish_price * nums;
        cart.items[cart.count].status = 0; 
        
        cart.count++;
        update_total();
        printf("已将 %s 加入选膳筐！\n", dm[index].dish_name);
    }
    update_total();
    getch();
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
 * 函数功能：显示购物车内容
 */
void display_cart() {
    system("cls");
    printf("========================================\n");
    printf("         选膳筐 (桌号: %d)\n", cart.table_no);
    printf("========================================\n");
    
    if (cart.count == 0) {
        printf("\n选膳筐为空！\n");
    } else {
        printf("\n%-4s %-6s %-10s %-8s %-6s %-10s %-8s\n", 
               "序号", "编号", "菜品名称", "单价", "数量", "小计", "状态");
        printf("----------------------------------------------------------\n");
        
        for (int i = 0; i < cart.count; i++) {
            const char* status = (cart.items[i].status == 0) ? "未提交" :
                                 (cart.items[i].status == 1) ? "制作中" : "已上菜"; 
            printf("%-4d %-6d %-10s %-8.2lf %-6d %-10.2lf %-8s\n",
                   i + 1,
                   cart.items[i].no,
                   cart.items[i].dish_name,
                   cart.items[i].dish_price,
                   cart.items[i].nums,
                   cart.items[i].subtotal,
                   status);
        }
        printf("----------------------------------------------------------\n");
        printf("总金额：%.2lf 元 | 厨房状态：%s\n",
		       cart.total_amount,
			   cart.kitchen_received ? "已接收" : "未接收"); 
    }
    
    printf("\n========================================\n");
    
    // 如果购物车为空或已提交，只显示返回选项
    if (cart.count == 0 || cart.kitchen_received) {
        printf("1. 返回主菜单\n");
    } else {
    	// 否则显示完整选项
    	printf("1. 删除菜品\n");
        printf("2. 提交，报送厨灶\n");
        printf("3. 返回主菜单\n");
	}
    printf("请选择: ");
        
    int choice;
    scanf("%d", &choice);
    error_check(1, cart.kitchen_received || cart.count == 0 ? 1 : 3, &choice);
    if (cart.kitchen_received || cart.count == 0){
    	return;
	}
    
    switch(choice) {
        case 1: {
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
        case 2: {
            submit_order();  // 提交后会显示已点菜品
            break;
        }
        case 3:
            break;
    }
}

//函数功能：清空菜品 不重置kitchen_received和table_no
void clear_cart_items() {
    cart.count = 0;
    cart.total_amount = 0;
    for (int i = 0; i < MAX_LENGTH; i++) {
        cart.items[i].no = 0;
        cart.items[i].nums = 0;
        cart.items[i].subtotal = 0;
        cart.items[i].status = 0;
        memset(cart.items[i].dish_name, 0, sizeof(cart.items[i].dish_name));
    }
}


/*
 * 函数功能：提交订单到厨房（写入文件并清空购物车）
 */
void submit_order() {
    if (cart.count == 0) {
        printf("选膳筐为空，无法提交！\n");
        getch();
        return;
    }
    
    if (cart.kitchen_received) {
        printf("订单已提交到厨房，请勿重复提交！\n");
        getch();
        return;
    }
    
    char filename[50];
    create_order_filename(cart.table_no, filename, sizeof(filename));
    
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("订单提交失败！\n");
        getch();
        return;
    }
    
    // 写入订单状态：1-待支付
    fprintf(fp, "1\n");
    
    // 获取并写入订单备注
    char remark[200] = "";
    printf("请输入订单备注 (如: 少辣, 免葱, 直接回车跳过): ");

    // 清空缓冲区防止读取到之前的回车
    clear_stdin_buffer(); 

    if (fgets(remark, sizeof(remark), stdin) != NULL) {
        // 去除末尾换行符
        size_t len = strlen(remark);
        if (len > 0 && remark[len - 1] == '\n') {
            remark[len - 1] = '\0';
        }
    }
    
    // 如果用户没输入，给个默认值
    if (strlen(remark) == 0) {
        strcpy(remark, "无备注");
    }
    
    //确保备注独占一行，后面必须跟一个换行符
    fprintf(fp, "%s\n", remark); // 写入备注作为第二行



    // 写入所有购物车项
    for (int i = 0; i < cart.count; i++) {
        fprintf(fp, "%d %s %lf %d %d\n",
                cart.items[i].no,
                cart.items[i].dish_name,
                cart.items[i].dish_price,
                cart.items[i].type,
                cart.items[i].nums);
    }  
    fclose(fp);
    
    // 标记厨房已收到
    cart.kitchen_received = 1;
    
    system("cls");
    printf("========================================\n");
    printf("         膳单报送成功！\n");
    printf("========================================\n");
    printf("\n厨灶备餐中！\n");
    printf("总金额: %.2lf 元\n", cart.total_amount);
    printf("\n提示：您可在【查看账单】中查看已提交膳单。\n");
    printf("\n正在返回主菜单...\n");
    
    Sleep(1500);  // 暂停1.5秒让用户看到提示
    
    // 清空购物车
    clear_cart_items();
    getch();
}