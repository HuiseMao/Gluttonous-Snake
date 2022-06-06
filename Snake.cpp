#undef UNICODE
#undef _UNICODE		
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#pragma warning(disable:4996)	//禁用安全检查 _tcscpy、_stprintf

/*--------------------------------属 性---------------------------*/
#define WIDTH 1170		/*	窗口大小  WIDTH * HIGHT 像素 尺寸需为 SIZE 的整数倍，  	 */
#define HIGHT 640		/*	蛇的行动在 A(SIZE,SIZE)到 B(WIDTH-FORM-SIZE,HIGHT-SIZE)之间的矩形内*/
#define FORM 250		/*	设置文字边框宽度	*/
#define SIZE 40			/*	每节蛇的尺寸		*/

#define COLOUR RGB(rand()%255,rand()%255,rand()%255)	/*	彩色 */

// 枚举值
enum Direction { right = 77, left = 75, down = 80, up = 72 };	//枚举方向值
enum Control   { F1 = 59, F2 = 60, Esc = 27 };					//枚举功能键值

// 全局变量
static int g_speed = 100;			//	速度(控制刷新的时间间隔)
static int g_status = 0;			//	游戏结束标志

// 全局常量
const char* IMAGE_PATH = "image";	//	贴图资源所在路径
const int SNAKE_LENGTH = 3;			//	初始，默认初始长度
const int SNAKE_TOWARD = right;		//	初始，默认蛇头方向
const int MAX_SIZE = 100;			//	蛇的最长长度
const int SHOW_TAIL = 0;			//	是否展示蛇尾巴，蛇尾不计入长度



/*---------------------------资源-------------------------------------*/
//蛇
typedef struct _snake
{
	int x[MAX_SIZE];		//坐标—X
	int y[MAX_SIZE];		//坐标—Y
	int len;				//当前长度
	int count;				//当前分数
	int direction;			//蛇头方向
}Snake;

//食物
typedef struct _food
{
	int x;					//坐标—X
	int y;					//坐标—Y
	int flg;				//标记食物是否已经出现
}Food;

// 图片对象
IMAGE img[8];		//墙，蛇头，身体，蛇尾，食物
IMAGE tips[4];		//其他界面图片

// 标记图片变量名
char name[8][15] = { "wall","snakeHead","body","food","snakeTailUp","snakeTailDown","snakeTailLeft","snakeTailRight" };
char tipsName[4][15] = { "snake","label","gameover","start" };	

//加载图片
void loadResources()
{
	for (int i = 0; i < 8; i++)
	{
		char filename[30] = "";
		sprintf_s(filename, "%s/%s.png", IMAGE_PATH, name[i]);
		loadimage(img + i, filename);
	}
	for (int i = 0; i < 4; i++)
	{
		char filename[30] = "";
		sprintf_s(filename, "%s/%s.png", IMAGE_PATH, tipsName[i]);
		loadimage(tips + i, filename);
	}
}

/************************************************************/
//变量
Snake snake;		//蛇
Food  food;			//食物
//函数
void PrintForm();		//文字提示框
void Init_Snake();		//初始化蛇
void Init_Food();		//初始化食物
void ShowSnake();		//贴图画蛇
void ShowFood();		//贴图画食物
void MoveSnake();		//蛇的移动
void KeyDown();			//控制
void EatFood();			//吃食物
void GameOver();		//游戏结束
int EndScreen();		//结束画面
void DrawWall();		//画墙



