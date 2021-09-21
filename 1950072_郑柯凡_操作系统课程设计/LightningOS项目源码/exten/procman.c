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

#define NULL ((void*)0)

void processManage()
{
    int i;
	printf("-----------------------------------------------------------------------------\n");
    printf("%9s      |%10s      |%12s    |%9s    |%9s    \n", "PID", "name", "priority", "parPID", "user");
    printf("-----------------------------------------------------------------------------\n");
    for (i = 0; i < NR_TASKS + NR_PROCS; ++i)
    {
        // 读取proc_table数组获取进程信息 
        if (proc_table[i].p_flags != FREE_SLOT)
            printf("%8d        %10s        %8d        %8d          %s\n", 
            proc_table[i].pid, proc_table[i].name, proc_table[i].priority, proc_table[i].p_parent, proc_table[i].user);
    }
    printf("-----------------------------------------------------------------------------\n\n");
}