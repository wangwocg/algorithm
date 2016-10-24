#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "keywordfilter.h"
#include "filter.h"
#include "audit.h"
//extern POLICY *GetPolicy(char *policyname);

//#define Print_t(format,...) printf("Print_t time=%d utime=%lld FILE:"__FILE__",LINE:%d pid=%lu "format"\n",time(NULL),GetuTime(),__LINE__,pthread_self(),##__VA_ARGS__);

extern int SWFilterInit();

long long GetuTime();

struct list_head  * getMPOLICY( char * policynames );

extern int SWContentFilter( const char * buffer , int bufferlen, char * policynames , SW_EVENT_INFO *sweventinfo,int * pIfFound ,int txtflags );

extern int SWFileFilter( const char * path , char * policynames , SW_EVENT_INFO *sweventinfo,int * pIfFound );

int search(struct WuManber *wm, const unsigned char *buffer, int length,int *resultnum,int (**presult)[2]);

void freeWM( struct WuManber * wm );

static void wmfilter_onebit(struct WuManber * wm , const unsigned char * text, int length ,int *resultnum,int (**presult)[2]);

static void wmfilter_bits(struct WuManber *wm, const unsigned char * text , int length ,int *resultnum,int (**presult)[2]);

void * KeyWordFilter( void * pv );

int setResult(int start , int end , int * presultnum , int (**presult)[2] );

void freeResult( int (* result)[2] , int resultnum );

void * PatternMatch( void * pv );

int match( POLICY * policy , char * buffer , int * resultnum , int (**presult)[2] , int * index );

int preprocess(POLICY ** ppolicy ,char * policyname , int filetypenum, char ** filetype ,int keynum,char ** keylist,int patternnum,char ** patternname, char ** pattern);

void freePOLICY( POLICY * policy );

int preprocess_patternmatch( regex_t ** ppreg , int patternnum , char ** pattern );

void freePatternMatch(regex_t * preg , int patternnum );

static int createHash(struct WuManber *wm, int hash_index, unsigned char *keyword);

static int createWM( struct WuManber * wm , int keywordnum, unsigned char **keyword );

static void freeHash( struct WuManber * wm );

int preprocess_keyword(struct WuManber ** pwm, int keywordnum , unsigned char **keyword);

void freeWM( struct WuManber * wm );

void cleanup(void * pv);

void freeKEYWORDFILTER( KEYWORDFILTER * keywordfilter );

void freePATTERNMATCH( PATTERNMATCH * patternmatch );

int updatePOLICY();

//只释放MPOLICY结构体的空间，头指针需要单独释放，因为头指针可能不是malloc分配的
void freeMPOLICY(struct list_head * mpolicy_head );

