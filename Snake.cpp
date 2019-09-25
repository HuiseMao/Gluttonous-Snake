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

/*--------------------------------�� ��---------------------------*/
#define WIDE 1170		/* ���ڴ�С  WIDE * HIGH���� �ߴ���ΪSIZE����������  	 */
#define HIGH 640		/*	�ߵ��ж���A(SIZE,SIZE)��B(WIDE-FORM-SIZE,HIGH-SIZE)֮��ľ�����*/
#define FORM 250		/*	�������ֱ߿���  */
#define SIZE 40				//ÿ���ߵĳߴ�
#define LEN 100				//�ߵ������
#define COLOUR RGB(rand()%255,rand()%255,rand()%255)	//��ɫ

static int speed = 100;		//�ٶ�(����ˢ�µ�ʱ����)

enum Direction { right = 77, left = 75, down = 80, up = 72, Esc = 27 };//ö�ٷ���ֵ


/*---------------------------��Դ-------------------------------------*/
//��
typedef struct _snake
{
	int x[LEN];				//���ꡪX
	int y[LEN];				//���ꡪY
	int len;				//����
	int count;				//����
	int direction;			//����
}Snake;

//ʳ��
typedef struct _food
{
	int x;					//���ꡪX
	int y;					//���ꡪY
	int flg;				//���ʳ���Ƿ��Ѿ�����
}Food;


IMAGE img[8];		//ǽ����ͷ������,��β��ʳ��
char name[8][15] = { "wall","snakeHead","body","food","snakeTailUp","snakeTailDown","snakeTailLeft","snakeTailRight" };	//���ͼƬ������
IMAGE tips[4];		//��������ͼƬ
char tipsName[4][15] = { "snake","label","gameover","start" };	//���ͼƬ������
//����ͼƬ
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
int	 GameOver();		//��Ϸ����
void DrawWall();		//��ǽ



//�Ҳ�չʾ����
void PrintForm()
{
	//setlinecolor(BLACK);	//������ɫ		/*���������Ѵ���*/
	//setfillcolor(WHITE);	//�����ɫ
	fillrectangle(0, 0, WIDE, HIGH - 1);	//�߿�

	LOGFONT f;
	gettextstyle(&f);                     // ��ȡ��ǰ��������
	f.lfHeight = 30;                      // ��������߶�Ϊ 48
	_tcscpy(f.lfFaceName, _T("��Բ"));    // ��������Ϊ�����塱
	f.lfQuality = ANTIALIASED_QUALITY;    // �������Ч��Ϊ�����  
	settextstyle(&f);                     // ����������ʽ

	putimage(WIDE - FORM, 0, &tips[0]);			//��ͼ������̰ͨ����
	//putimage(WIDE-FORM,250,&tips[1]);		//��ʾ����ͼ�������ֲ�
	setbkmode(TRANSPARENT);					//����͸����ʽ
	settextcolor(LIGHTBLUE);				//������ɫ
	TCHAR  str[3][50];
	_stprintf(str[0], _T("  �÷�: %3d ��"), snake.count);
	_stprintf(str[1], _T("  �ٶ�: %3d ��"), (100 - speed) / 20);
	_stprintf(str[2], _T("  ����: %3d ��"), snake.len);
	for (int i = 0; i < 3; ++i)
	{
		outtextxy(WIDE - FORM + 10, i * 40 + 250, str[i]);	//ָ��λ������ַ���
	}

	TCHAR  str2[11][50];
	_stprintf(str2[0], _T(" ��ʾ:"));
	_stprintf(str2[1], _T("  һö��ʵ10��,���  "));
	_stprintf(str2[2], _T("  100�ڣ�Ϊͨ�ء�ײǽ,"));
	_stprintf(str2[3], _T("  ��ײ���Լ�����Ϸʧ��"));
	_stprintf(str2[4], _T("      "));
	_stprintf(str2[5], _T(" ����:"));
	_stprintf(str2[6], _T("     F1 : ���� "));
	_stprintf(str2[7], _T("     F2 : ����"));
	_stprintf(str2[8], _T("    �ո�: ��ͣ"));
	_stprintf(str2[9], _T("    Esc : �˳� "));
	_stprintf(str2[10], _T("     "));
	settextcolor(BLACK);			//������ɫ
	f.lfHeight = 20;                      // ��������߶�Ϊ 20
	settextstyle(&f);                     // ����������ʽ
	for (int i = 0; i < 11; i++)
	{
		outtextxy(WIDE - FORM + 10, 400 + i * 20, str2[i]);	//ָ��λ������ַ���
	}

}