//右侧展示区域
void PrintForm()
{
	fillrectangle(0, 0, WIDTH, HIGHT - 1);	//边框

	LOGFONT f;
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfHeight = 30;                      // 设置字体高度为 48
	_tcscpy(f.lfFaceName, _T("幼圆"));    // 设置字体为“黑体”
	f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);                     // 设置字体样式

	putimage(WIDTH - FORM, 0, &tips[0]);	//贴图——卡通贪吃蛇
	//putimage(WIDTH-FORM,250,&tips[1]);	//提示框贴图，操作手册
	setbkmode(TRANSPARENT);					//文字透明方式
	settextcolor(LIGHTBLUE);				//文字颜色
	TCHAR  str[3][50];
	_stprintf(str[0], _T("  得分: %3d 分"), snake.count);
	_stprintf(str[1], _T("  速度: %3d 级"), (100 - g_speed) / 20);
	_stprintf(str[2], _T("  长度: %3d 节"), snake.len);
	for (int i = 0; i < 3; ++i)
	{
		outtextxy(WIDTH - FORM + 10, i * 40 + 250, str[i]);	//指定位置输出字符串
	}

	TCHAR  str2[11][50];
	_stprintf(str2[0], _T(" 提示:"));
	_stprintf(str2[1], _T("  一枚果实10分，蛇最长  "));
	_stprintf(str2[2], _T("  100节，为通关。撞墙,"));
	_stprintf(str2[3], _T("  或撞到自己，游戏失败"));
	_stprintf(str2[4], _T("      "));
	_stprintf(str2[5], _T(" 按键:"));
	_stprintf(str2[6], _T("     F1 : 加速 "));
	_stprintf(str2[7], _T("     F2 : 减速"));
	_stprintf(str2[8], _T("    空格: 暂停"));
	_stprintf(str2[9], _T("    Esc : 退出 "));
	_stprintf(str2[10], _T("     "));
	settextcolor(BLACK);				//文字颜色
	f.lfHeight = 20;					// 设置字体高度为 20
	settextstyle(&f);					// 设置字体样式
	for (int i = 0; i < 11; i++)
	{
		outtextxy(WIDTH - FORM + 10, 400 + i * 20, str2[i]);	//指定位置输出字符串
	}

}

int main()
{
	initgraph(WIDTH, HIGHT);	//初始化窗口大小
	loadResources();			//加载图片

	setlinecolor(BLACK);		//画线颜色
	setfillcolor(WHITE);		//填充颜色
	fillrectangle(-1, -1, WIDTH, HIGHT);	//边框

	putimage(200, 50, &tips[3]);	
	_getch();					// 按下任意键开始

	Init_Snake();				//初始化蛇
	Init_Food();				//初始化食物

	while (true)//强退
	{
		if (food.flg == 0)
		{
			Init_Food();		//初始化食物
		}

		BeginBatchDraw();
		cleardevice();			//刷新窗口
		PrintForm();			//打印表框
		DrawWall();				//画墙
		ShowFood();				//画食物
		ShowSnake();			//画蛇
		EndBatchDraw();

		if (_kbhit())
		{
			KeyDown();			//玩家控制蛇移动
		}

		MoveSnake();			//蛇移动
		EatFood();				//吃食物

		GameOver();				//检查游戏是否结束，设置结束标志位
		if(EndScreen() == 1)	//游戏结束，展示成绩		
		{
			break;
		}
		Sleep(g_speed);			//控制速度
	}

	system("pause");
	closegraph();
	return 0;
}
/*--------------------------蛇的功能函数--------------------------*/

//初始化蛇
void Init_Snake()
{
	//初始化前三节蛇
	snake.x[0] = SIZE * 3;
	snake.y[0] = SIZE;

	snake.x[1] = SIZE * 2;
	snake.y[1] = SIZE;

	snake.x[2] = SIZE;
	snake.y[2] = SIZE;

	/*	说明:
	*	1. 其余蛇默认坐标为 0。
	*		如果初始长度 SNAKE_LENGTH 设置为更长，也无需初始化其他坐标
	*	2. 因为前三节蛇位置固定。
	*		因此，游戏开始时的S NAKE_TOWARD 蛇头方向，只能向右或向下。
	*		如果，向设定其他蛇头初始方向，需要修改前 SNAKE_TOWARD 节蛇的坐标。
	*/

	snake.count = 0;				// 初始化分数
	snake.len = SNAKE_LENGTH;		// 初始化长度
	snake.direction = SNAKE_TOWARD;	// 初始化蛇头方向
}

