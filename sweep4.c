#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS			10//总行数
#define COLUMNS			10//总列数
#define MINE_QTY		10//地雷个数
#define HI_MASK			0Xf0//高位掩码
#define LOW_MASK		0X0f//低位掩码
#define EMPTY			0//空格子
#define MINE			10//地雷格子
#define HIDDEN			0//隐藏的格子
#define OPEN			0X10//挖开的格子
#define SIGN_EMPTY		' '//表示空位置
#define SIGN_MINE		'X'//表示地雷的字符
#define SIGN_HIDDEN		'*'//表示隐藏格子的字符
#define SIGN_RIM		'+'//表示边框的字符
#define IS_EMPTY(n)		(!((n) & LOW_MASK))//判断格子内容是否为空；放在宏LOW_MASK之前也可以
#define IS_MINE(n)		(((n) & LOW_MASK) == MINE)//判断格子内容是否地雷
#define IS_HIDDEN(n)	(!((n) & HI_MASK))//判断格子内容是否隐藏
#define NUM2CHAR(n)		((n) - 10 + 'a')//把数字转换为字符（最大值不是f）
enum{WIN, LOSE, GOON}; //代表游戏结果的枚举值

//向地图里放置多个地雷
void plant(char map[][COLUMNS]/*地图*/,int rows/*地图行数，不能直接设置为ROWS*/){
	int row=0,col=0,cnt=0;
	do{
		row=rand() % rows;
		col=rand() % COLUMNS;
		if(!IS_MINE(map[row][col])){
			map[row][col] &= 0xf0;//把低四位内容设置为0
			map[row][col] |= MINE;//把这个格子设置为地雷
			cnt++;
		}
	}while(cnt < MINE_QTY);//地雷个数不足时循环继续
}

//向地图里放置数字
void mark(char map[][COLUMNS]/*地图*/, int rows/*地图的行数*/) {
    int row = 0, col = 0, i = 0;
    int tmp_row = 0, tmp_col = 0;//记录地雷相邻位置的变量
    int  delta[][2] = {-1, -1, -1, 0,
        -1, 1, 0, -1, 0, 1, 1, -1,
         1, 0, 1, 1};//用来计算相邻位置的辅助数组;
	//依次处理二维数组的每个元素
    for (row = 0;row <= rows - 1;row++) {
        for (col = 0;col <= COLUMNS - 1;col++) {
            if (!IS_MINE(map[row][col])) {//排除不是地雷的格子
                continue;
            }
            for (i = 0;i <= 7;i++) {//依次处理周围格子
                tmp_row = row + delta[i][0];//计算一个相邻格子的位置
                tmp_col = col + delta[i][1];
                if (tmp_row < 0 || tmp_row >= rows || tmp_col <0 || tmp_col >= COLUMNS) {//排除地图外的位置
                    continue;
                }
                if (IS_MINE(map[tmp_row][tmp_col])) {//排除地雷位置
                    continue;
                }
                map[tmp_row][tmp_col]++;//格子内容加一
            }
        }
    }
}

//显示整个图形
void show(char map[][COLUMNS]/*地图*/,int rows/*地图行数*/){
	int row=0,col=0,i=0;
	//显示第一行
	printf("  ");
	for(i=1;i<=COLUMNS;i++){
		if(i<=9){
			printf("%d ",i);
		}
		else{
			printf("%c",NUM2CHAR(i));
		}
	}
	printf("\n");
	//显示上边框
	printf("  ");
	for(i = 1;i <= COLUMNS;i++){
		printf("%c ",SIGN_RIM);
	}
	printf("\n");
	//显示地图内容
	for(row = 0;row <= rows-1;row++){
		//显示行号和左边框
		if(row+1<10){
			printf("%d",row+1);
		}
		else{
			printf("%c",NUM2CHAR(row+1));
		}
		printf("%c",SIGN_RIM);
		//显示一行内容
		for(col = 0;col < COLUMNS;col++){
			if(IS_HIDDEN(map[row][col])){//如果是隐藏的位置就显示*
				printf("%c ",SIGN_HIDDEN);
			}
			else if(IS_EMPTY(map[row][col])){//如果是挖开的空格子就显示空格
				printf("%c ",SIGN_EMPTY);
			}		
			else if(IS_MINE(map[row][col])){//如果是挖开的地雷就显示Ｘ
				printf("%c ",SIGN_MINE);
			}
			else{//如果是挖开的数字就显示数字
				printf("%d ",map[row][col] & LOW_MASK);
			}
		}
		//显示右边框
		printf("%c\n",SIGN_RIM);
	}
	//显示下边框
	printf("  ");
	for(i=1;i<=COLUMNS;i++){
		printf("%c ",SIGN_RIM);
	}
	printf("\n");
}

//扫雷函数
void sweep(char map[][COLUMNS],int rows/*地图行数*/,int row/*要挖位置的组下标*/,int col/*要挖位置的组内下标*/){
	static int delta[][2]={-1,-1,-1,0,-1,1,0,-1,0,1,1,-1,1,0,1,1};//用来计算周围位置的辅助数组，static是为了不用在递归时重复声明
	int i=0;
	if(row < 0 || row >= rows || col < 0 ||col >= COLUMNS){//位置不在地图里
		return;
	}
	if(!IS_HIDDEN(map[row][col])){//位置已经被挖开
		return;
	}
	map[row][col] |= OPEN;//把当前选择的位置挖开
	if(IS_MINE(map[row][col])){//如果挖开的是地雷就返回
		return;
	}
	if(!IS_EMPTY(map[row][col])){//如果挖开的不是空位置就只能是数字
		return;
	}
	for(i=0;i<=7;i++){
		sweep(map, rows, row + delta[i][0], col + delta[i][1]);//递归函数
	}
}

//判断结果的函数
int finish(char map[][COLUMNS]/*地图*/,int rows/*地图行数*/){
	int result = WIN/*假设已经赢了*/,row = 0,col = 0;
	//依次处理二维数组里的每个存储区
	for(row = 0;row <= rows - 1;row++){
		for(col = 0;col <= COLUMNS - 1;col++){
			if(IS_HIDDEN(map[row][col])){
				if(!IS_MINE(map[row][col])){
					result = GOON;//发现隐藏的非地雷格子就继续游戏
				}
			}
			else{
				if(IS_MINE(map[row][col])){
					return LOSE;//发现非隐藏的地雷就表示输了，结束游戏
				}
			}
		}
	}
	return result;
}

//主函数
int main(){
	int row=0,col=0,start=0,end=0,result=0;
	char map[ROWS][COLUMNS]={0};
	srand(time(0));
	/*for (row = 0;row <= ROWS - 1;row++) {//把所有格子设置为挖开状态
        for (col = 0;col <= COLUMNS - 1;col++) {
            map[row][col] |= OPEN;
        }
    }*/
	start=time(0);
	plant(map,ROWS);
	mark(map,ROWS);
	show(map,ROWS);
	while(1){
		printf("请选择一个位置：");
		scanf("%d%d",&row,&col);
		sweep(map,ROWS,row-1,col-1);
		show(map,ROWS);
		result=finish(map,ROWS);
		if(result==LOSE){//用户输了
			printf("你输了\n");
			break;
		}
		else if(result==WIN){//用户赢了
			printf("你赢了\n");
			break;
		}
	}
	end=time(0);
	printf("使用的时间是%d秒\n",end-start);
	return 0;
}
