#include "init.h"


/*
 * 函数功能：厨房主界面 - 按提交顺序展示
 */
void kitchen_form() {
    system("cls");
    printf("========================================\n");
    printf("         👨‍🍳 后厨 · 实时备餐队列\n");
    printf("========================================\n");

    kitchen_item queue[MAX_LENGTH];
    int count = load_kitchen_queue(queue);
    
    if (count == 0) {
        printf("\n当前所有菜品已制作完毕！☕\n");
    } else {
        printf("\n%-6s %-8s %-12s %-8s\n", "序号", "桌号", "菜品名称", "数量");
        printf("----------------------------------------\n");
        for (int i = 0; i < count; i++) {
            if (queue[i].status == STATUS_PENDING) {
                printf("[%d]    %-8d %-12s x%d\n", 
                       i + 1, 
                       queue[i].table_no, 
                       queue[i].dish_name, 
                       queue[i].nums);
            }
        }
    }

    printf("\n----------------------------------------\n");
    printf("请输入已完成菜品【序号】（按0返回）: ");
    int seq;
    scanf("%d", &seq);
    
    if (seq > 0) {
        complete_dish_in_queue(seq - 1); // 数组下标从0开始
    }
    
    // 递归刷新
    if (seq != 0) kitchen_form();
}



/*
 * 函数功能：加载厨房总队列
 */
int load_kitchen_queue(kitchen_item* queue) {
    FILE* fp = fopen("kitchen_queue.txt", "r");
    if (!fp) return 0;

    int count = 0;
    while (count < MAX_LENGTH && 
           fscanf(fp, "%d %d %s %d %d", 
                  &queue[count].table_no,
                  &queue[count].dish_no,
                  queue[count].dish_name,
                  &queue[count].nums,
                  &queue[count].status) == 5) {
        count++;
    }
    fclose(fp);
    return count;
}

/*
 * 函数功能：标记某道菜为已完成（通过行号索引）
 */
void complete_dish_in_queue(int index) {
    kitchen_item queue[MAX_LENGTH];
    int count = load_kitchen_queue(queue);

    if (index < 0 || index >= count) {
        printf("无效的序号！\n");
        getch();
        return;
    }

    queue[index].status = STATUS_DONE;

    FILE* fp = fopen("kitchen_queue.txt", "w");
    for (int i = 0; i < count; i++) {
        // 只写入未完成的菜品，已完成的直接从队列中移除
        if (queue[i].status == STATUS_PENDING) {
            fprintf(fp, "%d %d %s %d %d\n",
                    queue[i].table_no, 
                    queue[i].dish_no, 
                    queue[i].dish_name, 
                    queue[i].nums, 
                    queue[i].status);
        }
    }
    fclose(fp);
}

/*
 * 函数功能：读取指定桌号的订单文件，返回待制作的菜品列表
 */
int load_pending_orders(int table_no, cart_item* orders) {
    char filename[50];
    create_order_filename(table_no, filename, sizeof(filename));
    
    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;
    
    int count = 0;
    // 直接循环读取，不跳过状态行（因为我们现在的文件格式就是纯菜品列表）
    while (count < MAX_LENGTH && 
           fscanf(fp, "%d %s %lf %d %d %d", 
                  &orders[count].no,
                  orders[count].dish_name,
                  &orders[count].dish_price,
                  &orders[count].type,
                  &orders[count].nums,
                  &orders[count].status) == 6) {
        
        if (orders[count].status == STATUS_PENDING) {
            count++; // 只统计待制作的
        }
    }
    fclose(fp);
    return count;
}

/*
 * 函数功能：标记某桌号的某道菜为已完成，并写回文件
 */
void mark_dish_done(int table_no, int dish_no) {
    char filename[50];
    create_order_filename(table_no, filename, sizeof(filename));
    
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("未找到桌号 %d 的订单！\n", table_no);
        getch();
        return;
    }
    
    cart_item all_orders[MAX_LENGTH];
    int total_count = 0;
    
    while (total_count < MAX_LENGTH && 
           fscanf(fp, "%d %s %lf %d %d %d", 
                  &all_orders[total_count].no,
                  all_orders[total_count].dish_name,
                  &all_orders[total_count].dish_price,
                  &all_orders[total_count].type,
                  &all_orders[total_count].nums,
                  &all_orders[total_count].status) == 6) {
        total_count++;
    }
    fclose(fp);
    
    // 查找并修改状态
    int found = 0;
    for (int i = 0; i < total_count; i++) {
        if (all_orders[i].no == dish_no && all_orders[i].status == STATUS_PENDING) {
            all_orders[i].status = STATUS_DONE;
            found = 1;
            break;
        }
    }
    
    if (!found) {
        printf("未找到桌号 %d 中编号为 %d 的待做菜品！\n", table_no, dish_no);
        getch();
        return;
    }
    
    // 写回文件
    fp = fopen(filename, "w");
    for (int i = 0; i < total_count; i++) {
        fprintf(fp, "%d %s %.2lf %d %d %d\n",
                all_orders[i].no,
                all_orders[i].dish_name,
                all_orders[i].dish_price,
                all_orders[i].type,
                all_orders[i].nums,
                all_orders[i].status);
    }
    fclose(fp);
}
