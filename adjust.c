#include "init.h"
#include <string.h>
#include <stdio.h>

// 外部函数声明 
extern void error_check(int,int,int*);

// 外部变量声明
extern char hot_dish_filename[20];
extern char cold_dish_filename[20];
extern char staple_food_filename[20];
extern char drink_filename[20]; 

/*
* function_name: get_dish_filename
* return_type  : void
* param        : int(菜品种类), char*(返回的文件名)
* description  : 根据菜品类型获取对应的文件名
*/
void get_dish_filename(int type, char* filename) {
	switch(type) { // 使用switch语句根据菜品类型选择对应的文件名
		case 1: strcpy(filename, hot_dish_filename); break; // 热菜文件名
		case 2: strcpy(filename, cold_dish_filename); break; // 凉菜文件名
		case 3: strcpy(filename, staple_food_filename); break; // 主食文件名
		case 4: strcpy(filename, drink_filename); break;
	}
}


/*
* function_name: load_dishes
* return_type  : int (返回读取到的菜品数量)
* param        : const char*(文件名), dish_menu*(菜品数组)
* description  : 从指定文件中读取菜品到数组中
*/
int load_dishes(const char* filename, dish_menu* dm) {
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) return 0;
	
	int cnt = 0;
	while(cnt < MAX_LENGTH && fscanf(fp, "%d", &dm[cnt].no) == 1) {
		if(fscanf(fp, "%s", dm[cnt].dish_name) != 1) break;
		if(fscanf(fp, "%lf", &dm[cnt].dish_price) != 1) break;
		if(fscanf(fp, "%d", &dm[cnt].type) != 1) break;
		if(fscanf(fp, "%d", &dm[cnt].has_options) != 1) dm[cnt].has_options = 0;
		cnt++;
	}
	fclose(fp);
	return cnt;
}



