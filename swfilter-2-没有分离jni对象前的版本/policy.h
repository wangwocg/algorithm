/*************************************************************************
    > File Name: filter.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2016年06月03日 星期五 15时20分08秒
 ************************************************************************/
#ifndef _FILTER_H_
#define _FILTER_H_

#include <pthread.h>
#include "filter.h"

#define ERR_NULL_POINTER 1

#define RES_TYPE_KEYWORD	1
#define RES_TYPE_PATTERN	2
#define RES_TYPE_FILETYPE	3


/*
struct pattern_info{
	char name[128];
	char pattern[PATTERN_RULE_LEN];
	int exclude; // 0，不包含排除信息，1：排除模式，2：排除短语
	char *exinfo;
	int exwords_num;
	char *exwords;
};

struct prepattern {
	regex_t  preg ;//
	void * exinfo ;//如果
};
*/

typedef struct _str_policy{
	struct list_head list ;
	char * policyname ;		//策略名称
	int filetypenum ;	//文件类型的个数
	char * filetype ;	//文件类型
	struct WuManber * wm ;		//关键词预处理后的结果
	int patternnum ;	//模式个数
	struct pattern_info * pattinfo ;
	struct prepattern * prepatt ;//数组,大小为patternnum个
	char attrs[8];	       //策略属性，最多支持8个，attrs[0]:关键词匹配数量阈值;attrs[1]:匹配规则，计数唯一匹配还是计数所有匹配
}POLICY ;

typedef struct _str_MPOLICY{
	struct list_head list ;
	POLICY *policy;//空间不能由调用关键词过滤线程的父线程释放，这是POLICY链表的一部分，由主进程收到配置更新信号后释放
}MPOLICY ;


POLICY * search_policy_in_policylist(char *policyname);
POLICY *add_policy_into_policylist(POLICY *p, int *ret_code);
void free_policy_list();
POLICY *GetPolicy(char *policyname,int count);
struct list_head  * getMPOLICY( char * policynames );
void freePOLICY( POLICY * policy );
int updatePOLICY();

int query_policy_info(MYSQL *sql, char *objname,char *attrs, int *key_num, char**keywords, int *pattern_num, char **pattern, int *filetype_num, char **filetype_list);
MYSQL * db_connect();
int db_close(MYSQL *mysql);

int match( POLICY * policy , char * buffer ,int length, int * resultnum , int (**presult)[2] , int * index );
int preprocess(POLICY ** ppolicy ,char * policyname ,char *attrs, int filetypenum, char * filetype ,int keynum,char * keylist,int patternnum,struct pattern_info * pattern);
#endif
