#undef UNICODE
#undef _UNICODE		
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#pragma warning(disable:4996)	//���ð�ȫ��� _tcscpy��_stprintf

/*--------------------------------�� ��---------------------------*/
#define WIDTH 1170		/*	���ڴ�С  WIDTH * HIGHT ���� �ߴ���Ϊ SIZE ����������  	 */
#define HIGHT 640		/*	�ߵ��ж��� A(SIZE,SIZE)�� B(WIDTH-FORM-SIZE,HIGHT-SIZE)֮��ľ�����*/
#define FORM 250		/*	�������ֱ߿���	*/
#define SIZE 40			/*	ÿ���ߵĳߴ�		*/

#define COLOUR RGB(rand()%255,rand()%255,rand()%255)	/*	��ɫ */

// ö��ֵ
enum Direction { right = 77, left = 75, down = 80, up = 72 };	//ö�ٷ���ֵ
enum Control   { F1 = 59, F2 = 60, Esc = 27 };					//ö�ٹ��ܼ�ֵ

// ȫ�ֱ���
static int g_speed = 100;			//	�ٶ�(����ˢ�µ�ʱ����)
static int g_status = 0;			//	��Ϸ������־

// ȫ�ֳ���
const char* IMAGE_PATH = "image";	//	��ͼ��Դ����·��
const int SNAKE_LENGTH = 3;			//	��ʼ��Ĭ�ϳ�ʼ����
const int SNAKE_TOWARD = right;		//	��ʼ��Ĭ����ͷ����
const int MAX_SIZE = 100;			//	�ߵ������
const int SHOW_TAIL = 0;			//	�Ƿ�չʾ��β�ͣ���β�����볤��



/*---------------------------��Դ-------------------------------------*/
//��
typedef struct _snake
{
	int x[MAX_SIZE];		//���ꡪX
	int y[MAX_SIZE];		//���ꡪY
	int len;				//��ǰ����
	int count;				//��ǰ����
	int direction;			//��ͷ����
}Snake;

//ʳ��
typedef struct _food
{
	int x;					//���ꡪX
	int y;					//���ꡪY
	int flg;				//���ʳ���Ƿ��Ѿ�����
}Food;

// ͼƬ����
IMAGE img[8];		//ǽ����ͷ�����壬��β��ʳ��
IMAGE tips[4];		//��������ͼƬ

// ���ͼƬ������
char name[8][15] = { "wall","snakeHead","body","food","snakeTailUp","snakeTailDown","snakeTailLeft","snakeTailRight" };
char tipsName[4][15] = { "snake","label","gameover","start" };	

//����ͼƬ
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
//����
Snake snake;		//��
Food  food;			//ʳ��
//����
void PrintForm();		//������ʾ��
void Init_Snake();		//��ʼ����
void Init_Food();		//��ʼ��ʳ��
void ShowSnake();		//��ͼ����
void ShowFood();		//��ͼ��ʳ��
void MoveSnake();		//�ߵ��ƶ�
void KeyDown();			//����
void EatFood();			//��ʳ��
void GameOver();		//��Ϸ����
int EndScreen();		//��������
void DrawWall();		//��ǽ



//�Ҳ�չʾ����
void PrintForm()
{
	fillrectangle(0, 0, WIDTH, HIGHT - 1);	//�߿�

	LOGFONT f;
	gettextstyle(&f);                     // ��ȡ��ǰ��������
	f.lfHeight = 30;                      // ��������߶�Ϊ 48
	_tcscpy(f.lfFaceName, _T("��Բ"));    // ��������Ϊ�����塱
	f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
	settextstyle(&f);                     // ����������ʽ

	putimage(WIDTH - FORM, 0, &tips[0]);	//��ͼ������̰ͨ����
	//putimage(WIDTH-FORM,250,&tips[1]);	//��ʾ����ͼ�������ֲ�
	setbkmode(TRANSPARENT);					//����͸����ʽ
	settextcolor(LIGHTBLUE);				//������ɫ
	TCHAR  str[3][50];
	_stprintf(str[0], _T("  �÷�: %3d ��"), snake.count);
	_stprintf(str[1], _T("  �ٶ�: %3d ��"), (100 - g_speed) / 20);
	_stprintf(str[2], _T("  ����: %3d ��"), snake.len);
	for (int i = 0; i < 3; ++i)
	{
		outtextxy(WIDTH - FORM + 10, i * 40 + 250, str[i]);	//ָ��λ������ַ���
	}

	TCHAR  str2[11][50];
	_stprintf(str2[0], _T(" ��ʾ:"));
	_stprintf(str2[1], _T("  һö��ʵ10�֣����  "));
	_stprintf(str2[2], _T("  100�ڣ�Ϊͨ�ء�ײǽ,"));
	_stprintf(str2[3], _T("  ��ײ���Լ�����Ϸʧ��"));
	_stprintf(str2[4], _T("      "));
	_stprintf(str2[5], _T(" ����:"));
	_stprintf(str2[6], _T("     F1 : ���� "));
	_stprintf(str2[7], _T("     F2 : ����"));
	_stprintf(str2[8], _T("    �ո�: ��ͣ"));
	_stprintf(str2[9], _T("    Esc : �˳� "));
	_stprintf(str2[10], _T("     "));
	settextcolor(BLACK);				//������ɫ
	f.lfHeight = 20;					// ��������߶�Ϊ 20
	settextstyle(&f);					// ����������ʽ
	for (int i = 0; i < 11; i++)
	{
		outtextxy(WIDTH - FORM + 10, 400 + i * 20, str2[i]);	//ָ��λ������ַ���
	}

}