int setResult(int start , int end , int * presultnum , int (**presult)[2] ){
	int (*result)[2] = *presult ;
	int (*tmp)[2] = NULL ;
	//假设realloc不是线程取消点
	tmp = (int (*)[2])realloc(result , sizeof(int[2]));
	if( tmp == NULL ){
		return -1 ;
	}
	result = tmp ;
	result[*presultnum][0] = start ;
	result[*presultnum][1] = end ;
	*presult = result ;
	++*presultnum ;
	return 0 ;
}
static void wmfilter_onebit(struct WuManber * wm , const unsigned char * text, int length ,int *resultnum,int (**presult)[2]){
	const unsigned char * textend = text + length ;
	const unsigned char * textstart = text ;
	int  j , ret = 0 ; 
	struct hash_list * p ;
	int hash_index ; 
	int matched = 0 ;
	unsigned char * tr = wm->tr ;
	unsigned char * qx ;

	int count = 0 ;
	CLEANRESULT cleanresult ;
	cleanresult.presultnum = resultnum ;
	cleanresult.presult = presult ;
	
	text = text - 1 ;
	pthread_cleanup_push((void(*)(void *)) cleanup,(void *)&cleanresult);
	while (++text <= textend ){
		if( count == 100 ){
			//测试一下是否有要求线程退出的请求
			count = 0 ;
			pthread_testcancel();
		}
		p = wm->hash[*text];
		while(p != NULL){
			hash_index = p->index;
			p = p->next;
			qx = (unsigned char *)text;
			j = 0;
			while(tr[wm->keyword[hash_index][j]] == tr[*(qx++)]){
				j++;
			}
			if(wm->keywordlen[hash_index] <= j) {
				if(text >= textend){
					return;
				}
				
				printf("match keyword in offset %d-%d is %s\n",text-textstart,text-textstart+wm->keywordlen[hash_index]-1,wm->keyword[hash_index]);
				ret = setResult(text-textstart,text-textstart+wm->keywordlen[hash_index]-1,resultnum,presult);
				if( ret != 0 ){
					printf("realloc failed\n");
				}
				else{
					wm->matchnum++;
					matched = 1 ;
				}
			}
		}
		matched = 0;
		++count ;
	}
	pthread_cleanup_pop(0);
	return ;
}
void cleanup(void * pv){
	if( pv == NULL ){
		return ;
	}
	Print_t("in cleanup pid=%llu\n",pthread_self());
	CLEANRESULT * cleanresult = (CLEANRESULT *)pv ;
	freeResult( *cleanresult->presult , *cleanresult->presultnum );
	*cleanresult->presult = NULL ;
	*cleanresult->presultnum = 0 ;
	Print_t("end cleanup pid=%llu\n",pthread_self());
	return  ;
}
static void wmfilter_bits(struct WuManber *wm, const unsigned char * text , int length ,int *resultnum,int (**presult)[2]){
	const unsigned char * textstart = text ;
	const unsigned char * textend = text + length ;
	unsigned char shift ; 
	unsigned int hash, i, m1, j;
	int bit3 = 0 , ret = 0 ; 
	int hash_index;
	int m = wm->minkeyword ; 
	int matched = 0;
	unsigned char *tr = wm->tr;
	unsigned char * prehash = wm->prehash ;
	unsigned char *qx;
	struct hash_list *p;
	int count = 0 ;
	CLEANRESULT cleanresult ;
	cleanresult.presultnum = resultnum ;
	cleanresult.presult = presult ;
	m1 = m - 1;
	text = text + m1;
	if( wm->keywordnum > 100 && wm->minkeyword > 2 ){
		bit3 = 1 ;
	}
	else{
		bit3 = 0 ;
	}
	pthread_cleanup_push(cleanup,(void *)&cleanresult);
	while (text <= textend) {
		if( count == 100 ){
			//测试一下是否有要求线程退出的请求
			count = 0 ;
			pthread_testcancel();
		}
		hash=prehash[*text];
		hash=(hash<<4)+(prehash[*(text-1)]);
		if(bit3) hash=(hash<<4)+(prehash[*(text-2)]);
		shift = wm->shift[hash];
		if(shift == 0) {
			// text points to the m'th char of a candidate pattern
			hash=0;
			for(i=0;i<=m1;i++)  {
				hash=(hash<<4)+(prehash[*(text-i)]);
			}
			hash=hash&(HASH_SZ-1);
			p = wm->hash[hash];
			while(p != 0) {
				hash_index = p->index;
				p = p->next;
				qx = (unsigned char *)(text-m1);
#if DANGEROUS_BEANS
				// This code may read across array bounds.
				// It performs 10% better in one of my test cases.
				j = 0;
				while(tr[wm->keyword[hash_index][j]] == tr[*(qx++)]) j++;
				if (j > m1 ){ 
					if(wm->keywordlen[hash_index] <= j) {
						if(text > textend){
							return;
						}
#else
				if (text > textend){
					return;
				}
				int l = wm->keywordlen[hash_index];
				if (qx+l <= textend){
					j = 0;
					// not checking tr[] when not needed also saves 10%
					if ( wm->nocase )
						while ((--l>=0) && (tr[wm->keyword[hash_index][l]] == tr[qx[l]])) ;
					else
						while ((--l>=0) && (   wm->keyword[hash_index][l]  ==    qx[l])) ;
					if (l < 0){
#endif
						
						printf("match keyword in offset %d-%d is %s\n",text-textstart-m1,text-textstart-m1+wm->keywordlen[hash_index]-1,wm->keyword[hash_index]);
						ret = setResult(text-textstart-m1,text-textstart-m1+wm->keywordlen[hash_index]-1,resultnum,presult);
						if( ret != 0 ){
							printf("realloc failed\n");
						}
						else{
							wm->matchnum++;
							matched=1;
						}
					}
				}
			
			}
			matched = 0;
			shift = 1;	// take care of overlapping matches
		}
		++count ;
		text = text + shift;
	}
	pthread_cleanup_pop(0);
	return ;
}

/*
	参数:
		wm		struct WuManber结构体，用于存放关键词预处理后的结果，具体定义见下方
		buffer	需要过滤的文本缓冲区
		length	文本缓冲区的长度
		resultnum	用于保存匹配到的关键词的个数
		presult	用于存放每一个关键词在文本缓冲区中的偏移量信息，指向长度为2的整型数组的指针的指针
	返回值：
		0	操作成功	-1	操作失败
*/
int search(struct WuManber *wm, const unsigned char *buffer, int length,int *resultnum,int (**presult)[2]){
	if( wm == NULL ){
		return -1 ;
	}
	if( wm->minkeyword == 1 ){
		wmfilter_onebit( wm , buffer , length ,resultnum , presult);
	}
	else{
		wmfilter_bits( wm , buffer , length , resultnum , presult );
	}
	return 0 ;
}

/*
	参数：
		pv	因为这是一个线程运行的函数，所以这个参数是由下方的KEYWORDFILTER 结构体转换而成的指针
	返回值：
		空指针
*/
void * KeyWordFilter( void * pv ){
	
	KEYWORDFILTER * filter = ( KEYWORDFILTER * )pv ;
	const char * buffer = filter->buffer ;
	int bufferlen = filter->bufferlen ;
	struct list_head * mpolicy_head = filter->mpolicy_head ;
	struct list_head * lp = NULL ;
	MPOLICY * mpolicy = NULL ;
	POLICY * policy = NULL ;
	struct WuManber * wm = NULL ;
	int resultnum = 0 , len = 0 ;
	//result和resultnum一定要初始化，否则会在cleanup中崩溃
	pthread_cond_t * pcond = filter->pcond ;
	pthread_mutex_t * pmutex = filter->pmutex ;
	int (*result)[2] = NULL ;
	char * policyname = NULL ;
	Print_t("");
	/*
	if( buffer == NULL || bufferlen <= 0 || mpolicy_head == NULL || list_empty(mpolicy_head ) ){
		filter->finish = 1 ;
		return ;
	}
	*/
	filter->finish = 0 ;
	Print_t("");
	list_for_each(lp , mpolicy_head ){
		mpolicy = list_entry(lp, MPOLICY, list);
		Print_t("policyname=%s",mpolicy->policy->policyname);
		if( mpolicy->policy != NULL ){
			search( mpolicy->policy->wm , buffer , bufferlen , &resultnum , &result );
			Print_t("");
			if( resultnum > 0 ){
				len = strlen(mpolicy->policy->policyname);
				policyname = (char *)malloc(sizeof(char)*(len+1));
				if( policyname == NULL ){
					freeResult(result, resultnum);
					break ;
				}
				strncpy(policyname , mpolicy->policy->policyname , len+1 );
				
				filter->resultnum = resultnum ;
				filter->result = result ;
				filter->policyname = policyname ;
				filter->finish = 1 ;
				//发送信号给父线程
				pthread_mutex_lock(pmutex);
				pthread_cond_signal(pcond);
				pthread_mutex_unlock(pmutex);
			//	pthread_kill();
				Print_t("keywordfilter quit pid=%llu",pthread_self());
				return ;
			}
			Print_t("");
		}
	}
	
	filter->resultnum = 0 ;
	filter->finish = 1 ;
	//发送信号给父线程
	pthread_mutex_lock(pmutex);
	pthread_cond_signal(pcond);
	pthread_mutex_unlock(pmutex);
	Print_t("keywordfilter quit pid=%llu",pthread_self());
	return ;
}
void freeResult( int (* result)[2] , int resultnum ){
	/*
	int i = 0 ;
	if( result == NULL ){
		return ;
	}
	for( i=0;i< resultnum ; ++i){
		if( result[i] != NULL ){
			free(result[i]);
		}
	}
	*/
	if( resultnum <= 0 ){
		return ;
	}
	free(result);
	return ;
}

/*
	参数：
		pv	因为这是一个线程运行的函数，所以这个参数是由下方的PATTERNMATCH结构体转换而成的指针
	返回值：
		空指针
*/
void * PatternMatch( void * pv ){
	PATTERNMATCH * patternmatch = NULL ;
	int ret = 0 , index = -1 , resultnum = 0 ;
	int (*result)[2] = NULL ;
	char * patternname = NULL ;
	char * policyname = NULL ;
	int len = 0 ;
	struct list_head * lp = NULL ;
	MPOLICY * mpolicy = NULL ;

	patternmatch = (PATTERNMATCH *)pv ;
	const char * buffer = patternmatch->buffer ;
	int bufferlen = patternmatch->bufferlen ;
	struct list_head * mpolicy_head = patternmatch->mpolicy_head ;
	pthread_cond_t * pcond = patternmatch->pcond ;
	pthread_mutex_t * pmutex = patternmatch->pmutex ;
	patternmatch->finish = 0 ;
	
	list_for_each(lp , mpolicy_head ){
		mpolicy = list_entry(lp, MPOLICY, list);
		if( mpolicy->policy != NULL ){
			ret = match(mpolicy->policy , (char *)(patternmatch->buffer) , &resultnum , &result , &index );
			if( ret == 0 && resultnum > 0 && index >= 0 ){
				len = strlen(mpolicy->policy->patternname[index]);
				patternname = (char *)malloc(sizeof(char)*(len+1));
				if( patternname == NULL ){
					freeResult(result , resultnum);
					break ;
				}
				strncpy(patternname , mpolicy->policy->patternname[index] , len+1 );
				
				len = strlen(mpolicy->policy->policyname );
				policyname = (char *)malloc(sizeof(char)*(len+1));
				if( policyname == NULL ){
					free(patternname);
					freeResult(result , resultnum);
					break ;
				}
				strncpy(policyname , mpolicy->policy->policyname , len+1 );
				
				patternmatch->matchnum = resultnum ;
				patternmatch->result = result ;
				patternmatch->patternname = patternname ;
				patternmatch->policyname = policyname ;
				patternmatch->finish = 1 ;
				//发送信号
				pthread_mutex_lock(pmutex);
				pthread_cond_signal(pcond);
				pthread_mutex_unlock(pmutex);
				Print_t("patternmatch quit pid=%llu",pthread_self());
				return ;
			}
		}
	}
	patternmatch->matchnum = 0 ;
	patternmatch->finish = 1 ;
	//发送信号
	pthread_mutex_lock(pmutex);
	pthread_cond_signal(pcond);
	pthread_mutex_unlock(pmutex);
	Print_t("patternmatch quit pid=%llu",pthread_self());
	return ;
}


int match( POLICY * policy , char * buffer , int * resultnum , int (**presult)[2] , int * index ){
	if( policy == NULL || policy->preg == NULL || buffer == NULL || resultnum == NULL || presult == NULL || index == NULL ){
		return -1 ;
	}
	int patternnum = policy->patternnum , i = 0 ;
	regex_t * preg = policy->preg ;
	int nmatch = 1 , ret = 0 ;
	int (*result)[2] = NULL ;
	regmatch_t matchptr[1] ;
/*	
	int count = 0 ;
	RESULT cleanresult ;
	cleanresult.presultnum = resultnum ;
	cleanresult.presult = presult ;
	pthread_cleanup_push(cleanup,(void *)&cleanresult);
*/
	for( i=0; i < patternnum ; ++i ){
		pthread_testcancel();
		ret = regexec( preg+i , buffer , nmatch , matchptr , 0);
		if( ret == 0 ){
			result = (int (*)[2] )malloc(sizeof(int[2]));
			if( result == NULL ){
				*resultnum = 0 ;
				*index = -1 ;
				return -1 ;
			}
			*resultnum = 1 ;
			(*result)[0] = (int)(matchptr[0].rm_so) ;
			(*result)[1] = (int)(matchptr[0].rm_eo) ;
			*presult = result ;
			*index = i ;
			return 0 ;
		}
	}
	*resultnum = 0 ;
	*index = -1 ;
	return 0 ;
}

/*
	参数：
		ppolicy	输出参数，保存预处理后的结果，操作失败时为NULL，空间由主进程收到配置更新信号后释放
		policyname	策略对象的名称，空间由主进程收到配置更新信号后释放
		filetypenum	文件类型的个数
		filetype	文件类型字符串，会插入到policy结构体中，所以空间由主进程收到配置更新信号后释放
		keynum	关键词个数
		keylist	关键词列表，会插到policy结构体中，所以空间由主进程收到配置更新信号后释放
		patternnum	模式个数
		pattern	模式列表，会插入到policy结构体中，所以空间由主进程收到配置更新信号后释放
	返回值：
		O 操作成功 -1 操作失败,如果操作失败，上述空间都由调用者释放
*/

int preprocess(POLICY ** ppolicy ,char * policyname , int filetypenum, char ** filetype ,int keynum,char ** keylist,int patternnum,char ** patternname, char ** pattern){
	POLICY * policy = NULL ;
	int ret = 0 ;
	if( filetypenum < 0 || keynum < 0 || patternnum < 0 || (filetype == NULL && keylist == NULL && pattern == NULL ) || (patternname && !pattern ) || (!patternname && pattern )  ){
		return -1 ;
	}
	policy = (POLICY *)malloc(sizeof(POLICY));
	if( policy == NULL ){
		return -1 ;
	}
	memset(policy , 0 , sizeof(POLICY));
	policy->policyname = policyname ;
	policy->filetypenum = filetypenum ;
	policy->filetype = filetype ;
	policy->patternnum = patternnum ;
	policy->patternname = patternname ;
	//预处理
	if( keynum > 0 && keylist != NULL ){
		ret = preprocess_keyword( &policy->wm,keynum,(unsigned char **)keylist );
		if( ret != 0 ){
			if( policy != NULL ){
				free(policy);
			}
			return -1 ;
		}
	}
	
	if( patternnum > 0 && pattern != NULL ){
		ret = preprocess_patternmatch( &policy->preg , patternnum , pattern );
		if( ret != 0 ){
			if( policy->wm != NULL ){
				freeWM(policy->wm);
				policy->wm = NULL ;
			}
			if( policy != NULL ){
				free(policy);
			}
			return -1 ;
		}
	}
	
	*ppolicy = policy ;
	return 0 ;
}

void freePOLICY( POLICY * policy ){
	int i = 0 ;
	int filetypenum = policy->filetypenum ;
	int patternnum = policy->patternnum ;
	char ** filetype = NULL ;
	int keywordnum = 0 ;
	unsigned char ** keyword = NULL ;
	char ** patternname = NULL ;
	if( policy == NULL ){
		return ;
	}
	if( policy->policyname ){
		free( policy->policyname );
		policy->policyname = NULL ;
	}
	if( policy->filetype ){
		filetype = policy->filetype ;
		for( i = 0 ; i < filetypenum ; ++i ){
			if( filetype[i] ){
				free( filetype[i] );
			}
		}
		free(filetype);
		policy->filetype = NULL ;
	}
	if( policy->wm ){
		keywordnum = policy->wm->keywordnum ;
		keyword = policy->wm->keyword ;
		if( keyword ){
			for( i = 0 ; i < keywordnum ; ++i){
				if( keyword[i] ){
					free(keyword[i]);
				}
			}
			free(keyword);
			policy->wm->keyword = NULL ;
		}
		freeWM( policy->wm );
		policy->wm = NULL ;
	}
	if( policy->patternname ){
		patternname = policy->patternname ;
		for( i = 0 ; i < patternnum ; ++i ){
			if( patternname[i] ){
				free( patternname[i] );
			}
		}
		free(patternname);
		policy->patternname = NULL ;
	}
	if( policy->preg ){
		freePatternMatch( policy->preg , policy->patternnum );
	}
	free(policy);
	return ;
}
int preprocess_patternmatch( regex_t ** ppreg , int patternnum , char ** pattern ){
	int i = 0 , j = 0 ;
	regex_t * preg = NULL ;
	regex_t reg ;
	if( patternnum <= 0 || pattern == NULL ){
		return 0 ;
	}
	preg = (regex_t *)malloc(sizeof(regex_t)*patternnum);
	if( preg == NULL ){
		return -1 ;
	}
	memset(preg , 0 ,sizeof(regex_t)*patternnum );
	memset( &reg , 0 , sizeof(regex_t));
	for( i=0;i<patternnum ; ++i ){
	//	if(  pattern[i] && regcomp( preg+i , pattern[i] , REG_EXTENDED|REG_ICASE ) != 0 ){
		if(  pattern[i] && regcomp( preg+i , pattern[i] , REG_ICASE ) != 0 ){
			for( j=0;j<i;++j){
				if( memcmp( preg+j , &reg, sizeof(regex_t) ) != 0 ){
					regfree(preg+j);
				}
			}
			free(preg);
			return -1 ;
		}
	}
	*ppreg = preg ;
	return 0 ;
}

void freePatternMatch(regex_t * preg , int patternnum ){
	int i = 0 ;
	regex_t reg ;
	if( preg == NULL ){
		return ;
	}
	memset( &reg , 0 , sizeof(regex_t));
	for( i=0 ; i < patternnum ; ++i ){
		if( memcmp( preg+i , &reg, sizeof(regex_t) ) != 0 ){
			regfree(preg+i);
		}
	}
	free(preg);
	return ;
}
static int createHash(struct WuManber *wm, int hash_index, unsigned char *keyword){
	int i, m;
	struct hash_list  *pt, *qt;
	unsigned hashvalue=0, Mask=15;
	m = wm->minkeyword ;
	int onebit = 0 , bits = 0 ;
	if (wm->keywordnum > 100 && wm->minkeyword > 2){
		bits = 1 ;
	}
	if (wm->minkeyword == 1){
		onebit = 1 ;
	}
	for (i = m-1; i>=(1+bits); i--) {
		hashvalue = (keyword[i] & Mask);
		hashvalue = (hashvalue << 4) + (keyword[i-1]& Mask);
		if(bits) hashvalue = (hashvalue << 4) + (keyword[i-2] & Mask);
		if(wm->shift[hashvalue] >= m-1-i) wm->shift[hashvalue] = m-1-i;
	}
	if(onebit) Mask = 255;  /* 011111111 */
	hashvalue = 0;
	for(i = m-1; i>=0; i--)  {
		hashvalue = (hashvalue << 4) + (wm->tr[keyword[i]]&Mask);
	}
	hashvalue=hashvalue&(HASH_SZ-1);
	qt = (struct hash_list *) malloc(sizeof(struct hash_list));
	if( qt == NULL ){
		return -1 ;
	}
	qt->index = hash_index;
	pt = wm->hash[hashvalue];
	qt->next = pt;
	wm->hash[hashvalue] = qt;
	return 0 ;
}

static int createWM( struct WuManber * wm , int keywordnum, unsigned char **keyword ){
	wm->nocase = 1 ;
	wm->keyword = keyword;
	wm->keywordnum = keywordnum;

	unsigned Mask = 15;
	int i = 0 , len = 0 , ret = 0 ;
	for(i=0; i< TR_SZ; i++) wm->tr[i] = i;
	if(wm->nocase){
		for(i='A'; i<= 'Z'; i++) wm->tr[i] = i + 'a' - 'A';
	}
	for(i=0; i< TR_SZ; i++) wm->prehash[i] = wm->tr[i]&Mask;

	wm->keywordlen = (unsigned int *)calloc(keywordnum+2, sizeof(unsigned int));
	if( wm->keywordlen == NULL ){
		return -1 ;
	}
	wm->minkeyword = 255;		// max that fits in shift[] entries.
	for( i=0 ; i < wm->keywordnum; i++){
		len = strlen((char *)wm->keyword[i]);
		wm->keywordlen[i] = len;
		if (len !=0 && len < wm->minkeyword) wm->minkeyword = len;//记录最短的模式的长度
    }
	if (wm->minkeyword == 0){
		if( wm->keywordlen != NULL ){
			free(wm->keywordlen);
			wm->keywordlen = NULL ;
		}
		return -1 ;
    }
	for (i=0; i<SHIFT_SZ; i++) wm->shift[i] = wm->minkeyword - 2;
	for (i=0; i<HASH_SZ; i++) wm->hash[i] = 0;
	for (i=0; i< keywordnum; i++){
		ret = createHash(wm, i, wm->keyword[i]);
		if( ret != 0 ){
			if( wm->keywordlen != NULL ){
				free(wm->keywordlen);
				wm->keywordlen = NULL ;
			}
			freeHash(wm);
			return -1 ;
		}
	}
	return 0 ;
}
static void freeHash( struct WuManber * wm ){
	int i = 0 ;
	struct hash_list * list = NULL , * last = NULL ;//hash[HASH_SZ];
	if( wm == NULL ){
		return ;
	}
	for( i=0 ; i < HASH_SZ ; i++ ){
		list = wm->hash[i] ;
		while( list != NULL ){
			last = list ;
			list = list->next ;
			free(last);
		}
		wm->hash[i] = NULL ;
	}
	return ;
}
/*
	参数：
		pwm		用于存放预处理后的结果，会插入到POLICY链表中，所以空间由主进程收到配置更新信号后释放
		keywordnum 	关键词的个数
		keyword	关键词列表
	返回值：
		0	操作成功	-1	操作失败	
*/
int preprocess_keyword(struct WuManber ** pwm, int keywordnum , unsigned char **keyword){
	int ret = 0 ;
	if( keywordnum <= 0 || keyword == NULL ){
		return 0 ;
	}
	struct WuManber * wm = NULL ;
	wm = (struct WuManber *)malloc(sizeof(struct WuManber));
	if( wm == NULL ){
		return -1 ;
	}
	memset(wm , 0 , sizeof(struct WuManber));
	wm->keywordnum = keywordnum ;
	wm->keyword = keyword ;
	ret = createWM(wm,keywordnum,keyword);
	if( ret != 0 ){
		if( wm != NULL ){
			free(wm);
		}
		return -1 ;
	}
	*pwm = wm ;
	return 0 ;
}

//不会释放WM结构体中的关键词列表keyword
void freeWM( struct WuManber * wm ){
	if( wm == NULL ){
		return ;
	}
	if( wm->keywordlen != NULL ){
		free(wm->keywordlen);
		wm->keywordlen = NULL ;
	}
	freeHash(wm);
	free(wm);
	return ;
}


struct list_head * getMPOLICY( char * policynames ){
	char * tmp = policynames , * policyname = policynames ;
	POLICY * policy = NULL ;
	MPOLICY * mpolicy = NULL ;
	if( policynames == NULL ){
		return NULL ;
	}
	struct list_head * mpolicy_head = (struct list_head *)malloc(sizeof(struct list_head));
	if( mpolicy_head == NULL ){
		return NULL ;
	}
	INIT_LIST_HEAD( mpolicy_head );
	while( tmp == policyname ){
		tmp = strchr( policyname , ';');
		if( tmp == policyname ){
			++tmp ;
			++policyname ;
		}
	}
	
	while( tmp != NULL ){
		*tmp = '\0';
		mpolicy = (MPOLICY *)malloc(sizeof(MPOLICY));
		if( mpolicy == NULL ){
			*tmp = ';';
			policyname = ++tmp ;
			while( tmp == policyname ){
				tmp = strchr( policyname , ';');
				if( tmp == policyname ){
					++tmp ;
					++policyname ;
				}
			}
			continue ;
		}
		policy = GetPolicy(policyname);
		if( policy == NULL ){
			*tmp = ';';
			policyname = ++tmp ;
			while( tmp == policyname ){
				tmp = strchr( policyname , ';');
				if( tmp == policyname ){
					++tmp ;
					++policyname ;
				}
			}
			continue ;
		}
		mpolicy->policy = policy ;
		list_add_tail( &mpolicy->list , mpolicy_head );
		*tmp = ';';
		policyname = ++tmp ;
		
		while( tmp == policyname ){
			tmp = strchr( policyname , ';');
			if( tmp == policyname ){
				++tmp ;
				++policyname ;
			}
		}
	}
	if( strlen(policyname) > 0 ){
		mpolicy = (MPOLICY *)malloc(sizeof(MPOLICY));
		if( mpolicy == NULL ){
			return mpolicy_head ;
		}
		policy = GetPolicy(policyname);
		if( policy == NULL ){
			free(mpolicy);
			return mpolicy_head ;
		}
		mpolicy->policy = policy ;
		_list_add( &mpolicy->list , mpolicy_head );
	}
	return mpolicy_head ;
}

int SWContentFilter( const char * buffer , int bufferlen, char * policynames , SW_EVENT_INFO *sweventinfo,int * pIfFound ){
	struct list_head * mpolicy_head = NULL ;
	MPOLICY * mpolicy = NULL ;
	int length = 0 ;
	if( buffer == NULL || bufferlen <= 0 || policynames == NULL || sweventinfo == NULL || pIfFound == NULL ){
		return -1 ;
	}
	pthread_rwlock_rdlock(&rwlock);
	mpolicy_head = getMPOLICY(policynames);
	if( mpolicy_head == NULL ){
		pthread_rwlock_unlock(&rwlock);
		return -1 ;
	}
	if( list_empty(mpolicy_head) ){
		free(mpolicy_head);
		pthread_rwlock_unlock(&rwlock);
		return -1 ;
	}
	invokebyte(buffer, bufferlen , getMimeTypeID , getContentID , mpolicy_head , sweventinfo, pIfFound );
	if( mpolicy_head != NULL ){
		freeMPOLICY(mpolicy_head);
		free(mpolicy_head);
	}
	pthread_rwlock_unlock(&rwlock);
	return 0 ;
}

int SWFileFilter( const char * path , char * policynames , SW_EVENT_INFO *sweventinfo,int * pIfFound ){
	struct list_head * mpolicy_head = NULL ;
	MPOLICY * mpolicy = NULL ;
	int length = 0 ;
	if( path == NULL || policynames == NULL || sweventinfo == NULL || pIfFound == NULL ){
		return -1 ;
	}
	pthread_rwlock_rdlock(&rwlock);
	Print_t("");
	mpolicy_head = getMPOLICY(policynames);
	if( mpolicy_head == NULL ){
		pthread_rwlock_unlock(&rwlock);
		return -1 ;
	}
	Print_t("");
	if( list_empty(mpolicy_head) ){
		free(mpolicy_head);
		pthread_rwlock_unlock(&rwlock);
		return -1 ;
	}
	Print_t("");
	length = strlen(path);
	invokebyte(path, length, getFileMimeTypeID , getFileContentID , mpolicy_head , sweventinfo, pIfFound );
	if( mpolicy_head != NULL ){
		freeMPOLICY(mpolicy_head);
		free(mpolicy_head);
	}
	Print_t("");
	pthread_rwlock_unlock(&rwlock);
	return 0 ;
}

//会从mpolicy_head中删除文件类型不匹配的节点
int matchfiletype(char * mime , int mimelen , struct list_head * mpolicy_head , int * pIfKeyWord , int * pIfPatternMatch ){
	MPOLICY * mpolicy = NULL ;
	char ** filetype = NULL ;
	int filetypenum = 0 , i = 0 ;
	struct list_head * lh = NULL ;
	list_for_each(lh, mpolicy_head){
		Print_t("");
		mpolicy = list_entry(lh, MPOLICY, list);
		filetype = mpolicy->policy->filetype ;
		filetypenum = mpolicy->policy->filetypenum ;
		Print_t("policyname=%s",mpolicy->policy->policyname);
		for( i = 0 ; i < filetypenum ; ++i ){
			if( strcmp( filetype[i] , mime ) == 0 || strcmp( filetype[i] , "*") == 0 ){
				break ;
			}
		}
		if( i >= filetypenum ){
			list_del(lh);
			free(mpolicy);
		}
		else{
			if( mpolicy->policy->wm != NULL ){
				++*pIfKeyWord;
			}
			if( mpolicy->policy->patternnum > 0 ){
				++*pIfPatternMatch ;
			}
		}
	}
	if( list_empty(mpolicy_head) ){
		//策略中的文件类型都不匹配
		return 0 ;
	}
	return 1 ;
}
//改名字
int invokebyte(char * buffer, int length,jmethodID getMimeTypeID , jmethodID getContentID ,struct list_head * mpolicy_head ,SW_EVENT_INFO *sweventinfo,int * pIfFound  ){
    JNIEnv* env = NULL ;
	int nret = -1 ;
    pthread_t keywordid = 0 , patternmatchid = 0 ;
	int nIfKeyWord = 0 , nIfPatternMatch = 0 ;
	KEYWORDFILTER * keywordfilter = NULL ;
	PATTERNMATCH * patternmatch = NULL ;
	int nmatch = 0 , allfinish = 0 , i = 0;
	pthread_mutex_t mutex ;
	pthread_cond_t cond ;
	pthread_mutex_init( &mutex , NULL ) ;
	pthread_cond_init( &cond , NULL ) ;
	Print_t("");
	if( (*jvm)->AttachCurrentThread(jvm,(void **)&env,NULL) != JNI_OK ){
		printf("AttachCurrentThread error errno=%d\n",errno);
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
		return -1;
	}
	Print_t("");
	if((*env)->ExceptionOccurred(env)){
        printf("in invokebyte have exception while AttachCurrentThread\n");
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
		return -1;
    }
	//放到主进程
    jobject obj = getInstance(env,cls);//然后根据类获取对应的实例
	
//	jmethodID getContent = get_method(env,cls,"getContent","([B)Ljava/lang/String;");//根据类\方法名和签名获取到对应的方法
		
		jbyteArray jbytes = (*env)->NewByteArray(env,length);
		Print_t("");
	if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while NewByteArray\n");
				pthread_mutex_destroy(&mutex);
				pthread_cond_destroy(&cond);
                return -1;
    }
		(*env)->SetByteArrayRegion(env,jbytes,0,length,buffer);
	if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while SetByteArrayRegion\n");
				pthread_mutex_destroy(&mutex);
				pthread_cond_destroy(&cond);
                return -1;
    }
	char * mime = NULL ;
	int mimelen = 0 ;
	jstring jstrmime_ret ;
	nret = getMimeType(&mime, &mimelen ,env,obj , getMimeTypeID  , jbytes,&jstrmime_ret);
	if( nret != 0 ){
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
		return -1;
	}
		Print_t("mime=%s",mime);
		//匹配mime类型，返回值为0则没有匹配上文件类型
		if( matchfiletype(mime,mimelen,mpolicy_head , &nIfKeyWord , &nIfPatternMatch ) == 0 ){
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
			//释放mime
			return 0 ;
		}
		Print_t("");
		//从这里开始，MPOLICY结构体链表中的策略都是文件类型和mime匹配的
		(*env)->ReleaseStringUTFChars(env,jstrmime_ret,mime);
		if((*env)->ExceptionOccurred(env)){
            printf("in invokebyte have exception while ReleaseStringUTFChars\n");
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
            return -1;
        }
		Print_t("");
		if( nIfKeyWord == 0 && nIfPatternMatch == 0 ){
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
			return 0 ;
		}
		//end
		char * text = NULL ;
		int textlen = 0 ;
		jstring jstr_ret ;
		Print_t("");
		nret = getContent( &text, &textlen , env, obj , getContentID , jbytes, &jstr_ret);
		if( nret != 0 ){
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
			return -1;
		}
        
			printf("filtering...\n");
		//	search(ret,len,senwords,senwordslen,&dis);
			/*
			if( 1 ){
				pthread_create(&keywordid,NULL,);
			}*/
		//	printf("textlen=%d text=%s\n",textlen,text);
			
			if( nIfKeyWord > 0 ){
				Print_t("");
				keywordfilter = (KEYWORDFILTER *)malloc(sizeof(KEYWORDFILTER));
				if( keywordfilter != NULL ){
					memset(keywordfilter, 0 , sizeof(KEYWORDFILTER));
					//初始化
					keywordfilter->finish = -1 ;
					keywordfilter->ppid = pthread_self() ;
					keywordfilter->buffer = text ;
					keywordfilter->bufferlen = textlen ;
					keywordfilter->mpolicy_head = mpolicy_head ;
					keywordfilter->pcond = &cond ;
					keywordfilter->pmutex = &mutex ;
					//end初始化
					if( pthread_create( &keywordid , NULL , (void*(*)(void*))KeyWordFilter,(void *)keywordfilter ) != 0 ){
						Print_t("");
						keywordid = 0 ;
						free(keywordfilter);
						keywordfilter = NULL ;
					}
					Print_t("keywordid=%llu",keywordid);
				}
			}
			else{
				//如果不需要关键词过滤，则假设关键词过滤已经完成了
				allfinish |= 0x01 ;
				Print_t("no need keywordid");
			}
			if( nIfPatternMatch > 0 ){
				Print_t("");
				patternmatch = (PATTERNMATCH *)malloc(sizeof(PATTERNMATCH));
				if( patternmatch != NULL ){
					memset(patternmatch, 0 , sizeof(PATTERNMATCH));
					//初始化
					patternmatch->finish = -1 ;
					patternmatch->ppid = pthread_self() ;
					patternmatch->buffer = text ;
					patternmatch->bufferlen = textlen ;
					patternmatch->mpolicy_head = mpolicy_head ;
					patternmatch->pcond = &cond ;
					patternmatch->pmutex = &mutex ;
					//end初始化
					if( pthread_create( &patternmatchid , NULL , (void*(*)(void*))PatternMatch,(void *)patternmatch ) != 0 ){
						Print_t("");
						patternmatchid = 0 ;
						free(patternmatch);
						patternmatch = NULL ;
					}
					Print_t("patternmatchid=%llu",patternmatchid);
				}
			}
			else{
				//如果没有模式匹配，则假设模式匹配已经完成了
				allfinish |= 0x02 ;
				Print_t("no need patternmatch");
			}
			if( keywordid <= 0 && patternmatchid <= 0 ){
				Print_t("all quit");
				pthread_mutex_destroy(&mutex);
				pthread_cond_destroy(&cond);
				return 0 ;
			}
			pthread_mutex_lock(&mutex);
			while(1){
				Print_t("i am awake check if finish");
				if( (allfinish&0x01) == 0 && keywordid > 0 && keywordfilter->finish == 1 && nmatch == 0 ){ //have finished
					Print_t("keywordfilter finished match=%d",keywordfilter->resultnum);
					allfinish |= 0x01 ;
					nmatch = keywordfilter->resultnum;
					Print_t("will kill patternmatchid=%llu finish=%d",patternmatchid,patternmatchid>0?patternmatch->finish:2);
					if(nmatch > 0){ //matched key words
						if( patternmatchid > 0 && patternmatch->finish != 1){
							//kill patternmatch thread.
							Print_t("killing patternmatchid=%llu",patternmatchid);
							exit_pthread(patternmatchid);
						}
					}
				}
				if( (allfinish&0x02) == 0 && patternmatchid > 0 && patternmatch->finish == 1 && nmatch == 0 ){
					allfinish |= 0x02 ;
					nmatch = patternmatch->matchnum;
					Print_t("patternmatch finished match=%d",nmatch);
					if( nmatch > 0){
						Print_t("will kill keywordid=%llu finish=%d",keywordid,keywordid>0?keywordfilter->finish:2);
						if( keywordid > 0 && keywordfilter->finish != 1) {
							//kill keywordfilter thread
							Print_t("killing keywordid=%llu",keywordid);
							exit_pthread(keywordid);
						}
					}
				}//在这个位置，检查两个子线程都没有完成，而调度导致两个子线程已经完成了，并且发出信号后退出了，然后主线程就会一直阻塞在下方
				if( ( (allfinish&0x01) != 0 && (allfinish&0x02) != 0 ) || ( nmatch > 0 ) ){
					//如果两个附属线程都完成了或者有一个线程检测出了敏感信息，则退出循环
					Print_t("allfinish=%d nmatch=%d i am quited",allfinish,nmatch);
					break ;
				}
				Print_t("i am waiting...");
				//wait
			//	usleep(10);
				pthread_cond_wait(&cond,&mutex);//在阻塞之前的信号不会被保留，即如果两个子线程都已经发过信号了，则这里可能永远阻塞
			}
			Print_t("break from while");
			pthread_mutex_unlock(&mutex);
			/*
			if( keywordid > 0 ){
				pthread_join(keywordid,NULL );
			}
			if( patternmatchid > 0 ){
				pthread_join(patternmatchid,NULL );
			}
			*/
			*pIfFound = nmatch;
			if( nmatch > 0 ){
				//记录审计信息
				Print_t("auditing...");
				if( keywordid > 0 && keywordfilter->finish == 1 && keywordfilter->resultnum > 0 ){
					for( i = 0 ; i< keywordfilter->resultnum ; ++i ){
						Print_t("policyname %s keyword in %d-%d",keywordfilter->policyname,keywordfilter->result[i][0] , keywordfilter->result[i][1]);
					}
				}
				if( patternmatchid > 0 && patternmatch->finish == 1 && patternmatch->matchnum > 0 ){
					for( i = 0 ; i< patternmatch->matchnum ; ++i ){
						Print_t("policyname %s pattern %s in %d-%d",patternmatch->policyname,patternmatch->patternname,patternmatch->result[i][0] , patternmatch->result[i][1]);
					}
				}
			}
			//释放子线程运行时的信息
			if( keywordid > 0 && keywordfilter != NULL ){
				freeKEYWORDFILTER( keywordfilter );
			}
			if( patternmatchid > 0 && patternmatch != NULL ){
				freePATTERNMATCH( patternmatch );
			}
			
			printf("filter end\n");
			(*env)->ReleaseStringUTFChars(env,jstr_ret,text);
			if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while ReleaseStringUTFChars\n");
				pthread_mutex_destroy(&mutex);
				pthread_cond_destroy(&cond);
                return -1;
            }
		
			if( (*jvm)->DetachCurrentThread(jvm) != JNI_OK ){
				printf("DetachCurrentThread error errno=%d\n",errno);
				pthread_mutex_destroy(&mutex);
				pthread_cond_destroy(&cond);
				return -1;
			}
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
			return 0 ;
}

