#include <stdio.h>
#include "commands.h"

void pwd()
{
    char path_name[MAX_PATH_NAME];
    if(getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("%s\n",path_name);
    }
    else
    {
        printf("error!\n");
    }
}

void echo()
{
	for(int i = 1 ; i < cmdCnt; i++)
	{
		if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
		{
			break;
		}
		if(strcmp(cmdArry[i], "|") == 0)//test
		{
			break;
		}
		printf("%s ",cmdArry[i]);
	}
	printf("\n");
	return;
}

//~ struct dirent
//~ {
//~ long d_ino; /* inode number 索引节点号 
//~ off_t d_off; /* offset to this dirent 在目录文件中的偏移 
//~ unsigned short d_reclen; /* length of this d_name 文件名长 
//~ unsigned char d_type; /* the type of d_name 文件类型 
//~ char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长256字符
//~ }
void ls()
{
	char path_name[MAX_PATH_NAME];
	char dir_name[MAX_DIR_NAME];
    if(!getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("getcwd error!\n");
    }
    DIR* dir;
    struct dirent* pDIR;
    struct stat dir_stat;
    if(cmdArry[1] != NULL)//带参数
    {
		if((dir = opendir(cmdArry[1])) == NULL)
		{
			printf("No such file or directory!\n");
			return;
		}
	}
	else
	{
		dir = opendir(path_name);
	}		 
    int i = 0;//用来标记每一行能输出的个数
    int j = 0;//决定是否换行
    int n = 5;//n表示一行输出多少个
    while((pDIR = readdir(dir)) != NULL)
    {
		strcpy(dir_name, pDIR->d_name);
		if(dir_name[0] == '.')
		{
			continue;
		}
		else
		{
			stat(dir_name, &dir_stat);
			if(S_ISREG(dir_stat.st_mode))//是否为文件
			{
				if(access(pDIR->d_name, X_OK) != -1)//判断文件是否可执行
				{
					printf("\033[1;32m%s\033[m\t",pDIR->d_name);
				}
				else
					printf("%s\t",pDIR->d_name);
			}
			else //if(S_ISDIR(dir_stat.st_mode))//S_ISDIR(st_mode)是否为目录
			{
				printf("\033[1;34m%s\033[m\t",pDIR->d_name);
			}	
			++i;
			++j;
			if(i == n)//每行输出5个
			{
				printf("\n");
				i = 0;//用来标记每一行能输出的个数
			}
		}		
	}
	if(j % n != 0)//如果有余数，说明没有换行
		printf("\n");
	j = 0;
    return;
}

void cd()
{
	char path_name[MAX_PATH_NAME];
	char tmp[1] = "/";
	if(cmdArry[1] == NULL)
	{
		chdir(original_path);
		return;
	}
	if(chdir(cmdArry[1]) != -1)//先直接打开文件目录
	{
		return;
	}
	if(!getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("cd error\n");
        exit(-1);
    } 
    if(strcmp(cmdArry[1], "..") == 0)
	{
		int i = 0;
		while(path_name[i] != '\0')
		{
			++i;
		}
		while(path_name[i] != '/')
		{
			path_name[i] = '\0';
			--i;
		}
	}
	else if(strcmp(cmdArry[1], ".") == 0)
	{
		printf("\n");
	}
	else if(cmdArry[1] != NULL)
	{
		strncat(path_name, tmp, sizeof(tmp));
		strncat(path_name, cmdArry[1], 1000);
	} 
    if(chdir(path_name) == -1)
    {
		printf("cd no dir\n");
	}
    return;
}

//~ access to作为有权使用什么，即可理解access（）函数想表达有做某事的权限。
//~ 函数参数有两个，第一个为文件，那么对应的第二个参数就不难推想出为文件有那些权限和是否存在。
void makedir()
{
	if(access(cmdArry[1], 0) != 0)
	{
		if(mkdir(cmdArry[1], 0777) == -1)
		{
			printf("mkdir error\n");
			return;
		}
	}
	else
	{
		printf("the dir had exist!\n");
	}
	return;
}

