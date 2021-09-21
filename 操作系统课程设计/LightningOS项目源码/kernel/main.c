
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        Forrest Yu, 2005
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
#include "keyboard.h"


#define NULL ((void*)0)
#define ever ;;


/*****************************************************************************
 *                               Referenced Functions List
 *****************************************************************************/
PUBLIC void anim();
PUBLIC void commandList();
PUBLIC void noSuchCom();
PUBLIC void gameList();
PUBLIC void processManage();
PUBLIC void introduction();


char sys_user[16];
int boundary;
/*****************************************************************************
 *                               kernel_main
 *****************************************************************************/
/**
 * jmp from kernel.asm::_start. 
 * 
 *****************************************************************************/
PUBLIC int kernel_main()
{
	strcpy(sys_user,"root");
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
		 "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	int i, j, eflags, prio;
    u8 rpl;
    u8  priv;/* privilege */

	struct task * t;
	struct proc * p = proc_table;

	char * stk = task_stack + STACK_SIZE_TOTAL;

	for (i = 0; i < NR_TASKS + NR_PROCS; i++,p++,t++) 
	{
		p->pid=i;
		if (i >= NR_TASKS + NR_NATIVE_PROCS)
		{
			p->p_flags = FREE_SLOT;
			continue;
		}

		if (i < NR_TASKS) 
		{     /* TASK */
			t	= task_table + i;
			priv	= PRIVILEGE_TASK;
			rpl     = RPL_TASK;
			eflags  = 0x1202;/* IF=1, IOPL=1, bit 2 is always 1 */
			prio    = 15;
		}
		else 
		{   /* USER PROC */
			t	= user_proc_table + (i - NR_TASKS);
			priv	= PRIVILEGE_USER;
			rpl     = RPL_USER;
			eflags  = 0x202;	/* IF=1, bit 2 is always 1 */
			prio    = 5;
		}

		strcpy(p->name, t->name);	/* name of the process */
		p->p_parent = NO_TASK;

		if (strcmp(t->name, "TestA") != 0) 
		{
			p->ldts[INDEX_LDT_C]  = gdt[SELECTOR_KERNEL_CS >> 3];
			p->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];

			/* change the DPLs */
			p->ldts[INDEX_LDT_C].attr1  = DA_C   | priv << 5;
			p->ldts[INDEX_LDT_RW].attr1 = DA_DRW | priv << 5;
		}
		else 
		{		/* INIT process */
			unsigned int k_base;
			unsigned int k_limit;
			int ret = get_kernel_map(&k_base, &k_limit);
			assert(ret == 0);
			init_desc(&p->ldts[INDEX_LDT_C],
				  0, /* bytes before the entry point
				      * are useless (wasted) for the
				      * INIT process, doesn't matter
				      */
				  (k_base + k_limit) >> LIMIT_4K_SHIFT,
				  DA_32 | DA_LIMIT_4K | DA_C | priv << 5);

			init_desc(&p->ldts[INDEX_LDT_RW],
				  0, /* bytes before the entry point
				      * are useless (wasted) for the
				      * INIT process, doesn't matter
				      */
				  (k_base + k_limit) >> LIMIT_4K_SHIFT,
				  DA_32 | DA_LIMIT_4K | DA_DRW | priv << 5);
		}

		p->regs.cs = INDEX_LDT_C << 3 |	SA_TIL | rpl;
		p->regs.ds =
			p->regs.es =
			p->regs.fs =
			p->regs.ss = INDEX_LDT_RW << 3 | SA_TIL | rpl;
		p->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p->regs.eip	= (u32)t->initial_eip;
		p->regs.esp	= (u32)stk;
		p->regs.eflags	= eflags;

		p->ticks = p->priority = prio;
		p->running_prio = p->priority = prio;
		strcpy(p->user,sys_user);/* user of the process */
		p->cut_num = 0;

		p->p_flags = 0;
		p->p_msg = 0;
		p->p_recvfrom = NO_TASK;
		p->p_sendto = NO_TASK;
		p->has_int_msg = 0;
		p->q_sending = 0;
		p->next_sending = 0;

		for (j = 0; j < NR_FILES; j++)
			p->filp[j] = 0;

		stk -= t->stacksize;
	}

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

	init_clock();
    init_keyboard();

	restart();

	while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}


PUBLIC void clear()
{
	clear_screen(0, console_table[current_console].cursor);
	console_table[current_console].crtc_start = 0;
	console_table[current_console].cursor = 0;
}

/*======================================================================*
                               TestA
 *======================================================================*
							                            kefan.Z  2021
 *======================================================================*/
void TestA()
{
	//文件部分变量
	char curdir[512] = "/";
	char rdbuff[256];

	//char command_3[100], command_4[100], command_5[100];
	int fd_stdin  = open("/dev_tty0", O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open("/dev_tty0", O_RDWR);
	assert(fd_stdout == 1);
	anim();
	commandList();

	while (1) {
		printf("[%s@LightningOS %s]# ",sys_user, curdir);
		boundary = tty_table[0].console->cursor;
		int r = read(fd_stdin, rdbuff, 256);
		rdbuff[r] = 0;
		if(parsecmd(rdbuff, curdir)==0)
		{
			if (!strcmp(rdbuff, "help"))
			{
				clear();
				commandList();
			}
			else if (!strcmp(rdbuff, "clear"))
			{
				clear();
			}
			else if (!strcmp(rdbuff, "game"))
			{
				gameList();
			}
			else if (!strcmp(rdbuff, "calculator"))
			{
				clear();
				calculator_main(&fd_stdin);
				commandList();
				continue;
			}
			else if (!strcmp(rdbuff, "ps")) 
			{
				processManage();
			}
			else if (!strcmp(rdbuff, "fs --help")) 
			{
				filecmdList();
			}
			else if (!strcmp(rdbuff,"draughts"))
			{
				draughts_main(&fd_stdin);
				clear();
			}
			else if (!strcmp(rdbuff,"2048"))
			{
				g2048_main(&fd_stdin);
				clear();
			}
			else if (!strcmp(rdbuff,"snake"))
			{
				snake_main(&fd_stdin);
				clear();
			}
			else if (!strcmp(rdbuff,"transnum"))
			{
				numsys_main(&fd_stdin);
			}
			else if (!strcmp(rdbuff,"gomoku"))
			{
				gomoku_main(&fd_stdin);
				clear();
			}
			else   //找不到所输入的指令
			{
				noSuchCom();
			}
		}
	}	
	for(ever);
}


/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int fd_stdin  = open("/dev_tty1", O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open("/dev_tty1", O_RDWR);
	assert(fd_stdout == 1);
	introduction();
	for(ever);
}

/*======================================================================*
                               TestC
 *======================================================================*/
void TestC()
{     
	//char command_3[100], command_4[100], command_5[100];
	int fd_stdin  = open("/dev_tty2", O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open("/dev_tty2", O_RDWR);
	assert(fd_stdout == 1);
	printf("hello world!\n");
	for(ever){}
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}



