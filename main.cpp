#include<stdio.h>
#include<graphics.h>
#include<time.h>
#include<math.h>
#include"tools.h"

#include<mmsystem.h>//播放背景音乐和音效
#pragma comment(lib,"winmm.lib")//播放背景音乐和音效的库

//准备工作
//1.准备vs
//2.使用easyx图形库（用来画图）
//3.准备素材

//开发日志
//1.构建初始的界面
//2.构建初始的方块数组
//3.实现初始的方块数组
//4.实现方块的移动

#define WIN_WIDHT 485
#define WIN_HEIGHT 917
#define ROWS 8
#define COLS 8
#define BLOCK_TYPE_COUNT 7
IMAGE imgBg;//表示背景图片
IMAGE imgBlocks[BLOCK_TYPE_COUNT];

//定义一个数据类型，表示方块
struct block {
	int type;//方块的类型,0:表示空白
	int x, y;
	int row, col;//行，列
	int match;//匹配次数
	int tmd;//透明度：0-255；
};
struct block map[ROWS+2][COLS+2];

const int off_x = 17;
const int off_y = 274;
const int block_size = 52;
int click;//表示相邻位置的单击次数，第二次单击，才会交换
int posX1, posY1;
int posX2, posY2;

bool isMoving;//表示当前是否正在移动
bool isSwap;//当单击两个相邻的方块后，设置为true

int score;

void init() {
	//创建游戏窗口
	initgraph(WIN_WIDHT, WIN_HEIGHT);

	loadimage(&imgBg, "res/bg2.png");  
	char name[64];
	for (int i = 0; i < BLOCK_TYPE_COUNT; i++) {
		sprintf_s(name, sizeof(name), "res/%d.png", i + 1);
		loadimage(&imgBlocks[i], name, block_size, block_size, true);
	}

	//配置随机数的随机种子
	srand(time(NULL));

	//初始化方块数组
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			map[i][j].type = 1+rand() % 4;
			map[i][j].row = i;
			map[i][j].col = j;
			map[i][j].x = off_x + (j - 1) * (block_size + 5);
			map[i][j].y = off_y + (i - 1) * (block_size + 5);
			map[i][j].match = 0;
			map[i][j].tmd = 255;
		}
	}
	click = 0;
	isMoving = false;
	isSwap = false;
	score = 0;
	setFont("Segoe UI Black", 20, 40);

	//播放背景音乐
	//mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	//
	mciSendString("open res/bg.mp3 alias bgm", 0, 0, 0);
	mciSendString("play bgm repeat", 0, 0, 0);
	mciSendString("setaudio bgm volume to 80", 0, 0, 0);


	//mciSendString("play res/start.mp3", 0, 0, 0);
}

void updataWindow() {
	BeginBatchDraw();//开始双缓冲
	putimage(0, 0, &imgBg);
	for (int i = 1; i <= ROWS; i++) {
		for (int j=1; j<= COLS; j++) {
			if (map[i][j].type != 0) {
				IMAGE* img = &imgBlocks[map[i][j].type - 1];
					putimageTMD(map[i][j].x, map[i][j].y, img,map[i][j].tmd);
			}
		}
	}
	char scoreStr[16];
	sprintf_s(scoreStr, sizeof(scoreStr), "%d", score);
	int x = 394 + (75 - strlen(scoreStr) * 20) / 2;
	outtextxy(x, 60, scoreStr);
	EndBatchDraw();//结束双缓冲
}

void exchange(int row1, int col1, int row2, int col2) {
	//to do.
	struct block tmp = map[row1][col1];
	map[row1][col1] = map[row2][col2];
	map[row2][col2]= tmp;

	map[row1][col1].row = row1;
	map[row1][col1].col = col1;
	map[row2][col2].row = row2;
	map[row2][col2].col = col2;
}

