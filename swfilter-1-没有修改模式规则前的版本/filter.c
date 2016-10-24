#include <stdlib.h>
#include <string.h>
#include "filter.h"
#include "keywordfilter.h"
/*
int query_policy_info(MYSQL *sql, char *objname, int *key_num, char**keywords, int *pattern_num, char ** patternname ,char **pattern, int *filetype_num, char **filetype_list);


POLICY *GetPolicy(char *policyname);
POLICY *add_policy_into_policylist(POLICY *p, int *ret_code);
*/
//global vars
/*
MYSQL *sql ;
struct list_head POLICY_LIST;
*/
void addchar( char * keyword , int * key_num , char *** keywords ){
	Print_t("");
	char * tmp = NULL ;
	int len = strlen(keyword);
	char ** tmp1 = NULL ;
	tmp = (char *)malloc(len+1);
	strncpy(tmp,keyword,len+1);
	Print_t("key_num=%d",*key_num);
	tmp1 = (char **)realloc(*keywords,sizeof(char *)*(*key_num+1));
	*keywords = tmp1 ;
	(*keywords)[(*key_num)++] = tmp ;
	Print_t("");
}
int query_policy_info(MYSQL *sql, char *objname, int *key_num, char***keywords, int *pattern_num, char *** patternname ,char ***pattern, int *filetype_num, char ***filetype_list){
	
	char keyword[256] ;
	Print_t("");
	if( strcmp(objname , "test1") == 0 ){
		Print_t("");
		strncpy(keyword , "*", 256);
		addchar(keyword , filetype_num , filetype_list );
		
		strncpy(keyword , "thanks",256);
		addchar(keyword,key_num , keywords);
		strncpy(keyword , "little",256);
		addchar(keyword,key_num , keywords);
		
		strncpy(keyword , "account",256);
		addchar(keyword,pattern_num , patternname);
	//	strncpy(keyword , "\b(?!\b0{6,13}\b)\d{6,13}\b",256);
	//	strncpy(keyword , "\\b(?!\\b0{6,13}\\b)\\d{6,13}\\b",256);
		strncpy(keyword , "[1-9]\\{1\\}[0-9]\\{6,10\\}",256);
		--*pattern_num;
		addchar(keyword, pattern_num , pattern);
		
		strncpy(keyword , "ipaddr",256);
		addchar(keyword,pattern_num , patternname);
	//	strncpy(keyword , "((2[0-4]\d|25[0-5]|[01]?\d\d?)\.){3}(2[0-4]\d|25[0-5]|[01]?\d\d?)",256);
		strncpy(keyword , "((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)",256);
		--*pattern_num;
		addchar(keyword, pattern_num , pattern);

	}
	else if ( strcmp(objname , "test2") == 0 ){
		strncpy(keyword , "*", 256);
		addchar(keyword , filetype_num , filetype_list );
		Print_t("");
		strncpy(keyword , "bigger",256);
		addchar(keyword,key_num , keywords);
		strncpy(keyword , "country",256);
		addchar(keyword,key_num , keywords);
		
		strncpy(keyword , "phonenumber",256);
		addchar(keyword,pattern_num , patternname);
	//	strncpy(keyword , "((\(\d{2,3}\))|(\d{3}\-))?(\(0\d{2,3}\)|0\d{2,3}-)?[1-9]\d{6,7}(\-\d{1,4})?",256);
		strncpy(keyword , "((\\(\\d{2,3}\\))|(\\d{3}\\-))?(\\(0\\d{2,3}\\)|0\\d{2,3}-)?[1-9]\\d{6,7}(\\-\\d{1,4})?",256);
		--*pattern_num;
		addchar(keyword, pattern_num , pattern);
		
		strncpy(keyword , "cellphonenumber",256);
		addchar(keyword,pattern_num , patternname);
	//	strncpy(keyword , "((\(\d{2,3}\))|(\d{3}\-))?13\d{9}",256);
		strncpy(keyword , "((\\(\\d{2,3}\\))|(\\d{3}\\-))?13\\d{9}",256);
		--*pattern_num;
		addchar(keyword, pattern_num , pattern);
	}
	
	return 0 ;
}
POLICY *add_policy_into_policylist(POLICY *p, int *ret_code)
{
	if( p == NULL){
		*ret_code = ERR_NULL_POINTER;
		return NULL;
	}
	list_add_tail(&(p->list), &POLICY_LIST);
	*ret_code = 0;
	return p;
}