//初始化食物
void Init_Food()
{
	srand((unsigned int)time(NULL));		//随机种子
	int x = (WIDTH - FORM - SIZE) / SIZE;	//预留 FORM 像素显示信息
	int y = (HIGHT - SIZE) / SIZE;

	//检查食物是否在蛇身上
	int i = 0;
	while (i < snake.len)
	{
		if (food.x == snake.x[i] && food.y == snake.y[i])	//食物在蛇身上
		{
			food.x = rand() % x * SIZE;		// *SIZE 保证食物地址为整数
			food.y = rand() % y * SIZE;
		}
		if (food.x == 0 || food.y == 0)		//食物在上边界或左边界
		{
			food.x = rand() % x * SIZE;		
			food.y = rand() % y * SIZE;
		}

		++i;
	}
	food.flg = 1;
}

//贴图画蛇
void ShowSnake()
{
	/*	画蛇身	*/
	int i;
	for (i = 1; i < snake.len; ++i)	//若画蛇尾，条件为 i < snake.len - 1
	{
		putimage(snake.x[i], snake.y[i], &img[2]);
	}

	// 是否展示蛇尾
	if (SHOW_TAIL != 0) 
	{
		if (snake.y[i - 1] < snake.y[i])			//倒数第一节在尾巴的上边，蛇尾向上
		{
			putimage(snake.x[i], snake.y[i], &img[4]);
		}	
		else if (snake.y[i - 1] > snake.y[i])		//倒数第一节在尾巴的下边，蛇尾向下
		{
			putimage(snake.x[i], snake.y[i], &img[5]);
		}	
		else if (snake.x[i - 1] < snake.x[i])		//倒数第一节在尾巴的左边，蛇尾向左
		{
			putimage(snake.x[i], snake.y[i], &img[6]);
		}
		else if (snake.x[i - 1] > snake.x[i])		//倒数第一节在尾巴的右边，蛇尾向右
		{
			putimage(snake.x[i], snake.y[i], &img[7]);
		}
	}
	
	/*	画蛇头	*/
	putimage(snake.x[0], snake.y[0], &img[1]);

}

//贴图画食物
void ShowFood()
{
	putimage(food.x, food.y, &img[3]);
}

//蛇的移动
void MoveSnake()
{
	//把蛇的后一节坐标移动到前一节的坐标位置
	for (int i = snake.len; i > 0; --i)
	{
		snake.x[i] = snake.x[i - 1];
		snake.y[i] = snake.y[i - 1];
	}
	//单独移动蛇头，根据蛇的方向移动
	switch (snake.direction)
	{
	case right:
		snake.x[0] += SIZE;
		break;
	case left:
		snake.x[0] -= SIZE;
		break;
	case up:
		snake.y[0] -= SIZE;
		break;
	case down:
		snake.y[0] += SIZE;
		break;
	default:
		break;
	}
}

