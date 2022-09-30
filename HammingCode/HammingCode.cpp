/*
Author:Ju5tice
Date:2022/10/1
*/
/*
二进制信道P(Y/X)=[6/7 1/7;1/7 6/7]，构造校验矩阵H
编程得出对应的生成矩阵G，输入8位二进制数据，利用GH对其进行编译码
给出x、c、r、c的估计
(7,4)汉明码，n=7,k=4
校验矩阵H为3*7矩阵，生成矩阵G为4*7矩阵，GH^T=0
*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define n 7
#define k 4

#define LEN 8

short H[n - k][n] = {//初始化校验矩阵
	{0,0,0,1,1,1,1},
	{0,1,1,0,0,1,1},
	{1,0,1,0,1,0,1}
	};

short G[k][n] = {//初始化生成矩阵
		{1,0,0,0,0,0,0},
		{0,1,0,0,0,0,0},
		{0,0,1,0,0,0,0},
		{0,0,0,1,0,0,0}
	};


void matrixDisplayH(short ptr[n - k][n]);
void matrixDisplayG(short ptr[k][n]);
void solveLE(short H[n - k][n], short G[k][n]);
short dec2bin(short src, short dest);
int Exp(int x, int y);
void Encode(short* msg, short* code);
void Decode(short* code, short* est);

void main()
{
	//-----------------解方程组产生G矩阵
	matrixDisplayH(H);
	solveLE(H, G);
	matrixDisplayG(G);
	//-----------------根据消息序列产生码字
	short message[LEN];
	printf("输入消息位:");
	scanf_s("%hd %hd %hd %hd %hd %hd %hd %hd", &message[0], &message[1], &message[2],
		&message[3], &message[4], &message[5], &message[6], &message[7]);
	printf("x: ");
	for (int i = 0; i < LEN; i++)
		printf("%hd ", message[i]);
	putchar('\n');
	short send[(LEN / k) * n];
	for (int i = 0; i < (LEN / k) * n; i++)
		send[i] = 0;

	for (int i = 0; i < LEN / k; i++)
		Encode(message + i * k, send + i * n);
	printf("c: ");
	for (int i = 0; i < (LEN / k) * n; i++)
		printf("%hd ",send[i]);
	putchar('\n');
	//----------------生成错误图像进行叠加
	short receive[(LEN / k) * n];
	for (int i = 0; i < (LEN / k) * n; i++)
		receive[i] = send[i];

	for (int i = 0; i < LEN / k; i++)
	{
		srand((unsigned)time(NULL) + 114514 * i);
		int j = rand() % 7;
		receive[i * n + j] = (receive[i * n + j] == 0) ? 1 : 0;
	}
	printf("r: ");
	for (int i = 0; i < (LEN / k) * n; i++)
		printf("%hd ", receive[i]);
	putchar('\n');
	//----------------接收端乘以校验矩阵
	short estlimate[(LEN / k) * n];
	for (short i = 0; i < (LEN / k) * n; i++)
		estlimate[i] = receive[i];
	for (int i = 0; i < LEN / k; i++)
		Decode(receive + i * n, estlimate + i * n);
	printf("c的估计: ");
	for (int i = 0; i < (LEN / k) * n; i++)
		printf("%hd ", estlimate[i]);
	putchar('\n');
}

void matrixDisplayH(short ptr[n-k][n])//显示(n-k)*n的矩阵，本程序中为H
{
	printf("校验矩阵H为:\n");
	for (short i = 1; i < n-k + 1; i++)
	{
		for (short j = 1; j < n + 1; j++)
		{
			printf("%hd", *(*(ptr + i - 1) + j - 1));
			putchar('  ');
		}
		putchar('\n');
	}
	putchar('\n');
}

void matrixDisplayG(short ptr[k][n])//显示k*n的矩阵，本程序中为G
{
	printf("生成矩阵G为:\n");
	for (short i = 1; i < k+1; i++)
	{
		for (short j = 1; j < n+1; j++)
		{
			printf("%hd",* (*(ptr + i - 1) + j - 1));
			putchar('  ');
		}
		putchar('\n');
	}
	putchar('\n');
}

void solveLE(short ptrH[n - k][n], short ptrG[k][n])
{
	short tempVect[n];


	for (int ti = 0; ti < k; ti++)//对G每一行进行求值
	{
		for (int i = 0; i < n; i++)
			tempVect[i] = 0;//初始化

		tempVect[n-1-ti] = 1;
		for (int i = 0; i < Exp(2, n - k); i++)
		{
			for (int j = 0; j < n - k; j++)
			{
				tempVect[j] = dec2bin(i, j);
			}//完成G的第ti+1行的赋值，开始检测
			//printf("tempVect的值为%d %d %d %d %d %d %d\n", tempVect[6],tempVect[5], tempVect[4], tempVect[3],tempVect[2],tempVect[1],tempVect[0]);
			int flag = 0;
			for (int j = 0; j < n - k; j++)//对H的每一行，H^T的每一列进行检测
			{
				int sum = 0;
				for (int temp = 0; temp < n; temp++)
				{
					sum += tempVect[temp] * H[j][n-1-temp];
					//printf("%d %d %d\n", tempVect[temp], H[j][n-1-temp], sum);
				}
				if (sum % 2 == 1)
				{
					flag = 1;
					//printf("%d不是正确解!\n",sum);
					break;
				}
			}
			if (flag == 0)
			{
				//printf("已找到G的第%d行的正确解!\n", ti+1);
				//printf("tempVect的值为%d %d %d %d %d %d %d\n", tempVect[6], tempVect[5], tempVect[4], tempVect[3], tempVect[2], tempVect[1], tempVect[0]);
				for (int j = 0; j < n; j++)
					G[ti][n-1-j] = tempVect[j];
			}

		}
	}
}

short dec2bin(short src, short dest)
{
	for (short i = 0; i < dest; i++)
	{
		src /= 2;
	}
	return src % 2;
}

int Exp(int x, int y)
{
	int j = x;
	for (int i = 0; i < y - 1; i++)
		x *= j;
	return x;
}

void Encode(short* msg, short* code)
{
	for (int i = 0; i < n; i++)
	{
		int sum = 0;
		for (int j = 0; j < k; j++)
			sum += msg[j] * G[j][i];
		code[i] = sum % 2;
	}
}

void Decode(short* code, short* est)
{
	short tempVect[n - k];
	for (int i = 0; i < n - k; i++)
		tempVect[i] = 0;

	for (int i = 0; i < n - k; i++)
	{
		int sum = 0;
		for (int j = 0; j < n; j++)
		{
			sum += code[j] * H[i][j];
		}
		tempVect[i] = sum % 2;
	}
	//printf("\n错误图谱：%d %d %d", tempVect[0], tempVect[1], tempVect[2]);
	for (int i = 0; i < n; i++)
	{
		if (tempVect[0] == H[0][i] && tempVect[1] == H[1][i] && tempVect[2] == H[2][i])
		{
			//printf("错误出在第%d位\n", i + 1);
			est[i] = (est[i] == 1) ? 0 : 1;
			break;
		}
	}
}