void userClick() {
	ExMessage msg;//需要安装最新版的easyx
	if (peekmessage(&msg)&& msg.message == WM_LBUTTONDOWN) {
		/*
			map[i][j].x = off_x + (j - 1) * (block_size+5);
			map[i][j].y = off_y + (i - 1) * (block_size + 5);
		*/
		if (msg.x < off_x||msg.y<off_y) return;

		int col = (msg.x - off_x) / (block_size + 5) + 1;
		int  row = (msg.y - off_y) / (block_size + 5) + 1;

		if (col > COLS || row > ROWS) return;

		click++;
		if (click == 1) {
			posX1 = col;
			posY1 = row;
		}
		else if (click == 2) {
			posX2 = col;
			posY2 = row;
			if (abs(posX2 - posX1) + abs(posY2 - posY1) == 1) {
				exchange(posY1, posX1, posY2, posX2);
				click = 0;
				isSwap = true;
				//播放音效；
				PlaySound("res/pao.wav", 0, SND_FILENAME | SND_ASYNC);
			}
			else {
				click = 1;
				posX1 = col;
				posY1 = row;
			}
		}
	}

}

void move() {
	isMoving = false;
	for (int i = ROWS; i >= 1; i--) {
		for (int j = 1; j <=COLS; j++) {
			struct block* p = &map[i][j];
			int dx, dy;
			for (int k = 0; k < 4; k++) {
				int x = off_x + (p->col - 1) * (block_size + 5);
				int y = off_y + (p->row - 1) * (block_size + 5);

				dx = p->x - x;
				dy = p->y - y;

				if (dx) p->x -= dx / abs(dx);
				if (dy) p->y -= dy / abs(dy);
			}
			if (dx || dy) isMoving = true;
		}
	}
}

void huanYuan() {
	//发生移动后，而且这个单向移动已经结束
	if (isSwap && !isMoving) {
		//如果没有匹配到大于等于三个以上的方块，就要还原
		int count = 0;
		for (int i = 1; i < ROWS; i++) {
			for (int j = 1; j < COLS; j++) {
				count += map[i][j].match;
			}
		}
		if (count==0) {
			exchange(posY1, posX1, posY2, posX2);
		}
		isSwap = false;
	}
}

void check() {
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type == map[i + 1][j].type &&
				map[i][j].type == map[i - 1][j].type) {
				for (int k = -1; k <= 1; k++) map[i+k][j].match++;
			}
			if (map[i][j].type == map[i][j - 1].type &&
				map[i][j].type == map[i][j + 1].type) {
				for (int k = -1; k <= 1; k++) map[i][j + k].match++;
			}
		}
	}
}

void xiaochu() {
	bool flag = false;
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match && map[i][j].tmd > 10) {
				if (map[i][j].tmd == 255) {
					flag = true;
				}
				map[i][j].tmd -= 10;
				isMoving = true;
			}
		}
	 }
	if (flag) {
		PlaySound("res/clear.wav", 0, SND_FILENAME | SND_ASYNC);
	}
}

void updateGame() {
	//下沉（降落）
	for (int i = ROWS; i >= 1; i--) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match) {
				for (int k = i - 1; k >= 1; k--) {
					if (map[k][j].match == 0) {
						exchange(k, j, i, j);
						break;
					}
				}
			}
		}
	}
	//生成新的方块，进行降落处理
	for (int j = 1; j <= COLS; j++) {
		int n = 0;
		for (int i = ROWS; i >= 1; i--) {
			if (map[i][j].match) {
				map[i][j].type = 1 + rand() % 7;
				map[i][j].y = off_y - (n + 1) * (block_size + 5);
				n++;
				map[i][j].match = 0;
				map[i][j].tmd = 255;

			}
		}
		score += n;
	}
}

int main(void) {
	init();

	updataWindow();//更新窗口
	while (1) {
		userClick();//处理用户点击操作
		check();//匹配次数检查
		move();
		if (!isMoving) xiaochu();
		huanYuan();//还原
		updataWindow(); //更新窗口

		if(!isMoving) updateGame();//更新游戏数据（降落）

		if(isMoving) Sleep(10);//帧等待;
	}
	system("pause");

	return 0;
}