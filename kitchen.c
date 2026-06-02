#include "init.h"

/*
 * 函数功能：加载厨房总队列
 */
int load_kitchen_queue(kitchen_item* queue) {
    FILE* fp = fopen("kitchen_queue.txt", "r");
    if (!fp) return 0;

    int count = 0;
    char line[256];
    while (count < MAX_LENGTH && fgets(line, sizeof(line), fp) != NULL) {
        int consumed = 0;
        int ret = sscanf(line, "%d %d %s %d %d %n",
                  &queue[count].table_no,
                  &queue[count].dish_no,
                  queue[count].dish_name,
                  &queue[count].nums,
                  &queue[count].status,
                  &consumed);

        if (ret < 5) continue;

        char* remark_ptr = line + consumed;
        while (*remark_ptr == ' ') remark_ptr++;
        size_t len = strlen(remark_ptr);
        if (len > 0 && remark_ptr[len - 1] == '\n') {
            remark_ptr[len - 1] = '\0';
            len--;
        }
        if (len == 0) {
            strcpy(queue[count].remark, "-");
        } else {
            strncpy(queue[count].remark, remark_ptr, sizeof(queue[count].remark) - 1);
            queue[count].remark[sizeof(queue[count].remark) - 1] = '\0';
        }

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

    // --- 同步更新订单文件中对应菜品的状态 ---
    {
        int tno = queue[index].table_no;
        int dno = queue[index].dish_no;
        char remark_target[50];
        strncpy(remark_target, queue[index].remark, sizeof(remark_target) - 1);
        remark_target[sizeof(remark_target) - 1] = '\0';

        char filename[50];
        create_order_filename(tno, filename, sizeof(filename));

        FILE* ofp = fopen(filename, "r");
        if (ofp) {
            cart_item orders[MAX_LENGTH];
            int ocount = 0;
            char line[256];
            while (ocount < MAX_LENGTH && fgets(line, sizeof(line), ofp) != NULL) {
                int consumed = 0;
                int ret = sscanf(line, "%d %s %lf %d %d %d %n",
                          &orders[ocount].no,
                          orders[ocount].dish_name,
                          &orders[ocount].dish_price,
                          &orders[ocount].type,
                          &orders[ocount].nums,
                          &orders[ocount].status,
                          &consumed);
                if (ret < 6) continue;

                char* rem_ptr = line + consumed;
                while (*rem_ptr == ' ') rem_ptr++;
                size_t rlen = strlen(rem_ptr);
                if (rlen > 0 && rem_ptr[rlen - 1] == '\n') {
                    rem_ptr[rlen - 1] = '\0';
                    rlen--;
                }
                if (rlen == 0) {
                    strcpy(orders[ocount].remark, "-");
                } else {
                    strncpy(orders[ocount].remark, rem_ptr, sizeof(orders[ocount].remark) - 1);
                    orders[ocount].remark[sizeof(orders[ocount].remark) - 1] = '\0';
                }

                // 匹配：菜品编号和备注都一致才更新状态
                if (orders[ocount].no == dno &&
                    strcmp(orders[ocount].remark, remark_target) == 0 &&
                    orders[ocount].status == STATUS_PENDING) {
                    orders[ocount].status = STATUS_DONE;
                }
                ocount++;
            }
            fclose(ofp);

            // 写回订单文件
            ofp = fopen(filename, "w");
            if (ofp) {
                for (int j = 0; j < ocount; j++) {
                    fprintf(ofp, "%d %s %.2lf %d %d %d %s\n",
                            orders[j].no,
                            orders[j].dish_name,
                            orders[j].dish_price,
                            orders[j].type,
                            orders[j].nums,
                            orders[j].status,
                            orders[j].remark);
                }
                fclose(ofp);
            }
        }
    }

    FILE* fp = fopen("kitchen_queue.txt", "w");
    for (int i = 0; i < count; i++) {
        // 只写入未完成的菜品，已完成的直接从队列中移除
        if (queue[i].status == STATUS_PENDING) {
            fprintf(fp, "%d %d %s %d %d %s\n",
                    queue[i].table_no,
                    queue[i].dish_no,
                    queue[i].dish_name,
                    queue[i].nums,
                    queue[i].status,
                    queue[i].remark);
        }
    }
    fclose(fp);
}

/*
 * 函数功能：厨房主界面 - 按提交顺序展示
 */
void kitchen_form() {
    CLEAR_SCREEN();
    printf("========================================\n");
    printf("         👨‍🍳 后厨 · 实时备餐队列\n");
    printf("========================================\n");

    kitchen_item queue[MAX_LENGTH];
    int count = load_kitchen_queue(queue);
    
    if (count == 0) {
        printf("\n当前所有菜品已制作完毕！☕\n");
    } else {
        printf("\n%-6s %-8s %-12s %-8s %-10s\n", "序号", "桌号", "菜品名称", "数量", "备注");
        printf("----------------------------------------\n");
        for (int i = 0; i < count; i++) {
            if (queue[i].status == STATUS_PENDING) {
                printf("[%d]    %-8d %-12s x%d   [%s]\n", 
                       i + 1, 
                       queue[i].table_no, 
                       queue[i].dish_name, 
                       queue[i].nums,
                       queue[i].remark);
            }
        }
    }

    printf("\n----------------------------------------\n");
    printf("请输入【序号】标记完成 (输入0返回): ");
    int seq;
    scanf("%d", &seq);
    
    if (seq > 0) {
        complete_dish_in_queue(seq - 1); // 数组下标从0开始
    }
    
    // 递归刷新
    if (seq != 0) kitchen_form();
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
    while (count < MAX_LENGTH) {
        // 尝试读取7个字段（兼容新格式）
        int ret = fscanf(fp, "%d %s %lf %d %d %d %s", 
                  &orders[count].no,
                  orders[count].dish_name,
                  &orders[count].dish_price,
                  &orders[count].type,
                  &orders[count].nums,
                  &orders[count].status,
                  orders[count].remark);
        
        // 旧格式没有备注字段，新格式有7个字段
        if (ret == 6) {
            strcpy(orders[count].remark, "-");
        } else if (ret < 6) {
            break;
        } else if (strlen(orders[count].remark) == 0) {
            strcpy(orders[count].remark, "-");
        }
        
        if (orders[count].status == STATUS_PENDING) {
            count++;
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
    
    while (total_count < MAX_LENGTH) {
        int ret = fscanf(fp, "%d %s %lf %d %d %d %s", 
                  &all_orders[total_count].no,
                  all_orders[total_count].dish_name,
                  &all_orders[total_count].dish_price,
                  &all_orders[total_count].type,
                  &all_orders[total_count].nums,
                  &all_orders[total_count].status,
                  all_orders[total_count].remark);
        
        if (ret == 6) {
            strcpy(all_orders[total_count].remark, "-");
        } else if (ret < 6) {
            break;
        } else if (strlen(all_orders[total_count].remark) == 0) {
            strcpy(all_orders[total_count].remark, "-");
        }
        
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
    
    // 写回文件（保持7字段格式）
    fp = fopen(filename, "w");
    for (int i = 0; i < total_count; i++) {
        fprintf(fp, "%d %s %.2lf %d %d %d %s\n",
                all_orders[i].no,
                all_orders[i].dish_name,
                all_orders[i].dish_price,
                all_orders[i].type,
                all_orders[i].nums,
                all_orders[i].status,
                all_orders[i].remark);
    }
    fclose(fp);
}