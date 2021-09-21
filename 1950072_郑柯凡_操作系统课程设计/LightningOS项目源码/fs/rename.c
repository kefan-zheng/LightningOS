/*************************************************************************/ /**
 *****************************************************************************
 * @file   rename.c
 * @brief  
 * @author original: kefan.Z
 * @date   2021.08.25
 *****************************************************************************
 *****************************************************************************/

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

PUBLIC int myrename(char* pathname)
{
	MESSAGE msg;

	msg.type	= MV;

	msg.PATHNAME	= (void*)pathname;
	msg.NAME_LEN	= strlen(pathname);

	send_recv(BOTH, TASK_FS, &msg);
	assert(msg.type == SYSCALL_RET);
	printl("debug3\n");
	return msg.RETVAL;
}

PUBLIC int do_mv()
{
	int file_cnt=0;
    char pathname[MAX_PATH];
    char old_name[32];
    char new_name[32];
	int name_num = 0;
    int old_or_new = 0;//1为旧名字，2为新名字

	// get parameters from the message
	int name_len = fs_msg.NAME_LEN;	// length of filename 
	int src = fs_msg.source;	// caller proc nr. 
	assert(name_len < MAX_PATH);
	phys_copy((void*)va2la(TASK_FS, pathname),
		  (void*)va2la(src, fs_msg.PATHNAME),
		  name_len);
	pathname[name_len] = 0;
	
    for(int len=0;len<name_len;len++) // check each character
	{	
		if(pathname[len] == '/')
		{
			continue;
		}	
		if(pathname[len] == 0)
		{
			break;
		}
		else if (pathname[len] == '@')    //获得一级文件目录，此时名称存储在filename中
		{
			if(old_or_new == 0)
            {
                old_or_new = 1;
				name_num = 0;
            }
            else if(old_or_new == 1)
            {
                old_or_new = 2;
				name_num = 0;
            }
		}
        else
		{
			if(old_or_new == 1)
			{
				old_name[name_num] = pathname[len];
				name_num++;
			}
			else if(old_or_new == 2)
			{
				new_name[name_num] = pathname[len];
				name_num++;
			}
		}
	}

    for(int len=0;len<name_len;len++)
    {
        if(pathname[len] == '@')
        {
			old_or_new = -1;
        }
		if(old_or_new == -1)
		{
			pathname[len] = 0;
		}
    }

	char * filename;
	struct inode * dir_inode;
	strip_path(filename, pathname, &dir_inode);
    //遍历改名字
    int dir_blk0_nr = dir_inode->i_start_sect;  
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	int nr_dir_entries =    
	  dir_inode->i_size / DIR_ENTRY_SIZE; //
					       //* including unused slots
					       //* (the file has been deleted
					       //* but the slot is still there)
					       //
	int i, j, m = 0;
	struct dir_entry * pde;
	int dev = dir_inode->i_dev;
	for (i = 0; i < nr_dir_blks; i++) 
	{
		RD_SECT(dev, dir_blk0_nr + i);
		pde = (struct dir_entry *)fsbuf;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++, pde++) 
		{
			
			//check every directory entry
			if (strcmp(old_name, pde->name) == 0)
			{
				strcpy(pde->name,new_name);
				file_cnt++;
			}	
			RD_SECT(dev, dir_blk0_nr + i);
			if (++m > nr_dir_entries)
				break;
		}
		if (m > nr_dir_entries) // all entries have been iterated 
			break;
	}
	return file_cnt;  //system file
}