/*
* function_name: is_recommend
* return_type  : int
* param        : int(菜品种类), int(菜品编号)
* description  : 检查菜品是否在招牌菜列表中，是返回1，否返回0
*/
int is_recommend(int type, int no) {
	FILE* fp = fopen("recommend.txt", "r");
	if(fp == NULL) return 0;

	int t, n;
	while(fscanf(fp, "%d %d", &t, &n) == 2) {
		if(t == type && n == no) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}



/*
* function_name: set_recommend
* return_type  : void
* param        : NULL
* description  : 设置招牌菜
*/
void set_recommend() {
	CLEAR_SCREEN();
	printf("===== 设置招牌菜 =====\n");
	printf("1.热菜\n2.凉菜\n3.主食\n4.饮品\n");
	printf("请选择菜品种类：");
	int type;
	scanf("%d", &type);
	error_check(1, 4, &type);

	char filename[20];
	get_dish_filename(type, filename);

	dish_menu dm[MAX_LENGTH];
	int cnt = load_dishes(filename, dm);

	if(cnt == 0) {
		printf("该分类下暂无菜品！\n");
		getch();
		return;
	}

	printf("当前菜品列表：\n----------------------------------------------------------\n");
	printf("%-8s %-24s %-10s\n", "编号", "名称", "价格");
	int i;
	for(i = 0; i < cnt; i++) {
		if(is_recommend(dm[i].type, dm[i].no))
			printf("%-8d 【招牌】%-10s %.2lf\n", dm[i].no, dm[i].dish_name, dm[i].dish_price);
		else
			printf("%-8d %-14s %.2lf\n", dm[i].no, dm[i].dish_name, dm[i].dish_price);
	}
	printf("----------------------------------------------------------\n");

	printf("请输入要设为招牌的菜品编号(0取消)：");
	int rec_no;
	scanf("%d", &rec_no);
	if(rec_no == 0) return;

	int found = 0;
	for(i = 0; i < cnt; i++) {
		if(dm[i].no == rec_no) { found = 1; break; }
	}
	if(!found) {
		printf("没有该菜品！\n");
		getch();
		return;
	}

	if(is_recommend(type, rec_no)) {
		printf("该菜品已是招牌菜！\n");
		getch();
		return;
	}

	FILE* fp = fopen("recommend.txt", "a");
	if(fp == NULL) {
		printf("错误: 无法写入招牌菜文件！\n");
		getch();
		return;
	}
	fprintf(fp, "%d %d\n", type, rec_no);
	fclose(fp);

	printf("设置成功！\n");
	getch();
}

/*
* function_name: cancel_recommend
* return_type  : void
* param        : NULL
* description  : 取消招牌菜
*/
void cancel_recommend() {
	CLEAR_SCREEN();
	printf("===== 取消招牌菜 =====\n");

	FILE* fp = fopen("recommend.txt", "r");
	if(fp == NULL) {
		printf("暂无招牌菜记录！\n");
		getch();
		return;
	}

	int types[MAX_LENGTH], nos[MAX_LENGTH];
	int cnt = 0;
	int t, n;
	while(cnt < MAX_LENGTH && fscanf(fp, "%d %d", &t, &n) == 2) {
		types[cnt] = t;
		nos[cnt] = n;
		cnt++;
	}
	fclose(fp);

	if(cnt == 0) {
		printf("暂无招牌菜！\n");
		getch();
		return;
	}

	printf("当前招牌菜：\n----------------------------------------------------------\n");
	printf("序号  分类    编号\n");
	int i;
	char* type_names[] = {"", "热菜", "凉菜", "主食", "饮品"};
	for(i = 0; i < cnt; i++) {
		printf("%-6d %-8s %d\n", i + 1, type_names[types[i]], nos[i]);
	}
	printf("----------------------------------------------------------\n");

	printf("请输入要取消的序号(0退出)：");
	int choice;
	scanf("%d", &choice);
	if(choice == 0) return;
	if(choice < 1 || choice > cnt) {
		printf("无效选择！\n");
		getch();
		return;
	}

	fp = fopen("recommend.txt", "w");
	if(fp == NULL) {
		printf("错误: 无法写入！\n");
		getch();
		return;
	}
	for(i = 0; i < cnt; i++) {
		if(i != choice - 1) fprintf(fp, "%d %d\n", types[i], nos[i]);
	}
	fclose(fp);

	printf("已取消该招牌菜！\n");
	getch();
}

/*
* function_name: price_adjust 
* return_type  : void
* param        : NULL
* description  : 价格调整 
*/ 
void price_adjust() {
	int choice;
	int quit_handle = 0;
	do {
		CLEAR_SCREEN();
		printf("1.热菜\n2.凉菜\n3.主食\n4.饮品\n");
		printf("请选择修改菜品的类型:");
		int type;
		scanf("%d", &type);
		error_check(1, 4, &type);

		char filename[20];
		get_dish_filename(type, filename);

		dish_menu dm[MAX_LENGTH];
		int cnt = load_dishes(filename, dm);

		if(cnt == 0) {
			printf("该分类下暂无菜品！\n");
			getch();
			return;
		}

		printf("当前菜品列表：\n----------------------------------------------------------\n");
		printf("编号    名称          价格      选项\n");
		int i;
		for(i = 0; i < cnt; i++) {
			const char* opt_str = dm[i].has_options ? "[选]" : "";
			if(is_recommend(dm[i].type, dm[i].no))
				printf("%-8d 【招牌】%-10s %.2lf   %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
			else
				printf("%-8d %-14s %.2lf   %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
		}
		printf("----------------------------------------------------------\n");

		printf("请输入需要修改价格的菜品编号：");
		int adjust_no;
		scanf("%d", &adjust_no);

		int flag = 0, pos = -1;
		do {
			for(i = 0; i < cnt; i++) {
				if(dm[i].no == adjust_no) { flag = 1; pos = i; break; }
			}
			if(flag == 0) {
				printf("没有该菜品!\n1.退出\n2.重新输入\n");
				int _choice;
				scanf("%d", &_choice);
				error_check(1, 2, &_choice);
				if(_choice == 1) { quit_handle = 1; break; }
				if(_choice == 2) { printf("请重新输入菜品编号："); scanf("%d", &adjust_no); }
			}
		} while(flag == 0);

		if(quit_handle == 1) break;

		printf("是否确认修改<%s>菜品的价格?1.是 2.否", dm[pos].dish_name);
		int adjust_choice;
		scanf("%d", &adjust_choice);
		error_check(1, 2, &adjust_choice);

		if(adjust_choice == 1) {
			printf("请输入修改菜品后的价格：");
			double re_price;
			scanf("%lf", &re_price);
			dm[pos].dish_price = re_price;

			FILE* fp = fopen(filename, "w");
			for(i = 0; i < cnt; i++) {
				fprintf(fp, "%d\n%s\n%lf\n%d\n%d\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, dm[i].type, dm[i].has_options);
			}
			fclose(fp);
			printf("修改成功！\n");
		}

		printf("是否继续修改？1.是，2否？");
		scanf("%d", &choice);
		error_check(1, 2, &choice);
	} while(choice != 2);
}

/*
* function_name: add_dish 
* return_type  : void
* param        : NULL
* description  : 添加菜品 
*/ 
void add_dish() {
	int choice;
	do {
		dish_menu new_dish;
		CLEAR_SCREEN();
		printf("请输入菜品编号：");
		scanf("%d", &new_dish.no);
		printf("请输入菜品名称：");
		scanf("%s", new_dish.dish_name);
		printf("请输入菜品价格：");
		scanf("%lf", &new_dish.dish_price);
		printf("请选择菜品种类(1.热菜 2.凉菜 3.主食 4.饮品)：");
		scanf("%d", &new_dish.type);
		error_check(1, 4, &new_dish.type);

		printf("该菜品是否需要调整辣度/口味？(1.是 0.否): ");
		scanf("%d", &new_dish.has_options);

		char filename[20];
		get_dish_filename(new_dish.type, filename);

		FILE* fp = fopen(filename, "a");
		if(fp == NULL) {
			printf("错误: 无法打开菜品文件 %s\n", filename);
			getch();
			return;
		}
		fprintf(fp, "%d\n%s\n%lf\n%d\n%d\n", new_dish.no, new_dish.dish_name, new_dish.dish_price, new_dish.type, new_dish.has_options);
		fclose(fp);

		printf("是否继续添加菜品：\n1.是\n2.否\n");
		scanf("%d", &choice);
		error_check(1, 2, &choice);
	} while(choice != 2);
}

/*
* function_name: del_dish
* return_type  : void
* param        : NULL
* description  : 删除菜品 
*/ 
void del_dish() {
	int choice;
	int quit_handle = 0;
	do {
		CLEAR_SCREEN();
		printf("1.热菜\n2.凉菜\n3.主食\n4.饮品\n");
		printf("请选择删除菜品的类型:");
		int type;
		scanf("%d", &type);
		error_check(1, 4, &type);

		char filename[20];
		get_dish_filename(type, filename);

		dish_menu dm[MAX_LENGTH];
		dish_menu dm_new[MAX_LENGTH];
		int cnt = load_dishes(filename, dm);

		if(cnt == 0) {
			printf("该分类下暂无菜品！\n");
			getch();
			return;
		}

		printf("当前菜品列表：\n----------------------------------------------------------\n");
		printf("%-8s %-24s %-10s %-8s\n", "编号", "名称", "价格", "选项");
		int i;
		for(i = 0; i < cnt; i++) {
			const char* opt_str = dm[i].has_options ? "[选]" : "";
			if(is_recommend(dm[i].type, dm[i].no))
				printf("%-8d 【招牌】%-16s %10.2lf %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
			else
				printf("%-8d %-24s %10.2lf %-8s\n", dm[i].no, dm[i].dish_name, dm[i].dish_price, opt_str);
		}
		printf("----------------------------------------------------------\n");

		printf("请输入需要删除的菜品编号：");
		int del_no;
		scanf("%d", &del_no);

		int flag = 0, pos = -1;
		do {
			int j;
			for(j = 0; j < cnt; j++) {
				if(dm[j].no == del_no) { flag = 1; pos = j; break; }
			}
			if(flag == 0) {
				printf("没有该菜品!\n1.退出\n2.重新输入\n");
				int _choice;
				scanf("%d", &_choice);
				error_check(1, 2, &_choice);
				if(_choice == 1) { quit_handle = 1; break; }
				if(_choice == 2) { printf("请重新输入菜品编号："); scanf("%d", &del_no); }
			}
		} while(flag == 0);

		printf("是否确认删除<%s>菜品?1.是 2.否", dm[pos].dish_name);
		int del_choice;
		scanf("%d", &del_choice);
		error_check(1, 2, &del_choice);

		if(del_choice == 1) {
			int j = 0;
			for(i = 0; i < cnt; i++) {
				if(i != pos) {
					dm_new[j] = dm[i]; // C语言支持同类型结构体直接赋值
					j++;
				}
			}

			FILE* fp = fopen(filename, "w");
			for(i = 0; i < j; i++) {
				fprintf(fp, "%d\n%s\n%lf\n%d\n%d\n", dm_new[i].no, dm_new[i].dish_name, dm_new[i].dish_price, dm_new[i].type, dm_new[i].has_options);
			}
			fclose(fp);
			printf("删除成功！\n");
		}

		printf("是否继续删除？1.是，2否？");
		scanf("%d", &choice);
		error_check(1, 2, &choice);
	} while(choice != 2);
}
