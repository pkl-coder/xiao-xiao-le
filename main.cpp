#include<stdio.h>
#include<graphics.h>
#include<time.h>
#include<math.h>
#include"tools.h"

#include<mmsystem.h>//���ű������ֺ���Ч
#pragma comment(lib,"winmm.lib")//���ű������ֺ���Ч�Ŀ�

//׼������
//1.׼��vs
//2.ʹ��easyxͼ�ο⣨������ͼ��
//3.׼���ز�

//������־
//1.������ʼ�Ľ���
//2.������ʼ�ķ�������
//3.ʵ�ֳ�ʼ�ķ�������
//4.ʵ�ַ�����ƶ�

#define WIN_WIDHT 485
#define WIN_HEIGHT 917
#define ROWS 8
#define COLS 8
#define BLOCK_TYPE_COUNT 7
IMAGE imgBg;//��ʾ����ͼƬ
IMAGE imgBlocks[BLOCK_TYPE_COUNT];

//����һ���������ͣ���ʾ����
struct block {
	int type;//���������,0:��ʾ�հ�
	int x, y;
	int row, col;//�У���
	int match;//ƥ�����
	int tmd;//͸���ȣ�0-255��
};
struct block map[ROWS+2][COLS+2];

const int off_x = 17;
const int off_y = 274;
const int block_size = 52;
int click;//��ʾ����λ�õĵ����������ڶ��ε������Żύ��
int posX1, posY1;
int posX2, posY2;

bool isMoving;//��ʾ��ǰ�Ƿ������ƶ�
bool isSwap;//�������������ڵķ��������Ϊtrue

int score;

void init() {
	//������Ϸ����
	initgraph(WIN_WIDHT, WIN_HEIGHT);

	loadimage(&imgBg, "res/bg2.png");  
	char name[64];
	for (int i = 0; i < BLOCK_TYPE_COUNT; i++) {
		sprintf_s(name, sizeof(name), "res/%d.png", i + 1);
		loadimage(&imgBlocks[i], name, block_size, block_size, true);
	}

	//������������������
	srand(time(NULL));

	//��ʼ����������
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

	//���ű�������
	//mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	//
	mciSendString("open res/bg.mp3 alias bgm", 0, 0, 0);
	mciSendString("play bgm repeat", 0, 0, 0);
	mciSendString("setaudio bgm volume to 80", 0, 0, 0);


	//mciSendString("play res/start.mp3", 0, 0, 0);
}

void updataWindow() {
	BeginBatchDraw();//��ʼ˫����
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
	EndBatchDraw();//����˫����
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
	ExMessage msg;//��Ҫ��װ���°��easyx
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
				//������Ч��
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
	//�����ƶ��󣬶�����������ƶ��Ѿ�����
	if (isSwap && !isMoving) {
		//���û��ƥ�䵽���ڵ����������ϵķ��飬��Ҫ��ԭ
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
	//�³������䣩
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
	//�����µķ��飬���н��䴦��
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

	updataWindow();//���´���
	while (1) {
		userClick();//�����û��������
		check();//ƥ��������
		move();
		if (!isMoving) xiaochu();
		huanYuan();//��ԭ
		updataWindow(); //���´���

		if(!isMoving) updateGame();//������Ϸ���ݣ����䣩

		if(isMoving) Sleep(10);//֡�ȴ�;
	}
	system("pause");

	return 0;
}