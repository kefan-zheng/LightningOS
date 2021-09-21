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
#include "string.h"

#define false 0
#define true 1
#define NULL ((void*)0)

//节点类，用于存储表达式中各个部分的值
struct Node
{
    //存储的数据类型（0为整型，1为字符）
	int type;
	int intnum;
	char charnum;
	//栈内优先级
	int isp;
	//栈外优先级
	int icp;
	//下一个结点地址
	struct Node* next;
};

struct Node allnode[100];
int cnt = 0;
  
//链式栈，用于将中缀表达式修改为后缀表达式
struct Stack
{
    //栈顶元素地址
	struct Node* top;
	//栈的大小
	int size;
};

int Stack_IsEmpty(struct Stack* this) 
{
    if (this->size > 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void Stack_Push(struct Stack** this, struct Node* address)
{
	struct Node* temp = &(allnode[cnt]);
	cnt++;
	temp->type = address->type;
	temp->intnum = address->intnum;
	temp->charnum = address->charnum;
	temp->next = (*this)->top;
	switch (temp->charnum)
	{
	case'(':temp->isp = 1; temp->icp = 8; break;
	case')':temp->isp = 8; temp->icp = 1; break;
	case'+':temp->isp = 3; temp->icp = 2; break;
	case'-':temp->isp = 3; temp->icp = 2; break;
	case'*':temp->isp = 5; temp->icp = 4; break;
	case'/':temp->isp = 5; temp->icp = 4; break;
	case'%':temp->isp = 5; temp->icp = 4; break;
	case'^':temp->isp = 7; temp->icp = 6; break;
	case'#':temp->isp = 0; temp->icp = 0; break;
	}
	(*this)->top = temp;
	(*this)->size++;
}

void Stack_Pop(struct Stack** this)
{
	if (Stack_IsEmpty((*this))==0)
	{
		(*this)->top = (*this)->top->next;
		(*this)->size--;
	}
}

//队列，用于存放表达式
struct Queue
{
    //队列头尾地址
	struct Node* front;
	struct Node* rear;
	//队列的长度
	int size;
};

int Queue_IsEmpty(struct Queue* this) { return (this->size < 1) ? 1 : 0; }

void Queue_EnQueue(struct Queue** this, struct Node* address)
{
	//队列为空
	if ((*this)->front == NULL)
	{
		struct Node* tmp = &(allnode[cnt]);
		cnt++;
		(*this)->front = tmp;
		(*this)->rear = (*this)->front;
		(*this)->rear->type = address->type;
		(*this)->rear->intnum = address->intnum;
		(*this)->rear->charnum = address->charnum;
	}
	//队列非空
	else
	{
		struct Node* tmp = &(allnode[cnt]);
		cnt++;
		(*this)->rear->next = tmp;
		(*this)->rear = (*this)->rear->next;
		(*this)->rear->type = address->type;
		(*this)->rear->intnum = address->intnum;
		(*this)->rear->charnum = address->charnum;
	}
	switch ((*this)->rear->charnum)
	{
	case'(':(*this)->rear->isp = 1; (*this)->rear->icp = 8; break;
	case')':(*this)->rear->isp = 8; (*this)->rear->icp = 1; break;
	case'+':(*this)->rear->isp = 3; (*this)->rear->icp = 2; break;
	case'-':(*this)->rear->isp = 3; (*this)->rear->icp = 2; break;
	case'*':(*this)->rear->isp = 5; (*this)->rear->icp = 4; break;
	case'/':(*this)->rear->isp = 5; (*this)->rear->icp = 4; break;
	case'%':(*this)->rear->isp = 5; (*this)->rear->icp = 4; break;
	case'^':(*this)->rear->isp = 7; (*this)->rear->icp = 6; break;
	case'#':(*this)->rear->isp = 0; (*this)->rear->icp = 0; break;
	}
	(*this)->size++;
}

void Queue_DeQueue(struct Queue** this)
{
	if (Queue_IsEmpty((*this)) == 0)
	{
		(*this)->front = (*this)->front->next;
		(*this)->size--;
	}
}

//将中缀表达式转换为后缀表达式
void Queue_Postfix(struct Queue** this, struct Node* current)
{
	struct Stack tmpbox = {NULL,0};
	struct Stack* box = &tmpbox;
	Stack_Push(&box, current);
	current = current->next;
	while (!Stack_IsEmpty(box) || current->charnum != '#')
	{
		//是操作数，直接输出至新队列
		if (current->type == 0)
		{
			Queue_EnQueue(this, current);
			current = current->next;
		}
		//是操作符，进行判断
		else if (current->type == 1)
		{
			//进栈
			if (current->icp > box->top->isp)
			{
				Stack_Push(&box, current);
				current = current->next;
			}
			//退栈并输出
			else if (current->icp < box->top->isp)
			{
				Queue_EnQueue(this, box->top);
				Stack_Pop(&box);
			}
			//退栈但不输出
			else if (current->icp == box->top->isp)
			{
				if (box->top->charnum == '(')
				{
					current = current->next;
				}
				Stack_Pop(&box);
			}
		}
	}
}

//表达式计算类
struct Calculator
{
    struct Queue* expre;
	//模拟栈
	struct Stack* box;
};

//清空队列和栈
void Calculator_Clear(struct Calculator** this)
{
	cnt = 0;
    while (Queue_IsEmpty((*this)->expre) == 0)
    {
        Queue_DeQueue(&((*this)->expre));
    }
    while (Stack_IsEmpty((*this)->box) == 0)
    {
        Stack_Pop(&((*this)->box));
    }
}

int Calculator_Get2operands(struct Calculator** this, int* left, int* right)
{
	//取两个操作数
	if (Stack_IsEmpty((*this)->box) == 0)
	{
		*right = (*this)->box->top->intnum;
		Stack_Pop(&((*this)->box));
	}
	else
	{
		return false;
	}
	if (Stack_IsEmpty((*this)->box) == 0)
	{
		*left = (*this)->box->top->intnum;
		Stack_Pop(&((*this)->box));
	}
	else
	{
		return false;
	}
	return true;
};

//计算后缀表达式
int Calculator_Calculate(struct Calculator** this, struct Node** current)
{
	//左右操作数和结果
	int left = 3;
	int right = 3;
	struct Node tmptmp = {0,0,'#',0,0,NULL};
	struct Node* temp = &tmptmp;
	int res = Calculator_Get2operands(this, &left, &right);
	//根据操作符进行运算
	//if (Calculator_Get2operands(this, &left, &right) == true)
	if(res == true)
	{
		switch ((*current)->charnum)
		{
		case'+':temp->intnum = left + right; Stack_Push(&((*this)->box),temp); break;
		case'-':temp->intnum = left - right; Stack_Push(&((*this)->box),temp); break;
		case'*':temp->intnum = left * right; Stack_Push(&((*this)->box),temp); break;
		case'/':if(right==0)
		        {
					printf("================================================================================\n");
		        	printf("#     ERROR:divide by zero is invalid. please input the expression again!      #\n");
					printf("================================================================================\n");
		        	return false;
	        	}
				temp->intnum = left / right; Stack_Push(&((*this)->box), temp); break;
		case'%':temp->intnum = left % right; Stack_Push(&((*this)->box), temp); break;
		case'^':{
					int value = left;
					for (int i = 0; i < right - 1; i++)
					{
						value = value * left;
					}
					temp->intnum = value;
					Stack_Push(&((*this)->box), temp);
					break;
				}
		}
		(*current)=(*current)->next;
		return true;
	}
	else
	{
		printf("================================================================================\n");
        printf("#          ERROR:expression fault. please input the expression again!          #\n");
		printf("================================================================================\n");
		return false;
	}
};

void Calculator_Input(struct Calculator** this, int* fd_stdin)
{
    char buff[128];
	printf("================================================================================\n");
    printf("#          Please input the arithmetic expression you want to compute.         #\n");
	printf("#             Step 1:Input an expression ends with '='.(e.g '1+1=')            #\n");
	printf("#                       Step 2:Press ENTER to continue.                        #\n");
	printf("#            Note:This calculator supports the following operations.           #\n");
	printf("#                        ('+','-','*','/','mod','^')                           #\n");
	printf("================================================================================\n");
	printf("expression:");
    int r = read((*fd_stdin), buff, 128);
	buff[r] = 0;
    char num[128];
    num[0] = '#';
    for(int i = 1;i <= r;i++)
    {
        num[i] = buff[i-1];
    }
	num[r+1] = '#';
	struct Node tmptmp = {0,0,'#',0,0,NULL};
	struct Node* temp = &tmptmp;
	for (unsigned int i = 1; i < r + 1; i++)
	{
		if ((num[i] >= 48 && num[i] < 58) || ((num[i] < 48 && num[i] != '(') && ((num[i - 1] < 48 || num[i - 1] >= 58) && num[i - 1] != ')') && (num[i + 1] >= 48 && num[i + 1] < 58)))
		{
			int tempnum = 0, templen = 0, negative = 1, j = i;
			switch (num[i])
			{
			case'-':negative = 2; j++; break;
			case'+':negative = 3; j++; break;
			}
			while (1)
			{
				if (num[j] < 48 || num[j] >= 58)
				{
					break;
				}
				else
				{
					templen++;
					j++;
				}
			}
			for (int k = (negative == 1) ? i : i + 1; k < j; k++)
			{
				tempnum = tempnum + (num[k] - 48) * pow(10, templen - 1);
				templen--;
			}
			i = j - 1;
			temp->type = 0;
			if (negative == 2)
			{
				negative = -1;
			}
			else if (negative == 3)
			{
				negative = 1;
			}
			temp->intnum = negative * tempnum;
			temp->charnum = '#';
			Queue_EnQueue(&((*this)->expre), temp);
		}
		else if ((num[i] < 48 || num[i] >= 58))
		{
			if (num[i] == '=')
			{
				temp->type = 1;
				temp->intnum = 0;
				temp->charnum = '#';
				Queue_EnQueue(&((*this)->expre), temp);
				break;
			}
			temp->type = 1;
			temp->intnum = 0;
			temp->charnum = num[i];
			Queue_EnQueue(&((*this)->expre), temp);
		}
	}
}


void Calculator_Run(struct Calculator** this, int* fd_stdin)
{
	Calculator_Input(this, fd_stdin);
	//获得后缀表达式
	struct Node fro = {0,0,'#',0,0,NULL};
	struct Queue tmppostexpre = {&fro,&fro,1};
	struct Queue* postexpre = &tmppostexpre;	
	Queue_Postfix(&postexpre, (*this)->expre->front);
    
	struct Node* current = postexpre->front->next;
	while (current != NULL)
	{
		//操作数进栈
		if (current->type == 0)
		{
			Stack_Push(&((*this)->box), current);
			current = current->next;
		}
		//操作符运算
		if (current->type == 1)
		{
			if (current->charnum == '#')
			{
				current = current->next;
				continue;
			}
			if (Calculator_Calculate(this,(&current)) == false)
			{
				return;
			};
		}
	}
	printf("Result:%d",(*this)->box->top->intnum);
    printf("\n\n");
};

//初始化
void Calculator_Initialize(struct Calculator** this, int* fd_stdin)
{
	Calculator_Run(this, fd_stdin);
	Calculator_Clear(this);
	char option = '#'; int iscontinue = 1;
	while (iscontinue)
	{
		printf("Continue(y,n)?");
        char buff[2];
		int r = read(*fd_stdin, buff, 512);
		printf("\n");
		switch (buff[0])
		{
		case'y':clear();Calculator_Run(this, fd_stdin); Calculator_Clear(this); break;
		case'n':iscontinue = 0; break;
		}
	}
}

PUBLIC int calculator_main(int *fd_stdin)
{
	struct Node tmpfro = {0,0,'#',0,0,NULL};
	struct Node* fro = &tmpfro;
	struct Queue tmpexpre = {fro,fro,1};
	struct Queue* expre = &tmpexpre;
	struct Stack tmpbox = {NULL,0};
	struct Stack* box = &tmpbox;
	struct Calculator tmpexpression = {expre,box};
	struct Calculator* expression = &tmpexpression; 
	Calculator_Initialize(&expression, fd_stdin);
	printf("\n");
	return 0;
}