void free_policy_list(struct list_head * policy_head )
{
	struct list_head *lp = NULL, *tlp = NULL;
	POLICY *policy = NULL;

	list_for_each_safe(lp, tlp, policy_head ){
		policy = list_entry(lp, POLICY, list);
		list_del(lp);
		freePOLICY(policy);
	}
	return ;
}

POLICY * search_policy_in_policylist(char *policyname)
{
	struct list_head *lp = NULL;
	POLICY *p = NULL;

	if( policyname == NULL)
		return NULL;

	list_for_each(lp, &POLICY_LIST){
		p = list_entry(lp, POLICY, list);
		if( !strcmp(p->policyname, policyname)){
			return p;
		}
	}
	return NULL;
}

int exit_pthread(pthread_t pid)
{
	int s;
	void *res;
	Print_t("in exit_pthread killing pid=%u",pid);
	s = pthread_cancel(pid);
	if(s != 0){
		Print_t("kill pid=%u fail",pid);
		printf("pthread_cancel failed.\n");
	}
	Print_t("wait to join pid=%llu",pid);
	s = pthread_join(pid, &res);
	Print_t("after wait to join s=%d pid=%llu",s,pid);
	if(s != 0){
		printf("pthread_join failed.\n");
	}
	if( res == PTHREAD_CANCELED){
		Print_t("kill pid=%u success",pid);
		printf("pattern thread was canceled.\n");
		return 0;
	}
	else {
		Print_t("kill pid=%u fail",pid);
		printf("pattern thread was not canceled.\n");
		return -1;
	}
	return 0 ;
}
POLICY *GetPolicy(char *policyname)
{
	POLICY *p = NULL, *policy = NULL;
	int ret = 0;
	int key_num = 0, pattern_num = 0, filetype_num = 0;
	char **keywordlist = NULL, **patternlist = NULL, **filetypelist = NULL , **patternname = NULL ;
	Print_t("");
	if( policyname == NULL){
		return NULL;
	}
	Print_t("policyname=%s",policyname);
	p = search_policy_in_policylist( policyname);
	if( p ){
		Print_t("find %s in policy_list",policyname);
		return p;
	}
	Print_t("");
	//don't find 
	pthread_rwlock_unlock(&rwlock);//解除读锁
	ret = query_policy_info(sql, policyname, &key_num, &keywordlist, &pattern_num, &patternname , &patternlist, &filetype_num, &filetypelist);
	if( ret < 0 ){
		Print_t("");
		pthread_rwlock_rdlock(&rwlock);//重新加读锁
		return NULL;
	}
	Print_t("");
	ret = preprocess( &policy, policyname, filetype_num, filetypelist, key_num, keywordlist, pattern_num, patternname,patternlist);
	if( ret < 0){
		printf("Error: preprocess policy %s failed, errno:%d\n",policyname, ret);
		if(filetypelist)
			free(filetypelist);
		if(keywordlist)
			free(keywordlist);
		if(patternlist)
			free(patternlist);
		pthread_rwlock_rdlock(&rwlock);//重新加读锁
		return NULL;
	}
	pthread_rwlock_wrlock(&rwlock);//加上写锁，刚才的解除读锁已经防止了死锁
	add_policy_into_policylist(policy, &ret);	
	pthread_rwlock_unlock(&rwlock);//解除写锁
	//#这个位置可能有主线程获取写锁，并且是所有策略失效，从而刚才加入链表中的策略已经不存在了
	pthread_rwlock_rdlock(&rwlock);//重新加读锁，注意这里就算加了读锁，也不能保证策略已经加入链表中，因为可能被配置更新函数释放掉
	return GetPolicy(policyname);//重新读链表，以防止链表被主线程失效,当不断更新配置，会有不断递归导致栈溢出的风险
}
/*
POLICY *GetPolicy(char *policyname)
{
	POLICY *p = NULL, *policy = NULL;
	int ret = 0;
	int key_num = 0, pattern_num = 0, filetype_num = 0;
	char *keywordlist = NULL, *patternlist = NULL, filetypelist = NULL;

	if( policyname == NULL)
		return NULL;

	p = search_policy_in_policylist( policyname);
	if( p )
		return p;

	//don't find 
	ret = query_policy_info(sql, policyname, &key_num, &keywordlist, &pattern_num, &patternlist, &filetype_num, &filetypelist);
	if( ret < 0 ){
		return NULL;
	}
	ret = preprocess( &policy, policyname, file_num, &filetypelist, key_num, &keywordlist, pattern_num, &patternlist);
	if( ret < 0){
		printf("Error: preprocess policy %s failed, errno:%d\n",policyname, ret);
		if(filetypelist)
			free(filetypelist);
		if(keywordlist)
			free(keywordlist);
		if(patternlist)
			free(patternlist);
		return NULL;
	}
	add_policy_into_policylist(policy, &ret);	
	return policy;
}
*/
/*
int SWContentFilter(	
		const char * buffer , 
		int	length , 
		char * policynames , 
		SW_EVENT_INFO *sweventinfo,
		ConfigList * confighead,
		int * pIfFound 
		)
{
	MPOLICY *mp = NULL;
	char mime_type[MIME_TYPE_LEN] = NULL;
	int ret = 0;
	int nmatch = -1;
	char *outbuf = NULL;
	int outlen = 0;
	KEYWORDFILTER ;* keyfilter = NULL;
	PATTERNMATCH *pattern = NULL;
	pthread_t keythread;
	pthread_t patternthread;

	mp = GetMultiPolicy(policyname);
	if( mp == NULL){
		*pIfFound = 0;
		return 0;
	}
	ret = getMimeType(buffer, length, mime_type);
	if(ret < 0){
		printf("Error: getMimeType failed.\n");
	}
	else{
		nmatch = MatchMimeType(mp, mime_type);
	}
	if( !nmatch ){ //not matched 
		*pIfFound = 0;	
		return 0;
	}
	outbuf = (char *)malloc(length);
	if(outbuf == NULL){
		*pIfFound = 0;
		printf("Error: %s(), alloc memory failed.\n",__FUNCTION__);
		return -1;
	}
		
	ret = getPlainText(buffer, length, outbuf, &outlen);
	if(ret < 0 ){
		*pIfFound = 0;
		free(outbuf);
		printf("Error: get plaintext failed.\n");
		return -2;
	}

	//create keyword filter thread
	keyfilter = (KEYWORDFILTER *)malloc(sizeof(KEYWORDFILTER));
	if(keyfilter == NULL){
		free(outbuf);
		*pIfFound = 0;
		return -1;
	}
	keyfilter->finish = -1;
	keyfilter->ppid = pthread_self();
	keyfilter->buffer = outbuf;
	keyfilter->bufferlen = outlen;
	keyfilter->mpolicy = mp;
	if( pthread_create( &keythread, NULL,(void * ((*)(void *))) KeywordFilter,(void *)keyfilter)){
		printf("create keyword filter thread failed.\n");
		free(keyfilter);
		free(outbuf);
		*pIfFound = 0;
		return -1;
	}

	//create patter match thread
	pattern = (PATTERNMATCH *)malloc(sizeof(KEYWORDFILTER));
	if(pattern == NULL){
	//	free(outbuf);
	//	*pIfFound = 0;
	//	return -1;
	//  wait result of keyword filter 
	}
	pattern->finish = -1;
	pattern->ppid = pthread_self();
	pattern->buffer = outbuf;
	pattern->bufferlen = outlen;
	pattern->mpolicy = mp;

	if( pthread_create( &patternthread, NULL,(void * ((*)(void *))) PatternMatch,(void *)pattern)){
		printf("create keyword filter thread failed.\n");
		free(keyfilter);
		free(outbuf);
		*pIfFound = 0;
		return -1;
	}

	//signal(SIGUSR1, CheckFlag);
	//check two threads' finish flag
	while(1){
		if( keyfilter->finish == 1){ //have finished
			nmatch = keyfilter->resultnum;
			if(nmatch > 0){ //matched key words
				*pIfFound = nmatch;
				if(pattern->finish == 0){
					//kill pattern thread.
					exit_pthread(patternthread);
				}
			}
			break;
		}
		else if( pattern->finish == 1){
			nmatch = pattern->matchnum;
			if( nmatch > 0){
				*pIfFound = nmatch;
				if(keyfilter->finish == 0) {
					//kill keyfilter thread
					exit_pthread(keythread);
				}
			}
			break;
		}
		//wait
		usleep(10);
	}
	if( *pIfFound > 0 ){
		//record audit log
	}
	return 0;
}
*/