int main()
{
	initgraph(WIDTH, HIGHT);	//��ʼ�����ڴ�С
	loadResources();			//����ͼƬ

	setlinecolor(BLACK);		//������ɫ
	setfillcolor(WHITE);		//�����ɫ
	fillrectangle(-1, -1, WIDTH, HIGHT);	//�߿�

	putimage(200, 50, &tips[3]);	
	_getch();					// �����������ʼ

	Init_Snake();				//��ʼ����
	Init_Food();				//��ʼ��ʳ��

	while (true)//ǿ��
	{
		if (food.flg == 0)
		{
			Init_Food();		//��ʼ��ʳ��
		}

		BeginBatchDraw();
		cleardevice();			//ˢ�´���
		PrintForm();			//��ӡ���
		DrawWall();				//��ǽ
		ShowFood();				//��ʳ��
		ShowSnake();			//����
		EndBatchDraw();

		if (_kbhit())
		{
			KeyDown();			//��ҿ������ƶ�
		}

		MoveSnake();			//���ƶ�
		EatFood();				//��ʳ��

		GameOver();				//�����Ϸ�Ƿ���������ý�����־λ
		if(EndScreen() == 1)	//��Ϸ������չʾ�ɼ�		
		{
			break;
		}
		Sleep(g_speed);			//�����ٶ�
	}

	system("pause");
	closegraph();
	return 0;
}
/*--------------------------�ߵĹ��ܺ���--------------------------*/

//��ʼ����
void Init_Snake()
{
	//��ʼ��ǰ������
	snake.x[0] = SIZE * 3;
	snake.y[0] = SIZE;

	snake.x[1] = SIZE * 2;
	snake.y[1] = SIZE;

	snake.x[2] = SIZE;
	snake.y[2] = SIZE;

	/*	˵��:
	*	1. ������Ĭ������Ϊ 0��
	*		�����ʼ���� SNAKE_LENGTH ����Ϊ������Ҳ�����ʼ����������
	*	2. ��Ϊǰ������λ�ù̶���
	*		��ˣ���Ϸ��ʼʱ��S NAKE_TOWARD ��ͷ����ֻ�����һ����¡�
	*		��������趨������ͷ��ʼ������Ҫ�޸�ǰ SNAKE_TOWARD ���ߵ����ꡣ
	*/

	snake.count = 0;				// ��ʼ������
	snake.len = SNAKE_LENGTH;		// ��ʼ������
	snake.direction = SNAKE_TOWARD;	// ��ʼ����ͷ����
}

//��ʼ��ʳ��
void Init_Food()
{
	srand((unsigned int)time(NULL));		//�������
	int x = (WIDTH - FORM - SIZE) / SIZE;	//Ԥ�� FORM ������ʾ��Ϣ
	int y = (HIGHT - SIZE) / SIZE;

	//���ʳ���Ƿ���������
	int i = 0;
	while (i < snake.len)
	{
		if (food.x == snake.x[i] && food.y == snake.y[i])	//ʳ����������
		{
			food.x = rand() % x * SIZE;		// *SIZE ��֤ʳ���ַΪ����
			food.y = rand() % y * SIZE;
		}
		if (food.x == 0 || food.y == 0)		//ʳ�����ϱ߽����߽�
		{
			food.x = rand() % x * SIZE;		
			food.y = rand() % y * SIZE;
		}

		++i;
	}
	food.flg = 1;
}

//��ͼ����
void ShowSnake()
{
	/*	������	*/
	int i;
	for (i = 1; i < snake.len; ++i)	//������β������Ϊ i < snake.len - 1
	{
		putimage(snake.x[i], snake.y[i], &img[2]);
	}

	// �Ƿ�չʾ��β
	if (SHOW_TAIL != 0) 
	{
		if (snake.y[i - 1] < snake.y[i])			//������һ����β�͵��ϱߣ���β����
		{
			putimage(snake.x[i], snake.y[i], &img[4]);
		}	
		else if (snake.y[i - 1] > snake.y[i])		//������һ����β�͵��±ߣ���β����
		{
			putimage(snake.x[i], snake.y[i], &img[5]);
		}	
		else if (snake.x[i - 1] < snake.x[i])		//������һ����β�͵���ߣ���β����
		{
			putimage(snake.x[i], snake.y[i], &img[6]);
		}
		else if (snake.x[i - 1] > snake.x[i])		//������һ����β�͵��ұߣ���β����
		{
			putimage(snake.x[i], snake.y[i], &img[7]);
		}
	}
	
	/*	����ͷ	*/
	putimage(snake.x[0], snake.y[0], &img[1]);

}

