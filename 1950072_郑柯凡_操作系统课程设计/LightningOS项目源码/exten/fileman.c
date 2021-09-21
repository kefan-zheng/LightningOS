
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

int parsecmd(char* rdbuf, char* curdir)
{
	char cmd[20];
    char filename1[128];
	char filename2[128];
    // 解析命令
    int pos = 0;
    while (rdbuf[pos] != ' ' && rdbuf[pos] != 0)  // 读取指令
    {
        cmd[pos] = rdbuf[pos];
        pos++;
    }
    cmd[pos] = 0;
    if (rdbuf[pos] != 0)  // 指令还未结束
    {
        pos++;
        int len = pos;
        while (rdbuf[pos] != ' ' && rdbuf[pos] != 0)  // 读取第一个参数（文件名或指令属性）
        {
            filename1[pos - len] = rdbuf[pos];
            pos++;
        }
        filename1[pos - len] = 0;
    }
    else      // 不存在第一个参数，将其置空
    {
        filename1[0] = 0;
    }
    if (rdbuf[pos] != 0)  // 指令还未结束
    {
        pos++;
        int len = pos;
        while (rdbuf[pos] != ' ' && rdbuf[pos] != 0)  // 读取第二个参数（操作由参时的文件名）
        {
            filename2[pos - len] = rdbuf[pos];
            pos++;
        }
        filename2[pos - len] = 0;
    }
    else     // 不存在第二个参数，将其置空
    {
        filename2[0] = 0;
    }

    //比对用户输入内容执行相应命令
    // ls 展示当下目录所有文件与目录
    // cd 进入目录
    // mkdir 创建目录
    // touch 创建普通文件
    // wt 写文件
    // rd 读文件
    // rm 删除文件或目录
    if (strcmp(cmd, "ls") == 0)
    {
        myls(curdir);
        return 1;
    }
    else if (strcmp(cmd, "cd") == 0)
    {
        mycd(curdir, filename1);
        return 1;
    }
    else if (strcmp(cmd, "mkdir") == 0)
    {
        mymkdir(curdir, filename1);
        return 1;
    }
    else if (strcmp(cmd, "touch") == 0)
    {
        mytouch(curdir, filename1);
        return 1;
    }
    else if (strcmp(cmd, "rm") == 0)
    {
        //0 means regular file, 1 means dir file
        if (strcmp(filename1, "-r") == 0) 
        {
            myrm(curdir, filename2, 1);
        }
        else {
            myrm(curdir, filename1, 0);
        }
        return 1;
    }
    else if (strcmp(cmd, "wt") == 0)
    {
        mywt(curdir, filename1);
        return 1;
    }
    else if (strcmp(cmd, "rd") == 0)         
    {
        myrd(curdir, filename1);
        return 1;
    }
	else if(strcmp(cmd, "mv") == 0)
	{
		mymv(curdir, filename1, filename2);
		return 1;
	}
    else
    {
        return 0;
    }
}


void myls(char * cur_dir)
{	
	printf("----------------------------------\n");
	printf("|%12s   |%12s    |\n","name","type");
	printf("|---------------|----------------|\n");
	if (ls(cur_dir) < 0) printf("ls error"); 
	printf("----------------------------------\n\n");
}

void mycd(char * cur_dir, char * dir_name)
{
	//处理特殊目录
	if (dir_name[0] <= 0)
	{
		printf("cd:: error: command cd need one parameter.\n");
		return;
	}
	if (dir_name[0] == ' ')
	{
		printf("cd:: error: directory cannot start with space.\n");
		return;
	}
	//如果参数是"..",仅修改路径名
	if (strcmp(dir_name,"..")==0)
	{
		int i = 0;
		for (; ; i++)
		{
			if (cur_dir[i] == '/' && cur_dir[i + 1] == 0)
			{
				break;
			}
		}
		if (i > 0)
		{
			cur_dir[i--] = 0;
			for (; ; i--)
			{
				if (cur_dir[i] == '/') break;
				cur_dir[i] = 0;
			}
		}
		return;
	}
	//不是cd ..
	//获取打开的文件路径，将文件名连接在路径之后
	char path[32] = "$";
	int i = 0;
	for (; ; i++)
	{
		if (cur_dir[i] == 0) break;
		path[i + 1] = cur_dir[i];
	}
	i++;
	for (int j = 0; ; j++)
	{
		if (dir_name[j] == 0) break;
		path[i++] = dir_name[j];
	}
	path[i] = 0;
	int fd = open(path, O_RDWR);
	if (fd == -1)
	{
		printf("No such directory.\n");
		return;
	}
	close(fd);   
	strcat(cur_dir, dir_name);
	strcat(cur_dir, "/");
}