//~ struct stat结构体简介
//~ 在使用这个结构体和方法时，需要引入：
//~ <sys/types.h>
//~ <sys/stat.h>
//~ struct stat这个结构体是用来描述一个linux系统文件系统中的文件属性的结构。
//~ 可以有两种方法来获取一个文件的属性：
//~ 通过路径：
//~ int stat(const char *path, struct stat *struct_stat);
//~ int lstat(const char *path,struct stat *struct_stat);
//~ 两个函数的第一个参数都是文件的路径，第二个参数是struct stat的指针。返回值为0，表示成功执行。
//~ Linux下系统api int rmdir(const char *pathname);可以删除目录。但使用rmdir函数时，
//~ 目录必须为空，否则调用失败，函数返回-1，执行成功时，函数返回0。
//~ 若要删除一个目录，必须将给目录下的文件全部删除，系统调用int remove(const char * pathname)可以删除一个普通文件；
void deleting()
{
	struct stat dir_stat;
	if(access(cmdArry[1], 0) != 0)
	{
		printf("No such file or directory!\n");
		return;
	}
	stat(cmdArry[1], &dir_stat);
	if(S_ISREG(dir_stat.st_mode))
	{
		if(remove(cmdArry[1]) != 0)
		{
			printf("delete file false!\n");
			return;
		}
	}
	else
	{
		if(rmdir(cmdArry[1]) != 0)
		{
			printf("delete dir false!\n");
			return;
		}
	}
	return;
}

void cat()
{
	if(access(cmdArry[1], 0) != 0)
	{
		printf("No such file or directory!\n");
		return;
	}
	char buf[1024];
	FILE* fp;
	int len;
	if((fp = fopen(cmdArry[1], "r")) == NULL)
	{
		printf("open false!\n");
		return;
	}
	while(fgets(buf, 1024, fp) != NULL)
	{
		len = strlen(buf);
		buf[len - 1] = '\0';//去掉换行符
		printf("%s\n",buf);
	}
	fclose(fp);
	return;
}

void wc()
{
	if(access(cmdArry[1], 0) != 0)
	{
		printf("No such file or directory!\n");
		return;
	}
	char buf[1024];
	FILE* fp;
	int len;
	int length = 0;//字节数
	int RowNumber = 0;//行数
    int count = 0;//单词数
	if((fp = fopen(cmdArry[1], "r")) == NULL)
	{
		printf("open false!\n");
		return;
	}
	while(fgets(buf, 1024, fp) != NULL)
	{
		len = strlen(buf);
		length += len;
		++RowNumber;
		buf[len - 1] = '\0';//去掉换行符
        char* tmp = strtok(buf, " ");
        while(tmp != NULL)
        {
            ++count;
            tmp = strtok(NULL, " ");
    	}
    }
	printf("%d\t%d\t%d %s", RowNumber, count, length, cmdArry[1]);
	printf("\n");
	fclose(fp);
	return;
}

void clear()
{
	printf("\033[2J");
	printf("\033[H");
	return;
}