// 按键处理
void KeyDown()
{
	// 暂停，退出
	char tmp = _getch();

	if (tmp == ' ')				//判定空格 暂停
	{
		TCHAR ss[] = _T("暂停，按下任意键继续");
		outtextxy(WIDTH * 3 / 10, HIGHT / 2, ss);
		tmp = _getch();
		if (tmp == ' ') return;
	}
	else if (tmp == Esc)				//判定Esc
	{
		TCHAR s[] = _T("即将退出游戏，请确认:Esc键 确认");
		TCHAR s1[] = _T("          按任意键取消");
		outtextxy(WIDTH * 3 / 10, HIGHT / 2, s);		//指定位置输出字符串
		outtextxy(WIDTH * 3 / 10, HIGHT / 2 + 40, s1);	//指定位置输出字符串
		tmp = _getch();
		if (tmp == 27)	 exit(0);		// 退出
	}
	else if (tmp == -32 || tmp == 0) 
	{
		// ↑↓←→等按键，F1、F2等按键，有两个键值
		// 第二个是真实键值，在下面用 key = _getch(); 接收
	}	
	else 
	{
		// 键盘上 a\b\c\d 等按键，这里不做处理。
		return;
	}

	// 上下左右，加速减速
	char key = _getch();
	switch (key)
	{
	case right:
		if (snake.direction != left)	//不能相反方向移动
			snake.direction = right;
		break;
	case left:
		if (snake.direction != right)
			snake.direction = left;
		break;
	case up:
		if (snake.direction != down)
			snake.direction = up;
		break;
	case down:
		if (snake.direction != up)
			snake.direction = down;
		break;
	case F1:		//F1加速
		if (g_speed > 20)
		{
			g_speed -= 20;
		}
		break;
	case F2:		//F2减速
		if (g_speed < 280)
		{
			g_speed += 30;
		}
		break;
	default:
		break;

	}

}

//吃食物
void EatFood()
{
	if (food.x == snake.x[0] && food.y == snake.y[0])
	{
		++snake.len;
		snake.count += 10;	//一个食物十分
		food.flg = 0;
	}

}

//游戏结束
void GameOver()
{
	//通关
	if (snake.len == MAX_SIZE)		//最长长度
	{
		g_status = 1;
		return;
	}

	//撞墙
	else if (snake.x[0] < SIZE || snake.x[0] >= WIDTH - FORM - SIZE ||		//横坐标出界
		snake.y[0]<SIZE || snake.y[0]>HIGHT - SIZE * 2)		//纵坐标出界
	{
		g_status = 2;
		return;
	}

	for (int i = 1; i < snake.len; i++)
	{
		//撞自己
		if (snake.x[i] == snake.x[0] && snake.y[i] == snake.y[0])
		{
			g_status = 3;
			return;
		}
	}

}

//结束画面
int EndScreen()
{
	if (g_status == 0) // 游戏没结束
	{
		return 0;
	}

	TCHAR s[100] = "", s2[50] = "";
	if (g_status == 1)
	{
		_stprintf(s, _T("恭喜 通关 GameOver!\n您的分数为%d分"), snake.count);
	}
	else if (g_status == 2)
	{
		_stprintf(s, _T("你撞墙了 GameOver!\n您的分数为%d分"), snake.count);
	}
	else if (g_status == 3)
	{
		_stprintf(s, _T("咬到自己了 GameOver!\n您的分数为%d分"), snake.count);
	}
	_stprintf(s2, _T("       按任意键结束....\n"));

	ShowSnake();			//画蛇，显示死亡状态
	setbkmode(TRANSPARENT);		//文字透明方式
	settextcolor(LIGHTBLUE);	//文字颜色

	// 结束文字分数提示
	outtextxy(WIDTH * 2 / 10 + SIZE, HIGHT / 2 , s);		//指定位置输出字符串
	outtextxy(WIDTH * 2 / 10 + SIZE, HIGHT / 2 + SIZE, s2);	//指定位置输出字符串

	// 结束图片展示
	system("pause");
	putimage(0, 0, &tips[2]);		//贴图，结束界面
	outtextxy(WIDTH * 2 / 10 + SIZE, HIGHT / 2 + SIZE/2, s);	//指定位置输出字符串
	return 1;

}

// 画墙
void  DrawWall()
{
	for (int i = 0; i < WIDTH - FORM; i += SIZE)
	{
		putimage(i, 0, &img[0]);
	}
	for (int i = 0; i < WIDTH - FORM; i += SIZE)
	{
		putimage(i, HIGHT - SIZE, &img[0]);
	}
	for (int j = 0; j < WIDTH - FORM; j += SIZE)
	{
		putimage(0, j, &img[0]);
	}
	for (int j = 0; j < WIDTH - FORM; j += SIZE)
	{
		putimage(WIDTH - FORM - SIZE, j, &img[0]);
	}
}
