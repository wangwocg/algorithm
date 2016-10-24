#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jni.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "mysql/mysql.h"
#include "tos_list.h"
#include "audit.h"
/**
*初始化jvm
*/
typedef int bool;
#define false 0
#define true  1

extern int SWContentFilter( const char * buffer , int bufferlen, char * policynames , SW_EVENT_INFO *sweventinfo,int * pIfFound );

extern int SWFileFilter( const char * path , char * policynames , SW_EVENT_INFO *sweventinfo,int * pIfFound );

extern int SWFilterInit();

JavaVM * jvm;
jclass cls ;
jmethodID getContentID ;
jmethodID getMimeTypeID ;
jmethodID getFileContentID ;
jmethodID getFileMimeTypeID ;
JNIEnv* env ;
pthread_rwlock_t rwlock ;	//策略信息头结点policyhead的互斥锁
struct list_head POLICY_LIST ;		//指向策略信息链表的头结点
MYSQL * sql ;


int parser( char * filename ) {
		struct stat fileinfo ;
		int len = 0 ,length = 0 , ret = 0 ;
		memset(&fileinfo,0,sizeof(fileinfo));
		char policynames[256] = "test1;";
		SW_EVENT_INFO sweventinfo ;
		int found = 0 ;
		printf("filename=%s\n",filename);
		if(stat(filename,&fileinfo) < 0 ){
			printf("stat file error errno=%d\n",errno);
			return 0 ;
		}
		int filelen = fileinfo.st_size ;
		printf("filelen=%d\n",filelen);
		if( filelen <= 0 ){
			printf("file length is 0\n");
			return 0 ;
		}
		char * buffer = (char *)malloc(sizeof(char)*(filelen+1));
		if( buffer == NULL ){
			printf("malloc error errno=%d\n",errno);
			return 0 ;
		}
		FILE * fp = NULL ;
		fp = fopen(filename,"r");
		if( fp == NULL ){
			printf("open file error errno=%d\n",errno);
			return 0 ;
		}
		fseek(fp,0,0);
		while( !feof(fp) ){
			len = fread(buffer+length,1,filelen+1,fp);
			if( len <= 0 ){
				printf("read file error len=%d errno=%d\n",len,errno);
				free(buffer);
				return 0 ;
			}
			length += len ;
			filelen -= len ;
			
		}
		printf("length=%d\n",length);
        ret = SWContentFilter( buffer , length , policynames , &sweventinfo,&found );
		free(buffer);
		return 0 ;
}
void * runpath( void * p ){
	int ret = 0 ;
	if( p == NULL ){
		return ;
	}
	char policynames[256]= "test1;";
	SW_EVENT_INFO sweventinfo ;
	int found = 0 ;
	ret = SWFileFilter( p , policynames , &sweventinfo, &found );
	printf("filename:%s parser finish\n",p);
	return ;
}

void * runbuffer( void * p ){
	int ret = 0 ;
	if( p == NULL ){
		return ;
	}
	parser(p);
	return ;
}


void main(int argc,char * argv[]){
	char ** pp = NULL ;
	char * defaultfile = "test.txt";
	int filenum = 1 , i = 0 ;
	pthread_t * thread = NULL ;
	if( argc <= 1 ){
		filenum = 1 ;
		pp = &defaultfile ;
	}
	else{
		filenum = argc - 1 ;
		pp = argv + 1 ;
	}
	
	thread = (pthread_t *)malloc( sizeof(pthread_t)*filenum );
	if( SWFilterInit() != 0 ){
		return ;
	}
	for(i=0;i<filenum;i++ ){
		pthread_create(thread+i,NULL,runpath,pp[i]);
	}
	for( i=0;i<filenum;i++){
		pthread_join(thread[i],NULL);
	}
	return ;
}