void freeKEYWORDFILTER( KEYWORDFILTER * keywordfilter ){
	if( keywordfilter->resultnum > 0 ){
		freeResult(keywordfilter->result , keywordfilter->resultnum );
	}
	if( keywordfilter->policyname != NULL ){
		free(keywordfilter->policyname);
	}
	free(keywordfilter);
	return ;
}

void freePATTERNMATCH( PATTERNMATCH * patternmatch ){
	if( patternmatch->matchnum > 0 ){
		freeResult(patternmatch->result , patternmatch->matchnum );
	}
	if( patternmatch->patternname != NULL ){
		free(patternmatch->patternname);
	}
	if( patternmatch->policyname != NULL ){
		free(patternmatch->policyname);
	}
	free(patternmatch);
	return ;
}

//只释放MPOLICY结构体的空间，头指针需要单独释放，因为头指针可能不是malloc分配的
void freeMPOLICY(struct list_head * mpolicy_head ){
	MPOLICY * mpolicy = NULL ;
	struct list_head * lh = NULL ;
	struct list_head * ln = NULL ;
	//循环中会访问lh->next，释放空间时防止崩溃
	list_for_each_safe(lh,ln,mpolicy_head){
		mpolicy = list_entry(lh, MPOLICY, list);
		list_del(lh);
		free(mpolicy);
	}
	return ;
}

