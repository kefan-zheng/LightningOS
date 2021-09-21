/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		               Kefan-Zheng,  2021   		
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

int numsysend = 0;

//进制转换
int numsys_main(int* fd_stdin)
{
	while(numsysend == 0)
	{
		int sys, num, orsys;
		int len = 0, single;
		int i = 0;
		char res[100];
		char rdbuff[128];
		int rdbufflen;

		printf("\n");
		printf("Please input the target system:");
		rdbufflen = read(*fd_stdin,rdbuff,128);
		rdbuff[rdbufflen] = '\0';
		sys = atoi(rdbuff);
		printf("Please input the num and its sys:");
		rdbufflen = read(*fd_stdin,rdbuff,128);
		char tmpary[5][64];
		int first = 0;
		int second = 0;
		for(int j = 0;j<rdbufflen;j++)
		{
			if(rdbuff[j] == ' ')
			{
				tmpary[first][second] = '\0';
				first = 1;
				second = 0;
				j++;
			}
			tmpary[first][second] = rdbuff[j];
			second++;
		}
		tmpary[first][second] = '\0';
		num = atoi(tmpary[0]);
		orsys = atoi(tmpary[1]);
		//将输入的数字转换成10进制的
		int tmp = 0;
		for (len; len <= 50; len++)
		{
			single = num % 10;
			tmp = tmp + single * pow(orsys, len);
			num = num / 10;
		}
		char p[30] = { 0,1,2,3,4,5,6,7,8,9,'A','B','C','D','E','F','G','H','I','J' };
		if (sys <= 10)
		{
			while (tmp >= sys)
			{
				res[i] = tmp % sys;
				tmp = tmp / sys;
				i = i + 1;
			}
			res[i] = tmp;
			printf("ans:");
			for (i; i >= 0; i--)
			{
				printf("%d", res[i]);
			}
		}
		else if (10 < sys && sys <= 20)
		{
			while (tmp >= sys)
			{
				res[i] = p[tmp % sys];
				tmp = tmp / sys;
				i = i + 1;
			}
			res[i] = p[tmp];
			printf("ans:");
			for (i; i >= 0; i--)
			{
				if (res[i] < 60)
				{
					printf("%d", res[i]);
				}
				else
				{
					printf("%c", res[i]);
				}
			}
		}
		printf("\nDo u want to try again?(y/n):");
		read(*fd_stdin,rdbuff,128);
		if(rdbuff[0] == 'n')
		{
			numsysend = 1;
		}
	}
	printf("\n");
	return 0;
}