//新建文件夹
void mymkdir(char * cur_dir, char * dir_name)
{
	if (dir_name[0] <= 0)
	{
		printf("mkdir:: error: command mkdir need one parameter.\n");
		return;
	}
	if (dir_name[0] == ' ')
	{
		printf("mkdir:: error: directory cannot start with space.\n");
		return;
	}
	char path[32] = "$";  //文件目录标识符
	int i = 0;
	for (; ; i++)
	{
		if (cur_dir[i] == 0) break;
		path[i + 1] = cur_dir[i];
	}
	i++;
	for (int j = 0; ; j++)
	{
		if (dir_name[j] == 0) break;
		path[i++] = dir_name[j];
	}
	path[i] = 0;
	int fd = open(path, O_CREAT);
	if (fd != -1)
	{
		close(fd);
	}
	else
	{
		printf("mkdir error: directory exists.\n");
	}
}

//新建文件
void mytouch(char * cur_dir, char * filename)
{
	if (filename[0] <= 0)
	{
		printf("touch:: error: command touch need one parameter.\n");
		return;
	}
	if (filename[0] == ' ')
	{
		printf("touch:: error: file name cannot start with space.\n");
		return;
	}
	char path[32] = "#";   //普通目录标识符
	int i = 0;
	for (; ; i++)
	{
		if (cur_dir[i] == 0) break;
		path[i + 1] = cur_dir[i];
	}
	i++;
	for (int j = 0; ; j++)
	{
		if (filename[j] == 0) break;
		path[i++] = filename[j];
	}
	path[i] = 0;
	int fd = open(path, O_CREAT);
	if (fd != -1)
	{
		close(fd);
	}
	else
	{
		printf("touch error: file exists.\n");
	}

}

//删除文件
void myrm(char * cur_dir, char * filename, int flag) 
{
	char path[32] = "#";
	
	if (flag == 1) 
	{
		path[0] = '$';
	}
	int i = 0;
	for (; ; i++)
	{
		if (cur_dir[i] == 0) break;
		path[i + 1] = cur_dir[i];
	}
	i++;
	for (int j = 0; ; j++)
	{
		if (filename[j] == 0) break;
		path[i++] = filename[j];
	}
	path[i] = 0;
	int fd = unlink(path);
	if (fd == 0)
	{
		printf("Remove success: %s removed!\n", filename);
	}
	else if (fd == -2)
	{
		printf("Remove fail: cannot find %s!\n", filename);
	}
	else if (fd != -3)
	{
		printf("Failed to remove %s!\n", filename);
	}
}

//重命名（暂未成功）
void mymv(char* cur_dir, char* filename1, char* filename2)
{	
	char path[128];
	strcpy(path,cur_dir);
	//合成一个字符串
	strcat(path,"@");
	strcat(path,filename1);
	strcat(path,"/@");
	strcat(path,filename2);
	strcat(path,"/");
	int res = myrename(path);
	if(res>0)
	{
		printf("rename success\n");
	}
	else
	{
		printf("no such file.\n");
	}
	printf("%d\n",res);
}

//写文件
void mywt(char * cur_dir, char * filename) 
{
	char path[32] = "#";
	int i = 0;
	for (; ; i++)
	{
		if (cur_dir[i] == 0) break;
		path[i + 1] = cur_dir[i];
	}
	i++;
	for (int j = 0; ; j++)
	{
		if (filename[j] == 0) break;
		path[i++] = filename[j];
	}
	path[i] = 0;
	int fd = open (path, O_RDWR);
	if (fd == -1)
	{
		printf("Failed to open %s!\n", filename);
		return;
	}
	char tty_name[] = "/dev_tty0";
	int fd_stdin  = open(tty_name, O_RDWR);
	if (fd_stdin == -1)
	{
		printf("Failed to write file!\n");
		return;
	}
	char writeBuf[4096];  // 写缓冲区
	printf("Enter the content:\n");
	int final = read(fd_stdin, writeBuf, 4096);
	writeBuf[final] = 0;
	write(fd, writeBuf, final + 1);
	printf("\nTotally write %d bytes in the file : %s\n",final,writeBuf);  
	close(fd);
}

//读文件
void myrd(char*cur_dir, char * filename)
{
	char path[32] = "#";
	int i = 0;
	for (; ; i++)
	{
		if (cur_dir[i] == 0) break;
		path[i + 1] = cur_dir[i];
	}
	i++;
	for (int j = 0; ; j++)
	{
		if (filename[j] == 0) break;
		path[i++] = filename[j];
	}
	path[i] = 0;
	int fd = open (path, O_RDWR);
	if(fd==-1){
		printf("Failed to open %s!\n", filename);
        return;
	}
	char buf[4096];
    int rdFlag = read(fd, buf, 4096);
    if (rdFlag == -1)  // 读取文件内容失败
    {
        printf("Failed to read file!\n");
        close(fd);
        return;
    }

    printf("Content of file <%s> :\n", filename);
	printf("----------------------------------\n");

	int count = 0;
	char tem[33];
	char* content = buf;

	while(content[count] != NULL)
	{
		assert(tem != NULL && content != NULL);

		char* temp = tem;
		int i = 0;
		while (i++ < 32 && (*temp++ = *content++) != '\0');

		if (*(temp) != '\0') 
		{
			*temp = '\0';
		}
		printf("%s\n", tem);
		content += 32;
		count += 32;
	}

	printf("%s\n",content);
	printf("----------------------------------\n\n");
    close(fd);
}