int main()
{

	initgraph(WIDE, HIGH);		//��ʼ�����ڴ�С
	loadResources();			//����ͼƬ

	setlinecolor(BLACK);		//������ɫ
	setfillcolor(WHITE);		//�����ɫ
	fillrectangle(-1, -1, WIDE, HIGH);	//�߿�

	putimage(200, 50, &tips[3]);
	while (!_kbhit()); // û�а�������ѭ��   �����������ʼ
	//��ջ�����
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

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
			//��ջ�����
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		}

		EatFood();
		MoveSnake();			//���ƶ�

		if (GameOver() == 1)		//��Ϸ��������
		{
			//ShowSnake();			//����,��ʾ����״̬
			break;
		}
		Sleep(speed);			//�����ٶ�
	}

	_getch();
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

	snake.len = 3;
	snake.count = 0;
	snake.direction = right;	//Ĭ������
}
//��ʼ��ʳ��
void Init_Food()
{
	srand((unsigned int)time(NULL));//�������
label:
	int x = (WIDE - FORM - SIZE) / SIZE;	//Ԥ�� FORM ������ʾ��Ϣ
	int y = (HIGH - SIZE) / SIZE;
	food.x = rand() % x * SIZE;	// *10 ��֤ʳ���ַΪ����������ͷ�Ƚ�
	food.y = rand() % y * SIZE;
	//���ʳ���Ƿ���������
	int i = 0;
	while (i < snake.len)
	{
		if (food.x == snake.x[i] && food.y == snake.y[i])	//ʳ����������
		{
			goto label;
		}
		if (food.x == 0 || food.y == 0)		//ʳ�����ϱ߽����߽�
		{
			goto label;
		}

		++i;
	}
	food.flg = 1;
}
//��ͼ����
void ShowSnake()
{
	/*					������						*/
	int i;
	for (i = 1; i < snake.len; ++i)	//������β������Ϊ i < snake.len - 1
	{
		putimage(snake.x[i], snake.y[i], &img[2]);
	}
	/*					����β						*/
	//ע��û�кÿ�����β����ʱ������β����������βֻ������� ע�Ͷ�ȡ�����ѻ������������һ ����
	//if (snake.y[i - 1] < snake.y[i])			//������һ����β�͵��ϱߣ���β����
	//{
	//	putimage(snake.x[i], snake.y[i], &img[4]);
	//}	
	//else if (snake.y[i - 1] > snake.y[i])		//������һ����β�͵��±ߣ���β����
	//{
	//	putimage(snake.x[i], snake.y[i], &img[5]);
	//}	
	//else if (snake.x[i - 1] < snake.x[i])		//������һ����β�͵���ߣ���β����
	//{
	//	putimage(snake.x[i], snake.y[i], &img[6]);
	//}
	//else if (snake.x[i - 1] > snake.x[i])		//������һ����β�͵��ұߣ���β����
	//{
	//	putimage(snake.x[i], snake.y[i], &img[7]);
	//}
	/*					����ͷ						*/
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
	for (int i = snake.len - 1; i > 0; --i)
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



/*getch�����ӿ���̨��ȡ�����ַ��������ԣ���������ȥ��ȡCTRL+C������ȡһ��
	���ܼ����������������������Σ����˵���������������ȥ��ع��ܼ�
	�ͷ����������һ�ε��÷���0��0xe0���ڶ��η���ʵ�ʵļ�����*/
	//����
void KeyDown()
{
	char tmp = _getch();			//����_getch()�ĵ�һ������ֵ����ǹ��ܰ���
	if (tmp == -32 || tmp == 0) {}				//���ܼ���_getch()��һ������ֵ -32\'?' ��0
	else if (tmp == ' ')				//�ж��ո���ͣ
	{
		TCHAR ss[] = _T("��ͣ�����¿ո����");
		outtextxy(WIDE * 3 / 10, HIGH / 2, ss);
	suspend:
		while (!_kbhit()); // û�а�������ѭ��   �����������ʼ
		if (_getch() != ' ')	goto suspend;
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		return;
	}
	else if (tmp == 27)				//�ж�Esc
	{
		TCHAR s[] = _T("�����˳���Ϸ����ȷ��:Esc�� ȷ��");
		TCHAR s1[] = _T("          �������ȡ��");
		outtextxy(WIDE * 3 / 10, HIGH / 2, s);	//ָ��λ������ַ���
		outtextxy(WIDE * 3 / 10, HIGH / 2 + 40, s1);	//ָ��λ������ַ���
		while (!_kbhit()); // û�а�������ѭ��   �����������ʼ
		if (_getch() == 27)	 exit(0);
		return;
	}
	else return;

	char key = _getch();		//���ܼ��̼����ֵ
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
	case 59:		//F1����
		if (speed > 20)
		{
			speed -= 20;
		}
		break;
	case 60:		//F2����
		if (speed < 280)
		{
			speed += 30;
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



//����
int GameOver()
{
	//ͨ��
	if (snake.len == LEN)		//�����
	{
		TCHAR s[100];
		_stprintf(s, _T("��ϲ ͨ�� GameOver!\n���ķ���Ϊ%d��"), snake.count);

		//putimage(0,0,&tips[2]);	//��ͼ����������
		setbkmode(TRANSPARENT);		//����͸����ʽ
		settextcolor(LIGHTBLUE);	//������ɫ
		outtextxy(WIDE * 2 / 10, HIGH / 2, s);	//ָ��λ������ַ���
		return 1;
	}

	//ײǽ
	else if (snake.x[0] < SIZE || snake.x[0] >= WIDE - FORM - SIZE ||		//���������
		snake.y[0]<SIZE || snake.y[0]>HIGH - SIZE * 2)		//���������
	{
		TCHAR s[100];
		_stprintf(s, _T("��ײǽ�� GameOver!\n���ķ���Ϊ%d��"), snake.count);

		setbkmode(TRANSPARENT);			//����͸����ʽ
		settextcolor(LIGHTBLUE);		//������ɫ
		outtextxy(WIDE * 2 / 10, HIGH / 2, s);	//ָ��λ������ַ���
		return 1;
	}

	for (int i = 1; i < snake.len; i++)
	{
		//ײ�Լ�
		if (snake.x[i] == snake.x[0] && snake.y[i] == snake.y[0])
		{
			TCHAR s[100];
			_stprintf(s, _T("ҧ���Լ��� GameOver!\n���ķ���Ϊ%d��"), snake.count);

			setbkmode(TRANSPARENT);			//����͸����ʽ
			settextcolor(LIGHTBLUE);		//������ɫ
			outtextxy(WIDE * 2 / 10, HIGH / 2, s);	//ָ��λ������ַ���

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
