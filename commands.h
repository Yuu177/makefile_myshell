#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>//dirent ，为了获取某个文件夹的内容，所使用的结构体；
#include <sys/stat.h>//mkdir需要
#include <sys/utsname.h>
#include <fcntl.h>//open
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 100
#define MAX_PATH_NAME 100
#define MAX_DIR_NAME 100
#define MAX_JOBS 100

char* cmdArry[MAX_LINE];
int cmdCnt;//命令数从0开始
char original_path[MAX_PATH_NAME];//源目录

void pwd(void);
void echo(void);
void ls(void);//ls or ls 参数
void cd(void);
void makedir(void);
void deleting(void);
void cat(void);
void wc(void);
void clear(void);
void help(void);
void cat_redirection(void);
void echo_redirection(void);
void tail(void);//tail命令用途是依照要求将指定的文件的最后部分输出到标准设备
void tail_f(void);//tail -f用来实现后台程序
void quit(void);