void help()
{
	printf("************************************************\n");
	printf("welcome to the manual of myshell\n");
	printf("the following commands supported by myshell\n");
	printf("\n");
	printf("\033[1;33mNAMES\033[m      \033[1;33mFORMATS\033[m                         \033[1;33mDESCRIPTIONS\033[m\n");
	printf("\033[1;32m&\033[m:         [job_spec] &                    Commands in background\n");
	printf("\033[1;32mpwd\033[m:       pwd                             Print the current working directory\n");
	printf("\033[1;32mecho\033[m:      echo ...                        Print strings after echo\n");
	printf("           echo ... >(>>) [FILE]           Redirection is supported\n");
	printf("\033[1;32mls\033[m:        ls [DIR]                        List the file names in the target directory\n");
	printf("\033[1;32mcd\033[m:        cd [DIR]		           Go to a specified directory\n");
	printf("\033[1;32mmkdir\033[m:     mkdir [DIR]                     Create a file directory\n");
	printf("\033[1;32mrm\033[m:        rm [FILE]\\[DIR]                 Delete a file\\directory\n");
	printf("\033[1;32mcat\033[m:       cat [FILE]                      Print text Output\n"); 
	printf("           cat [FILE] >(>>) [FILE]         Redirection is supported\n");
	printf("\033[1;32mwc\033[m:        wc [FILE]                       Statistics the number of bytes, words and rows in the specified file, and display\n");
	printf("\033[1;32mclear\033[m:     clear                           Clear the screen\n");
	printf("\033[1;32mhelp\033[m:      help                            Show the manual of help/get help info of a sepcified command\n");
	printf("\033[1;32mtail\033[m:      tail [OPTION]... [FILE]...      Print  the  last  10  lines of FILE\n");
	printf("           -f                              Output appended data as the file grows\n");
	printf("\033[1;32mquit\033[m:      quit                            Quit the shell\n");
	printf("\033[1;32mjobs\033[m:      jobs                            Check the processes running in the system\n");
	printf("************************************************\n");
	fflush(stdout);
}

void cat_redirection()
{
	pid_t pid;
	char dest_filename[MAX_PATH_NAME];
	char source_filename[MAX_PATH_NAME];
	for(int i = 1; i < cmdCnt; i++)
	{
		if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
		{
			if(cmdArry[i+1] == NULL)
			{
				printf("> or >> no file\n");
			}
			else
			{
				strcpy(dest_filename, cmdArry[i+1]);
				strcpy(source_filename, cmdArry[i-1]);
			}
			if(strcmp(cmdArry[i], ">") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
				//O_WRONLY | O_TRUNC | O_CREAT，写文件，从头开始写，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					cmdArry[1] = source_filename;
					cat();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
			if(strcmp(cmdArry[i], ">>") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
				//O_WRONLY | O_APPEND | O_CREAT，写文件，追加，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					cmdArry[1] = source_filename;
					cat();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
		}
	}
	return;
}

void wc_redirection()
{
}

void echo_redirection()
{
	pid_t pid;
	char dest_filename[MAX_PATH_NAME];
	for(int i = 1; i < cmdCnt; i++)
	{
		if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
		{
			if(cmdArry[i+1] == NULL)
			{
				printf("> or >> no file\n");
			}
			else
			{
				strcpy(dest_filename, cmdArry[i+1]);
			}
			if(strcmp(cmdArry[i], ">") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
				//O_WRONLY | O_TRUNC | O_CREAT，写文件，从头开始写，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					echo();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
			if(strcmp(cmdArry[i], ">>") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
				//O_WRONLY | O_APPEND | O_CREAT，写文件，追加，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					echo();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
		}
	}
	return;
}


void tail()
{
	char buf[1024];
	FILE* fp;
	if((fp = fopen(cmdArry[1], "r")) == NULL)
	{
		printf("No such file or directory!\n");
		return;
	}
	fseek(fp, -1024, SEEK_END);//定位到文件文件结尾前1024个字节
	while(!feof(fp))//feof文件结束：返回非0值，文件未结束，返回0值
	{
		memset(buf, 0, sizeof(buf));
		fgets(buf, 1024, fp);
		printf("%s",buf);
	}
	fclose(fp);
	return;
}

void tail_f()//tail -f
{
	char buf[1024];
	FILE* fp;
	if((fp = fopen(cmdArry[2], "r")) == NULL)
	{
		printf("No such file or directory!\n");
		return;
	}
	fseek(fp, -1024, SEEK_END);//定位到文件文件结尾前1024个字节
	while(1)//1,后台tail -f 一直读取
	{
		memset(buf, 0, sizeof(buf));
		fgets(buf, 1024, fp);
		printf("%s",buf);
	}
	fclose(fp);
	return;
}

void quit()
{
	printf("***************************\n");
	printf("**Thank you for your use!**\n");
	printf("***************************\n");
	exit(-1);
}
