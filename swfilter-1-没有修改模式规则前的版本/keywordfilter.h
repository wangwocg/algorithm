#include "mysql/mysql.h"
#include <regex.h>
#include <errno.h>
#include "tos_list.h"
#include "swjni.h"
#ifndef _KEYWORDFILTER_H_
#define _KEYWORDFILTER_H_

#define N_SYMB     256		// characters per byte
#define SHIFT_SZ   4096		// sizeof shift_min
#define HASH_SZ 8192	// =(1<<13), must be a power of two
#define TR_SZ	256
#define W_DELIM	   128		// magic, unused
#define L_DELIM    10 		// must be '\n', unused

/*
typedef struct _strjniinfo{
	JavaVM* jvm;	//java虚拟机的句柄
	jclass cls ;		//java中某个类的句柄
	jmethodID getContentID ;	//java中getContent函数的句柄
	jmethodID getMimeTypeID ;//java中getMimeType 函数的句柄
	jmethodID getFileContentID ;//java中getFileContent函数的句柄
	jmethodID getFileMimeTypeID ;//java中getFileMimeType函数的句柄
}JNIINFO ;

typedef struct _str_configinfo{
	MYSQL * sql ;			//配置信息数据库的句柄
	pthread_mutex_t mutex ;	//策略信息头结点policyhead的互斥锁
	POLICY * policyhead ;		//指向策略信息链表的头结点，初始调用时为NULL
	JNIINFO * jniinfo ;		//跟tika有关的jni信息
}ConfigInfo ;

*/

/*
typedef struct{
	int a ;
}MYSQL;
*/
extern JavaVM* jvm;	//java虚拟机的句柄
extern jclass cls ;		//java中某个类的句柄
extern jmethodID getContentID ;	//java中getContent函数的句柄
extern jmethodID getMimeTypeID ;//java中getMimeType 函数的句柄
extern jmethodID getFileContentID ;//java中getFileContent函数的句柄
extern jmethodID getFileMimeTypeID ;//java中getFileMimeType函数的句柄
extern MYSQL * sql ;			//配置信息数据库的句柄
extern pthread_rwlock_t rwlock ;	//策略信息头结点policyhead的互斥锁
extern struct list_head POLICY_LIST ;		//指向策略信息链表的头结点
/*
struct list_head {
        struct list_head *next, *prev;
};
*/
struct hash_list {
	int index;		//当前hash的值
	struct hash_list * next;//有相同hash值的下一个关键词，空间由主进程收到配置更新信号后释放
};

struct WuManber
{
	unsigned int keywordnum ;	// 关键词的个数
	int minkeyword;	//关键词中的最小长度
	unsigned int *keywordlen;		// 每一个关键词长度的数组，长度为keywordnum，空间由主进程收到配置更新信号后释放
	unsigned char **keyword;		//关键词列表，空间由主进程收到配置更新信号后释放
	unsigned char tr[TR_SZ];	//关键词中每一个字符（字节）的前缀表，空间由主进程收到配置更新信号后释放
	unsigned char prehash[TR_SZ];	//提前对字符串做处理，用于生成字符的hash，空间由主进程收到配置更新信号后释放
	unsigned char shift[SHIFT_SZ];	//关键词中每一个字符（字节）的跳转表，空间由主进程收到配置更新信号后释放
	struct hash_list * hash[HASH_SZ];	//关键词中每一个字符（字节）的hash表，用链表来存放相同hash的字符串，空间由主进程收到配置更新信号后释放
	int matchnum;	//匹配到的个数
	int nocase ;	//是否忽略大小写
};
typedef struct _str_policy{
	struct list_head list ;
	char * policyname ;		//策略名称
	int filetypenum ;	//文件类型的个数
	char ** filetype ;	//文件类型
	struct WuManber * wm ;		//关键词预处理后的结果
	int patternnum ;	//模式个数
	char ** patternname ; //模式的名称列表
	regex_t * preg ;	//正则表达式预处理后的结果，用上方的regcomp函数预处理
}POLICY ;
typedef struct _str_MPOLICY{
	struct list_head list ;
	POLICY *policy;//空间不能由调用关键词过滤线程的父线程释放，这是POLICY链表的一部分，由主进程收到配置更新信号后释放
}MPOLICY ;
typedef struct{
	int * presultnum ;
	int (**presult)[2] ;
}CLEANRESULT;
typedef struct _str_keywordfilter{
	int finish ;	//标志位，初始值-1,0表示子线程运行中，1表示子线程已经结束运行
	pthread_t ppid ;	//新添加，父线程的线程ID，用于给父线程发送信号
	pthread_mutex_t *pmutex ;//用于通知父线程运行发现敏感信息
	pthread_cond_t *pcond ;//用于通知父线程运行发现敏感信息
	const char * buffer ;//文本缓冲区
	int bufferlen ;	//文本缓冲区长度
	int resultnum ;//返回检测到的关键词的个次数，即result指针指向的数组的长度
	int (* result)[2] ;//输出参数，指向长度为2的数组的指针，result[0][0]存放一个关键词的起始偏移，result[0][1]是这个关键词的结束偏移，空间由调用关键词过滤线程的父线程释放
	char * policyname ; //输出参数，匹配到的策略名称，NULL表示没有匹配到任何模式，以’\0’结尾，空间由调用关键词过滤线程的父线程释放
	struct list_head * mpolicy_head ;//指向策略的头指针，空间由调用关键词过滤线程的父线程释放
}KEYWORDFILTER ;

typedef struct _str_patternmatch{
	int finish ;		//标志位，初始值-1,0表示子线程运行中，1表示子线程已经结束运行
	pthread_t ppid ;	//新添加，父线程的线程ID，用于给父线程发送信号
	pthread_mutex_t *pmutex ;//用于通知父线程运行发现敏感信息
	pthread_cond_t *pcond ;//用于通知父线程运行发现敏感信息
	const char * buffer ;//文本缓冲区
	int bufferlen ;		//文本缓冲区长度
	int matchnum; 	//匹配到的个数
	int (* result)[2] ;//输出参数，指向长度为2的数组的指针，result[0][0]存放一个关键词的起始偏移，result[0][1]是这个关键词的结束偏移，空间由调用关键词过滤线程的父线程释放
	char * patternname ;	//输出参数，匹配到的模式名称，NULL表示没有匹配到任何模式，以’\0’结尾，空间由调用关键词过滤线程的父线程释放
	char * policyname ;	//输出参数，匹配到的策略名称，NULL表示没有匹配到任何模式，以’\0’结尾，空间由调用关键词过滤线程的父线程释放
	struct list_head * mpolicy_head ;//指向策略的头指针，空间由调用关键词过滤线程的父线程释放
}PATTERNMATCH;
#endif