//��ͼ��ʳ��
void ShowFood()
{
	putimage(food.x, food.y, &img[3]);
}

//�ߵ��ƶ�
void MoveSnake()
{
	//���ߵĺ�һ�������ƶ���ǰһ�ڵ�����λ��
	for (int i = snake.len; i > 0; --i)
	{
		snake.x[i] = snake.x[i - 1];
		snake.y[i] = snake.y[i - 1];
	}
	//�����ƶ���ͷ�������ߵķ����ƶ�
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

// ��������
void KeyDown()
{
	// ��ͣ���˳�
	char tmp = _getch();

	if (tmp == ' ')				//�ж��ո� ��ͣ
	{
		TCHAR ss[] = _T("��ͣ���������������");
		outtextxy(WIDTH * 3 / 10, HIGHT / 2, ss);
		tmp = _getch();
		if (tmp == ' ') return;
	}
	else if (tmp == Esc)				//�ж�Esc
	{
		TCHAR s[] = _T("�����˳���Ϸ����ȷ��:Esc�� ȷ��");
		TCHAR s1[] = _T("          �������ȡ��");
		outtextxy(WIDTH * 3 / 10, HIGHT / 2, s);		//ָ��λ������ַ���
		outtextxy(WIDTH * 3 / 10, HIGHT / 2 + 40, s1);	//ָ��λ������ַ���
		tmp = _getch();
		if (tmp == 27)	 exit(0);		// �˳�
	}
	else if (tmp == -32 || tmp == 0) 
	{
		// ���������Ȱ�����F1��F2�Ȱ�������������ֵ
		// �ڶ�������ʵ��ֵ���������� key = _getch(); ����
	}	
	else 
	{
		// ������ a\b\c\d �Ȱ��������ﲻ������
		return;
	}

	// �������ң����ټ���
	char key = _getch();
	switch (key)
	{
	case right:
		if (snake.direction != left)	//�����෴�����ƶ�
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
	case F1:		//F1����
		if (g_speed > 20)
		{
			g_speed -= 20;
		}
		break;
	case F2:		//F2����
		if (g_speed < 280)
		{
			g_speed += 30;
		}
		break;
	default:
		break;

	}

}

//��ʳ��
void EatFood()
{
	if (food.x == snake.x[0] && food.y == snake.y[0])
	{
		++snake.len;
		snake.count += 10;	//һ��ʳ��ʮ��
		food.flg = 0;
	}

}

//��Ϸ����
void GameOver()
{
	//ͨ��
	if (snake.len == MAX_SIZE)		//�����
	{
		g_status = 1;
		return;
	}

	//ײǽ
	else if (snake.x[0] < SIZE || snake.x[0] >= WIDTH - FORM - SIZE ||		//���������
		snake.y[0]<SIZE || snake.y[0]>HIGHT - SIZE * 2)		//���������
	{
		g_status = 2;
		return;
	}

	for (int i = 1; i < snake.len; i++)
	{
		//ײ�Լ�
		if (snake.x[i] == snake.x[0] && snake.y[i] == snake.y[0])
		{
			g_status = 3;
			return;
		}
	}

}

//��������
int EndScreen()
{
	if (g_status == 0) // ��Ϸû����
	{
		return 0;
	}

	TCHAR s[100] = "", s2[50] = "";
	if (g_status == 1)
	{
		_stprintf(s, _T("��ϲ ͨ�� GameOver!\n���ķ���Ϊ%d��"), snake.count);
	}
	else if (g_status == 2)
	{
		_stprintf(s, _T("��ײǽ�� GameOver!\n���ķ���Ϊ%d��"), snake.count);
	}
	else if (g_status == 3)
	{
		_stprintf(s, _T("ҧ���Լ��� GameOver!\n���ķ���Ϊ%d��"), snake.count);
	}
	_stprintf(s2, _T("       �����������....\n"));

	ShowSnake();			//���ߣ���ʾ����״̬
	setbkmode(TRANSPARENT);		//����͸����ʽ
	settextcolor(LIGHTBLUE);	//������ɫ

	// �������ַ�����ʾ
	outtextxy(WIDTH * 2 / 10 + SIZE, HIGHT / 2 , s);		//ָ��λ������ַ���
	outtextxy(WIDTH * 2 / 10 + SIZE, HIGHT / 2 + SIZE, s2);	//ָ��λ������ַ���

	// ����ͼƬչʾ
	system("pause");
	putimage(0, 0, &tips[2]);		//��ͼ����������
	outtextxy(WIDTH * 2 / 10 + SIZE, HIGHT / 2 + SIZE/2, s);	//ָ��λ������ַ���
	return 1;

}

// ��ǽ
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
