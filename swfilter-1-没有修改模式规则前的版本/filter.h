/*************************************************************************
    > File Name: filter.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2016年06月03日 星期五 15时20分08秒
 ************************************************************************/

#include <pthread.h>
#include "keywordfilter.h"
#ifndef _FILTER_H_
#define _FILTER_H_
#define ERR_NULL_POINTER 1

/*
 * 参数：
 * buffer	需要检测类型的缓冲区
 * length	缓冲区的长度
 * outbuf	输出参数，用于保存返回的mime类型，以‘\0’结尾的字符串，需要调用者				释放空间，当操作失败时值为NULL
 * 返回值：
 * 0	操作成功	-1	操作失败
 * */
//int getMimeType( const char * buffer , int length , char * outbuf ) ;
//int getFileMimeType( const char * path , char * outbuf ) ;


/*
 * 参数：
 * buffer	需要提取文本的缓冲区
 * length	缓冲区的长度
 * outbuf	输出参数，用于保存提取出的文本字符串，以’\0’结尾，需要由调用者释放空间，			操作失败时值为NULL
 * outlen	输出参数，整型指针的空间由调用者分配，用于保存outbuf的字节长度
 * 返回值：
 * 0	操作成功	-1	操作失败
 * */
//int getPlainText( const char * buffer , int length,char *outbuf, int *outlen ) ;
//int getFilePlainText( const char * path ,char *outbuf, int *outlen) ;

/*----------------------关键词匹配----------------------------------------------*/
/*
 * 参数：
 * pwm		用于存放预处理后的结果，会插入到POLICY链表中，所以空间由主进程收到				配置更新信号后释放，改成了二级指针，用于保存结果
 * keywordnum	关键词的个数
 * keyword	关键词列表
 * 返回值：
 * 0	操作成功	-1	操作失败	
 * */
//int preprocess_keyword(struct WuManber **pwm, int keywordnum , unsigned char **keyword);

/*
 * 参数：
 * pv	因为这是一个线程运行的函数，所以这个参数是由下方的KEYWORDFILTER 结构体转换而成的指针
 * 返回值：
 * 空指针
 * */
//void * KeyWordFilter( void * pv ) ;
/*-------------------------------------------------------------------------------*/


/*--------------------模式匹配函数------------------------------------------------*/
/*
 * 参数：
 * pv	因为这是一个线程运行的函数，所以这个参数是由下方的PATTERNMATCH结构体转换而成的指针
 * 返回值：
 * 空指针
 * */
//void * PatternMatch( void * pv ) ;
/*---------------------------------------------------------------------------------*/

/*
 * 参数：
 * ppolicy	输出参数，保存预处理后的结果，操作失败时为NULL，空间由主进程收到配置更			新信号后释放
 * policyname	策略对象的名称，空间由主进程收到配置更新信号后释放
 * filetypenum	文件类型的个数
 * filetype	文件类型字符串，会插入到policy结构体中，所以空间由主进程收到配置更新信号			后释放
 * keynum	关键词个数
 * keylist	关键词列表，会插到policy结构体中，所以空间由主进程收到配置更新信号后释放
 * patternnum	模式个数
 * patternname	模式的名称，用于在审计界面上显示命中了哪一个模式
 * pattern	模式列表，会插入到policy结构体中，所以空间由主进程收到配置更新信号后释放
 * 返回值：
 * O 操作成功 -1 操作失败,如果操作失败，上述空间都由调用者释放
 * */
//int preprocess(POLICY ** ppolicy ,char * policyname, int filetypenum, char ** filetype ,int keynum,char ** keylist,int patternnum, char **patternname,char ** pattern);


extern int preprocess(POLICY ** ppolicy ,char * policyname , int filetypenum, char ** filetype ,int keynum,char ** keylist,int patternnum,char ** patternname, char ** pattern);
POLICY *GetPolicy(char *policyname);
int exit_pthread(pthread_t pid);
POLICY * search_policy_in_policylist(char *policyname);
void free_policy_list();
POLICY *add_policy_into_policylist(POLICY *p, int *ret_code);
int query_policy_info(MYSQL *sql, char *objname, int *key_num, char***keywords, int *pattern_num,char *** patternname , char ***pattern, int *filetype_num, char ***filetype_list);
#endif