int updatePOLICY(){
	struct list_head policy_head ;
	pthread_rwlock_wrlock(&rwlock);
	policy_head.next = POLICY_LIST.next ;
	policy_head.prev = POLICY_LIST.prev ;
	POLICY_LIST.next = &POLICY_LIST ;
	POLICY_LIST.prev = &POLICY_LIST ;
	pthread_rwlock_unlock(&rwlock);
	free_policy_list(&policy_head);
	return 0 ;
}


int SWFilterInit(){

	JNIEnv* env = NULL ;
	env = create_vm() ;
	if( env == NULL ){
		return -1 ;
	}
	cls = create_class(env,"org/tika/secway/SecwayJni");
	if( cls == NULL ){
		return -1 ;
	}
	getContentID = get_method(env,cls,"getContent","([B)Ljava/lang/String;");
	if( getContentID == NULL ){
		return -1 ;
	}
	getFileContentID = get_method(env,cls,"getFileContent","([B)Ljava/lang/String;");
	if( getFileContentID == NULL ){
		return -1 ;
	}
	getMimeTypeID = get_method(env,cls,"getMimeType","([B)Ljava/lang/String;");
	if( getMimeTypeID == NULL ){
		return -1 ;
	}
	getFileMimeTypeID = get_method(env,cls,"getFileMimeType","([B)Ljava/lang/String;");
	if( getFileMimeTypeID == NULL ){
		return -1 ;
	}
	if( pthread_rwlock_init(&rwlock,NULL) != 0 ){
		return -1 ;
	}
	INIT_LIST_HEAD(&POLICY_LIST);
	return 0 ;
}

long long GetuTime(){
	struct timeval tv ;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000000+tv.tv_usec ;
}






