#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#undef UNICODE
#undef _UNICODE		//
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

/*--------------------------------属 性---------------------------*/
#define WIDE 1170		/* 窗口大小  WIDE * HIGH像素 尺寸需为SIZE的整数倍，  	 */
#define HIGH 640		/*	蛇的行动在A(SIZE,SIZE)到B(WIDE-FORM-SIZE,HIGH-SIZE)之间的矩形内*/
#define FORM 250		/*	设置文字边框宽度  */
#define SIZE 40				//每节蛇的尺寸
#define LEN 100				//蛇的最长长度
#define COLOUR RGB(rand()%255,rand()%255,rand()%255)	//彩色

static int speed = 100;		//速度(控制刷新的时间间隔)

enum Direction { right = 77, left = 75, down = 80, up = 72, Esc = 27 };//枚举方向值


/*---------------------------资源-------------------------------------*/
//蛇
typedef struct _snake
{
	int x[LEN];				//坐标—X
	int y[LEN];				//坐标—Y
	int len;				//长度
	int count;				//分数
	int direction;			//方向
}Snake;

//食物
typedef struct _food
{
	int x;					//坐标—X
	int y;					//坐标—Y
	int flg;				//标记食物是否已经出现
}Food;


IMAGE img[8];		//墙，蛇头，身体,蛇尾，食物
char name[8][15] = { "wall","snakeHead","body","food","snakeTailUp","snakeTailDown","snakeTailLeft","snakeTailRight" };	//标记图片变量名
IMAGE tips[4];		//其他界面图片
char tipsName[4][15] = { "snake","label","gameover","start" };	//标记图片变量名
//加载图片
void loadResources()
{
	for (int i = 0; i < 8; i++)
	{
		char filename[20] = "";
		sprintf_s(filename, "%s.bmp", name[i]);
		loadimage(img + i, filename);
	}
	for (int i = 0; i < 4; i++)
	{
		char filename[20] = "";
		sprintf_s(filename, "%s.bmp", tipsName[i]);
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
int	 GameOver();		//游戏结束
void DrawWall();		//画墙



//右侧展示区域
void PrintForm()
{
	//setlinecolor(BLACK);	//画线颜色		/*主函数中已存在*/
	//setfillcolor(WHITE);	//填充颜色
	fillrectangle(0, 0, WIDE, HIGH - 1);	//边框

	LOGFONT f;
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfHeight = 30;                      // 设置字体高度为 48
	_tcscpy(f.lfFaceName, _T("幼圆"));    // 设置字体为“黑体”
	f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);                     // 设置字体样式

	putimage(WIDE - FORM, 0, &tips[0]);			//贴图——卡通贪吃蛇
	//putimage(WIDE-FORM,250,&tips[1]);		//提示框贴图，操作手册
	setbkmode(TRANSPARENT);					//文字透明方式
	settextcolor(LIGHTBLUE);				//文字颜色
	TCHAR  str[3][50];
	_stprintf(str[0], _T("  得分: %3d 分"), snake.count);
	_stprintf(str[1], _T("  速度: %3d 级"), (100 - speed) / 20);
	_stprintf(str[2], _T("  长度: %3d 节"), snake.len);
	for (int i = 0; i < 3; ++i)
	{
		outtextxy(WIDE - FORM + 10, i * 40 + 250, str[i]);	//指定位置输出字符串
	}

	TCHAR  str2[11][50];
	_stprintf(str2[0], _T(" 提示:"));
	_stprintf(str2[1], _T("  一枚果实10分,蛇最长  "));
	_stprintf(str2[2], _T("  100节，为通关。撞墙,"));
	_stprintf(str2[3], _T("  或撞到自己，游戏失败"));
	_stprintf(str2[4], _T("      "));
	_stprintf(str2[5], _T(" 按键:"));
	_stprintf(str2[6], _T("     F1 : 加速 "));
	_stprintf(str2[7], _T("     F2 : 减速"));
	_stprintf(str2[8], _T("    空格: 暂停"));
	_stprintf(str2[9], _T("    Esc : 退出 "));
	_stprintf(str2[10], _T("     "));
	settextcolor(BLACK);			//文字颜色
	f.lfHeight = 20;                      // 设置字体高度为 20
	settextstyle(&f);                     // 设置字体样式
	for (int i = 0; i < 11; i++)
	{
		outtextxy(WIDE - FORM + 10, 400 + i * 20, str2[i]);	//指定位置输出字符串
	}

}

int main()
{

	initgraph(WIDE, HIGH);		//初始化窗口大小
	loadResources();			//加载图片

	setlinecolor(BLACK);		//画线颜色
	setfillcolor(WHITE);		//填充颜色
	fillrectangle(-1, -1, WIDE, HIGH);	//边框

	putimage(200, 50, &tips[3]);
	while (!_kbhit()); // 没有按键就死循环   按下任意键开始
	//清空缓冲区
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

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
			//清空缓冲区
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		}

		EatFood();
		MoveSnake();			//蛇移动

		if (GameOver() == 1)		//游戏结束条件
		{
			//ShowSnake();			//画蛇,显示死亡状态
			break;
		}
		Sleep(speed);			//控制速度
	}

	_getch();
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

	snake.len = 3;
	snake.count = 0;
	snake.direction = right;	//默认向右
}
//初始化食物
void Init_Food()
{
	srand((unsigned int)time(NULL));//随机种子
label:
	int x = (WIDE - FORM - SIZE) / SIZE;	//预留 FORM 像素显示信息
	int y = (HIGH - SIZE) / SIZE;
	food.x = rand() % x * SIZE;	// *10 保证食物地址为整数，与蛇头比较
	food.y = rand() % y * SIZE;
	//检查食物是否在蛇身上
	int i = 0;
	while (i < snake.len)
	{
		if (food.x == snake.x[i] && food.y == snake.y[i])	//食物在蛇身上
		{
			goto label;
		}
		if (food.x == 0 || food.y == 0)		//食物在上边界或左边界
		{
			goto label;
		}

		++i;
	}
	food.flg = 1;
}
//贴图画蛇
void ShowSnake()
{
	/*					画蛇身						*/
	int i;
	for (i = 1; i < snake.len; ++i)	//若画蛇尾，条件为 i < snake.len - 1
	{
		putimage(snake.x[i], snake.y[i], &img[2]);
	}
	/*					画蛇尾						*/
	//注：没有好看的蛇尾，暂时不贴蛇尾，如想贴蛇尾只需把以下 注释段取消，把画蛇身的条件减一 即可
	//if (snake.y[i - 1] < snake.y[i])			//倒数第一节在尾巴的上边，蛇尾向上
	//{
	//	putimage(snake.x[i], snake.y[i], &img[4]);
	//}	
	//else if (snake.y[i - 1] > snake.y[i])		//倒数第一节在尾巴的下边，蛇尾向下
	//{
	//	putimage(snake.x[i], snake.y[i], &img[5]);
	//}	
	//else if (snake.x[i - 1] < snake.x[i])		//倒数第一节在尾巴的左边，蛇尾向左
	//{
	//	putimage(snake.x[i], snake.y[i], &img[6]);
	//}
	//else if (snake.x[i - 1] > snake.x[i])		//倒数第一节在尾巴的右边，蛇尾向右
	//{
	//	putimage(snake.x[i], snake.y[i], &img[7]);
	//}
	/*					画蛇头						*/
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
	for (int i = snake.len - 1; i > 0; --i)
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



/*getch函数从控制台读取单个字符而不回显，函数不能去读取CTRL+C，当读取一个
	功能键或方向键，函数必须调用两次（这就说明可以用这个函数去监控功能键
	和方向键），第一次调用返回0或0xe0，第二次返回实际的键代码*/
	//控制
void KeyDown()
{
	char tmp = _getch();			//接受_getch()的第一个返回值，或非功能按键
	if (tmp == -32 || tmp == 0) {}				//功能键，_getch()第一个返回值 -32\'?' 或0
	else if (tmp == ' ')				//判定空格暂停
	{
		TCHAR ss[] = _T("暂停，按下空格继续");
		outtextxy(WIDE * 3 / 10, HIGH / 2, ss);
	suspend:
		while (!_kbhit()); // 没有按键就死循环   按下任意键开始
		if (_getch() != ' ')	goto suspend;
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		return;
	}
	else if (tmp == 27)				//判定Esc
	{
		TCHAR s[] = _T("即将退出游戏，请确认:Esc键 确认");
		TCHAR s1[] = _T("          按任意键取消");
		outtextxy(WIDE * 3 / 10, HIGH / 2, s);	//指定位置输出字符串
		outtextxy(WIDE * 3 / 10, HIGH / 2 + 40, s1);	//指定位置输出字符串
		while (!_kbhit()); // 没有按键就死循环   按下任意键开始
		if (_getch() == 27)	 exit(0);
		return;
	}
	else return;

	char key = _getch();		//接受键盘键入的值
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
	case 59:		//F1加速
		if (speed > 20)
		{
			speed -= 20;
		}
		break;
	case 60:		//F2减速
		if (speed < 280)
		{
			speed += 30;
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



//死亡
int GameOver()
{
	//通关
	if (snake.len == LEN)		//最长长度
	{
		TCHAR s[100];
		_stprintf(s, _T("恭喜 通关 GameOver!\n您的分数为%d分"), snake.count);

		//putimage(0,0,&tips[2]);	//贴图，结束界面
		setbkmode(TRANSPARENT);		//文字透明方式
		settextcolor(LIGHTBLUE);	//文字颜色
		outtextxy(WIDE * 2 / 10, HIGH / 2, s);	//指定位置输出字符串
		return 1;
	}

	//撞墙
	else if (snake.x[0] < SIZE || snake.x[0] >= WIDE - FORM - SIZE ||		//横坐标出界
		snake.y[0]<SIZE || snake.y[0]>HIGH - SIZE * 2)		//纵坐标出界
	{
		TCHAR s[100];
		_stprintf(s, _T("你撞墙了 GameOver!\n您的分数为%d分"), snake.count);

		setbkmode(TRANSPARENT);			//文字透明方式
		settextcolor(LIGHTBLUE);		//文字颜色
		outtextxy(WIDE * 2 / 10, HIGH / 2, s);	//指定位置输出字符串
		return 1;
	}

	for (int i = 1; i < snake.len; i++)
	{
		//撞自己
		if (snake.x[i] == snake.x[0] && snake.y[i] == snake.y[0])
		{
			TCHAR s[100];
			_stprintf(s, _T("咬到自己了 GameOver!\n您的分数为%d分"), snake.count);

			setbkmode(TRANSPARENT);			//文字透明方式
			settextcolor(LIGHTBLUE);		//文字颜色
			outtextxy(WIDE * 2 / 10, HIGH / 2, s);	//指定位置输出字符串

			return 1;
		}
	}

	return 0;
}

void  DrawWall()
{
	for (int i = 0; i < WIDE - FORM; i += SIZE)
	{
		putimage(i, 0, &img[0]);
	}
	for (int i = 0; i < WIDE - FORM; i += SIZE)
	{
		putimage(i, HIGH - SIZE, &img[0]);
	}
	for (int j = 0; j < WIDE - FORM; j += SIZE)
	{
		putimage(0, j, &img[0]);
	}
	for (int j = 0; j < WIDE - FORM; j += SIZE)
	{
		putimage(WIDE - FORM - SIZE, j, &img[0]);
	}
}
