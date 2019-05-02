//
//  main.c
//  MyShell
//
//  Created by u on 2019/4/19.
//  Copyright © 2019年 u. All rights reserved.
//	注释有test表示该段代码或者变量没有完善或者有问题

#include <stdio.h>
#include "commands.h"

void welcome(void);
void get_command(void);
int deal_command(void);
void login_info(void);
int is_bg(void);//后台程序

void welcome()
{
	if(!getcwd(original_path, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("get path_name error\n");
        exit(-1);
    }
	struct utsname uts;
	if(uname(&uts))
	{
		perror("uname");
		exit(1);	
	}
	printf("\n");
    printf("Welcome to %s\n",uts.version);
    printf("\n");
    printf("*************************************************************\n");
    printf("**Welcome to Tanpan Y's shell                              **\n");
    printf("**Please input \"\033[1mhelp\033[m\" to show what commands can you use    **\n");
    printf("**Input \"\033[1mquit\033[m\" to exit the shell                           **\n");
    printf("*************************************************************\n");
    printf(" _   _      _ _        __        __         _     _ \n");
    printf("| | | | ___| | | ___   \\ \\      / /__  _ __| | __| |\n");
    printf("| |_| |/ _ \\ | |/ _ \\   \\ \\ /\\ / / _ \\| '__| |/ _` |\n");
    printf("|  _  |  __/ | | (_) |   \\ V  V / (_) | |  | | (_| |\n");
    printf("|_| |_|\\___|_|_|\\___/     \\_/\\_/ \\___/|_|  |_|\\__,_|\n");
    printf("\n");
}

void login_info()
{
	struct utsname uts;
	char* login_name;
	char* host_name;
	login_name = getlogin();
	if(uname(&uts))
	{
		perror("uname");
		exit(1);	
	}
	host_name = uts.nodename;
	char path_name[MAX_PATH_NAME];
	if(!getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("get path_name error\n");
        exit(-1);
    }
	int i = 0;
	char Catalog[MAX_PATH_NAME/5];//除以5只是为了分配较小的空间，Catalog为最后一个路径
	memset(Catalog, 0, MAX_PATH_NAME/5);
	i = strlen(path_name);//定位到数组的最后一个字符
	int j = 0;
	if(strcmp(path_name, original_path) != 0)
	{
		while(path_name[i] != '/')
		{
			--i;
		}
		while(path_name[i] != '\0')
		{
			Catalog[j] = path_name[i];
			++j;
			++i;
		}
	}
	//如果源目录是当前目录的子串，则用～代源目录替
	if(strstr(getcwd(path_name, MAX_PATH_NAME), original_path) != NULL)
	{
		printf("\033[1;3;31m%s@%s\033[m:\033[34m~%s\033[m$ ", login_name
		, host_name, Catalog);
	} 
	else
	{
		printf("\033[1;3;31m%s@%s\033[m:\033[34m%s\033[m$ ", login_name
		, host_name, path_name);
	}	
	return;
}

void get_command()
{
    int cnt = 0;
    char str[MAX_LINE];
    char* temp;
	memset(cmdArry, 0, sizeof(cmdArry));
    fgets(str, MAX_LINE, stdin);//fgets保存回车，gets不保存，scanf遇到空格停止
    if(str[strlen(str)-1] == '\n')
    {
        str[strlen(str)-1] = '\0';
    }
    temp = strtok(str, " ");
    while(temp != NULL)
    {
        cmdArry[cnt] = (char*)malloc(sizeof(*temp));
        strcpy(cmdArry[cnt],temp);
        ++cnt;
        temp = strtok(NULL, " ");
    }
    cmdCnt = cnt;
    return;
}

int deal_command()
{
    if(cmdArry[0] == NULL)
    {
        return 0;
    }
    else if(strcmp(cmdArry[0], "pwd") == 0)
    {
        pwd();
        return 1;
    }
    else if(strcmp(cmdArry[0], "echo") == 0)
    {
		for(int i = 1; i < cmdCnt; i++)
		{
			if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
			{
				echo_redirection();
				return 1;
			}
			if(strcmp(cmdArry[i], "|") == 0)//test
			{
				//echo_pipe();
				return 1;
			}
		}
		echo();
		return 1;
	}
	else if(strcmp(cmdArry[0], "ls") == 0)
    {
		ls();
		return 1;
	}
	else if(strcmp(cmdArry[0], "cd") == 0)
    {
		cd();
		return 1;
	}
	else if(strcmp(cmdArry[0], "mkdir") == 0)
    {
		makedir();
		return 1;
	}
	else if(strcmp(cmdArry[0], "rm") == 0)
    {
		deleting();
		return 1;
	}
	else if(strcmp(cmdArry[0], "cat") == 0)
    {
		for(int i = 1; i < cmdCnt; i++)
		{
			if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
			{
				cat_redirection();
				return 1;
			}
		}
		cat();
		return 1;
	}
	else if(strcmp(cmdArry[0], "wc") == 0)
    {
		wc();
		return 1;
	}
	else if(strcmp(cmdArry[0], "clear") == 0)
    {
		clear();
		return 1;
	}
	else if(strcmp(cmdArry[0], "help") == 0)
    {
		help();
		return 1;
	}
	else if(strcmp(cmdArry[0], "tail") == 0)
    {
		if(strcmp(cmdArry[1], "-f") == 0)
		{
			tail_f();
			return 1;
		}
		else
		{
			tail();
			return 1;
		}		
	}
	else if(strcmp(cmdArry[0], "quit") == 0)
	{
		quit();
		return 1;
	}
	else if(strcmp(cmdArry[0], "jobs") == 0)
	{
		//jobs();
		return 1;
	}
	else
	{
		printf("No has this command!\n");
		return 0;
	}
	return 0;
}

int is_bg(void)
{
	int i = 0;
	if(cmdCnt == 0)
		return 0;
	while(i < cmdCnt && cmdArry[i] != NULL)
		++i; 
	i = i - 1; 
	if(strcmp(cmdArry[i], "&") == 0)
	{
		cmdArry[i] = NULL;
		cmdCnt--;
		return 1;
	}
	else
		return 0;
}

void run_bg()//后台进程这里写的不是很好，也许哪一天会修改,bug，后台进程开启时会偶尔影响主进程；
{
	pid_t pid;
	while((pid = fork()) < 0);
	if(pid == 0)
	{
		if(deal_command())
		{
			;
		}
	}
	else
	{
		signal(SIGCHLD,SIG_IGN);				
	}
	return;
}

int main(void)
{
	int bg = 0;
    welcome();
    while(1)
    {	
		if(bg != 1)
		{
			login_info();
		}
		get_command();
		bg = is_bg();
		if(bg == 1)//bg = 1表示该程序为后台程序
		{
			run_bg();//&有时候没有换行输出，有bug，后台程序运行的时候
		}
		else
		{
			if(deal_command())
			{
				;
			}
		}	
	}  
    return 0;
}
