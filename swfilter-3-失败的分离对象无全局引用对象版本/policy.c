#include <stdlib.h>
#include <string.h>
#include "policy.h"
#include "filter.h"
#include <mysql.h>

/*
int query_policy_info(MYSQL *sql, char *objname, int *key_num, char**keywords, int *pattern_num, char **pattern, int *filetype_num, char **filetype_list);


POLICY *GetPolicy(char *policyname);
POLICY *add_policy_into_policylist(POLICY *p, int *ret_code);
*/
//global vars
/*
MYSQL *sql ; */
extern struct list_head POLICY_LIST;
MYSQL *mydb;

MYSQL * db_connect()
{
        if(( mydb = mysql_init(NULL)) == NULL){
                printf("init mysql data failed\n");
                return NULL;
        }

        return mysql_real_connect(mydb,"192.168.28.30","mysql","123456","dlp",MYSQL_PORT,NULL,0);
	
}


int db_close(MYSQL *mysql)
{
        printf("-----db_close mysql connection.\n");
        mysql_close(mysql);
	return 0;
}


int db_lock(MYSQL *mydata,char *table, char *priority)
{
        char str[100];

        sprintf(str,"LOCK TABLES %s %s",table, priority);
        if(mysql_query(mydata,str))
                return -1;
        return 0;

}
int db_unlock(MYSQL *mydata)
{
        if(mysql_query(mydata,"UNLOCK TABLES"))
                return -1;
        return 0;
}
int query_keyword_info(MYSQL *mydb, char *query, int *num, char **buf)
{
	MYSQL_RES * result = NULL;
	MYSQL_ROW row = NULL;	
	int n = 0;	

	if(mysql_query(mydb ,query)){
		printf("mysql_query failed. %s",mysql_error(mydb));
		*num = 0;
             return -1;
	}   
	
	result = mysql_store_result(mydb);
	if( result ){
		*num = mysql_num_rows(result);
	
		printf("keyword num: %d\n",*num);
		*buf = (char *)malloc( (*num) * KEYWORD_LEN);
		if(*buf == NULL){
			printf("Error: malloc failed.\n");
			mysql_free_result(result);  
			*num = 0;
			return -1;
		}
		while((row = mysql_fetch_row(result))){		
			memcpy( *buf+ n*KEYWORD_LEN, row[0], KEYWORD_LEN);			
			printf("keyword: %s\n",row[0]);
			n++;
		}
		printf("keyword num: %d\n-----------------------------\n",*num);
		mysql_free_result(result);  
	}
	return 0;
}

int query_pattern_info(MYSQL *mydb, char *query, int *num, char **buf)	
{
 	 MYSQL_RES * result = NULL;
	 MYSQL_ROW row = NULL;
	 struct pattern_info *patinfo = NULL;
	 char sub_str[1024];
	 int expattern_num = 0, exinfo_len = 0;
	 char *expattern_info;
	  
	if(mysql_query(mydb ,query)){
		printf("mysql_query failed. %s",mysql_error(mydb));
		*num = 0;
             return -1;
	}   
	
	//result = mysql_use_result(mydb); //get one line , when data large
	result = mysql_store_result(mydb); //get all data, when data small
	if( result ) {
		*num = mysql_num_rows(result);
		printf("pattern num: %d\n",*num);
		*buf = (char *)malloc(*num *sizeof(struct pattern_info));
		if(*buf == NULL){
			printf("Error: malloc failed.\n");
			*num = 0;
			mysql_free_result(result);  
			return -1;
		}
		patinfo = (struct pattern_info *)(*buf);
		while((row = mysql_fetch_row(result))){			
			memset(patinfo, 0 , sizeof(struct pattern_info));			
			memcpy(patinfo->name, row[1], strlen(row[1]));
			memcpy(patinfo->pattern, row[4], strlen(row[4]));
			patinfo->exclude = atoi(row[5]);
			printf("pattern id: %s name: %s, exclude: %d\n",row[0],patinfo->name, patinfo->exclude);
			if(atoi(row[5])== 1){//?ų?ģʽ 
				exinfo_len = strlen(row[6]) ;
				if(exinfo_len){
					exinfo_len ++;
					patinfo->exinfo = (char *)malloc(exinfo_len);
					memset(patinfo->exinfo, 0, exinfo_len);
					memcpy(patinfo->exinfo, row[6], exinfo_len);					
				}
			}
			else if(atoi(row[5]) == 2){ //?ų?????
				//??ѯ?ų?????????Ϣ
				sprintf(sub_str,"select * from EXPATTERN_INFO where PatternObj_ID = %s",row[0]);
				query_keyword_info(mydb, sub_str, &expattern_num, &expattern_info);
				if(expattern_num > 0 && expattern_info){
					patinfo->exwords = expattern_info;
					patinfo->exwords_num = expattern_num ;
				}	
			}
			patinfo++;
		}	
		mysql_free_result(result);  
		printf("pattern num: %d\n-----------------------------\n",*num);
	}
	return 0;
}

int query_filetype_info(MYSQL *mydb, char *query, int *num, char **buf)
{
	MYSQL_RES * result = NULL;
        MYSQL_ROW row = NULL;	
	int n = 0;	
	  
	if(mysql_query(mydb ,query)){
		printf("mysql_query failed. %s",mysql_error(mydb));
		*num = 0;
             return -1;
	}   
	printf("filetype query\n");	
	result = mysql_store_result(mydb);
	if(result){
		printf("filetype query succ.\n");
		*num = mysql_num_rows(result);
		printf("filetype query num = %d.\n",*num);


		*buf = (char *)malloc(*num * MIMETYPE_LEN);
		if(*buf == NULL){
			printf("Error: malloc failed.\n");
			*num = 0;
			mysql_free_result(result);  
			return -1;
		}
		while((row = mysql_fetch_row(result))){		
			memcpy(*buf + n*MIMETYPE_LEN, row[3], MIMETYPE_LEN);		
			printf("filetype: %s\n",row[3]);
			n++;		
		}
		printf("query filetype info num: %d\n----------------------\n",n);
		mysql_free_result(result);  
	}
	return 0;
}

#if 1
int query_policy_info123(MYSQL *mydb, char *objname, char *attrs, int *key_num, char**keywords, int *pattern_num, char  **pattern_list, int *filetype_num, char **filetype_list)
{
	char str[2048];
	MYSQL_RES * result = NULL;
        MYSQL_ROW row = NULL;	
	int n = 0;	

	bzero(str, sizeof(str));
	n = sprintf(str,"select attrs from POLICY_OBJ where Name='%s'", objname);
	if(mysql_query(mydb ,str)){
		printf("mysql_query failed. %s",mysql_error(mydb));
		return -1;
	}
	result = mysql_store_result(mydb);
	if( result){
		row = mysql_fetch_row(result);
		if( row ){
			if( row[0] == NULL){
				memset(attrs, 0, 8);
				attrs[0]= 1;
			}
			else
				memcpy(attrs,row[0],8);
		}else{
			memset(attrs, 0, 8);
			attrs[0]= 1;
		}
		mysql_free_result(result);
	}
	printf("\n");
	//query keyword info by objname
	n = sprintf(str,"select distinct KEYWORD_INFO.info from KEYWORD_OBJ left join( \
	    select res_Type, res_ID, res_Attr, res_Option, POLICY_OBJ.id as id,name,sensitivity,comments \
	    from POLICY_RES,POLICY_OBJ where POLICY_RES.policy_ID=POLICY_OBJ.id and POLICY_OBJ.Name='%s')policy \
	    on policy.res_ID=KEYWORD_OBJ.id \
	    left join KEYWORD_INFO \
	    on \
	    KEYWORD_INFO.keywordObj_ID=KEYWORD_OBJ.id \
	    where 1=1\
	    and policy.res_Type=1 group by KEYWORD_INFO.info", objname);
	query_keyword_info(mydb, str, key_num, keywords);
	
	//query pattern info by objname
	n = sprintf(str,"select * from PATTERN_OBJ left join( \
	    select POLICY_RES.id as policy_id ,res_Type,res_ID,res_Attr,res_Option,POLICY_OBJ.id as id,name,sensitivity,comments  \
	    from POLICY_RES,POLICY_OBJ where POLICY_RES.policy_ID=POLICY_OBJ.id and POLICY_OBJ.Name='%s')policy \
	    on policy.res_ID=PATTERN_OBJ.id \
	    where 1=1 \
	    and policy.res_Type=2",objname);

	query_pattern_info( mydb, str, pattern_num, pattern_list);

	//query filetype info by objname
	n = sprintf(str,"select distinct * from FILETYPE_INFO,FILETYPE_OBJ left join \
	     (select POLICY_RES.id as policy_id ,res_Type,res_ID,res_Attr,res_Option,POLICY_OBJ.id as id,name,sensitivity,comments  \
	     from POLICY_RES,POLICY_OBJ \
	     where POLICY_RES.policy_ID= POLICY_OBJ.id \
	     and POLICY_OBJ.name='%s')policy \
	     on policy.res_ID=FILETYPE_OBJ.id \
	     left join FILETYPE_ASSIGN \
	     on FILETYPE_ASSIGN.fileObj_ID=FILETYPE_OBJ.id \
             where 1=1 \
	     and policy.res_Type=3 \
	     and FILETYPE_ASSIGN.fileObj_ID=FILETYPE_OBJ.id \
	     and FILETYPE_ASSIGN.fileType_ID=FILETYPE_INFO.ID \
	     group by suffix",objname);
	query_filetype_info( mydb, str, filetype_num, filetype_list);
	
	return 0;
}


void addchar( char * keyword , int * key_num , char ** keywords ,int length ){
	Print_t("");
	*keywords = (char *)realloc(*keywords , (*key_num+1)*length);
	Print_t("");
	strncpy( (*keywords)+(*key_num *length),keyword,length);
	Print_t("");
	++*key_num;
	Print_t("");
}
void addpattern1( int * num , struct pattern_info ** pattinfo ,char * name,char *info){
	*pattinfo = (struct pattern_info *)realloc( *pattinfo,sizeof(struct pattern_info)*(*num+1));
	strncpy((*pattinfo)[*num].name , name , 128 );
	strncpy((*pattinfo)[*num].pattern , info , 256 );
	(*pattinfo)[*num].exclude = 1 ;
	(*pattinfo)[*num].exinfo = (char *)malloc(sizeof(char)*256);
	strncpy((*pattinfo)[*num].exinfo,"exclude",256);
	++*num ;
}
void addpattern2( int * num , struct pattern_info ** pattinfo ,char * name,char *info){
	Print_t("");
	*pattinfo = (struct pattern_info *)realloc( *pattinfo,sizeof(struct pattern_info)*(*num+1));
	(*pattinfo)[*num].exwords = NULL ;
	memset( &(*pattinfo[*num]), 0 , sizeof(struct pattern_info) );
	Print_t("");
	strncpy((*pattinfo)[*num].name , name , 128 );
	Print_t("");
	strncpy((*pattinfo)[*num].pattern , info , 256 );
	Print_t("");
	(*pattinfo)[*num].exclude = 2 ;
	Print_t("");
	(*pattinfo)[*num].exinfo = NULL ;
	Print_t("");
	addchar("exctest1", &((*pattinfo)[*num].exwords_num) , &((*pattinfo)[*num].exwords),KEYWORD_LEN);
	Print_t("");
	addchar("exctest2", &((*pattinfo)[*num].exwords_num) , &((*pattinfo)[*num].exwords),KEYWORD_LEN);
	Print_t("");
	addchar("exctest3", &((*pattinfo)[*num].exwords_num) , &((*pattinfo)[*num].exwords),KEYWORD_LEN);
	Print_t("");
	addchar("exctest4", &((*pattinfo)[*num].exwords_num) , &((*pattinfo)[*num].exwords),KEYWORD_LEN);
	Print_t("");
	++*num ;
	Print_t("");
}
int query_policy_info(MYSQL *mydb, char *objname, char * attrs ,int *key_num, char**keywords, int *pattern_num, char  **pattern_list, int *filetype_num, char **filetype_list){
	char keyword[256] ;
	Print_t("");
	if( strcmp(objname , "test1") == 0 ){
		Print_t("");
		strncpy(keyword , "*", 256);
		addchar(keyword , filetype_num , filetype_list ,MIMETYPE_LEN);
		Print_t("");
		strncpy(keyword , "thanks",256);
		addchar(keyword,key_num , keywords,KEYWORD_LEN);
		strncpy(keyword , "little",256);
		addchar(keyword,key_num , keywords,KEYWORD_LEN);
		Print_t("");
		addpattern2(pattern_num,(struct pattern_info **)pattern_list ,"patterntest","pattern" );
		Print_t("");
		attrs[0]=2;
		Print_t("");
		attrs[1]=1;
		Print_t("");
	}
	else if ( strcmp(objname , "test2") == 0 ){
		strncpy(keyword , "*", 256);
		addchar(keyword , filetype_num , filetype_list ,MIMETYPE_LEN);
		Print_t("");
		strncpy(keyword , "bigger",256);
		addchar(keyword,key_num , keywords,KEYWORD_LEN);
		strncpy(keyword , "country",256);
		addchar(keyword,key_num , keywords,KEYWORD_LEN);
		
	}
	
	return 0 ;
}
#else
int query_policy_info(MYSQL *mydb, char *objname, char * attrs ,int *key_num, char**keywords, int *pattern_num, char  **pattern_list, int *filetype_num, char **filetype_list)
{
	MYSQL_RES * result = NULL;
        MYSQL_ROW row = NULL;
	char str[1024],sub_str[2048];
	int n = 0, knum=0,pnum=0,fnum=0;
	int line = 0;
	int res_type, res_id;
	char kres_id[256];
	char pres_id[256];
	char fres_id[256];
	int i=0,j=0,k=0;
	  	

	bzero(str, sizeof(str));
	//query policy obj's attrs
	n = sprintf(str,"select attrs from POLICY_OBJ where Name='%s' ", objname);
	if(mysql_query(mydb ,str)){
		printf("mysql_query failed. %s",mysql_error(mydb));
		return;
	}
	result = mysql_store_result(mydb);
	row = mysql_fetch_row(result);
	if( row ){
		memcpy(attrs,row[0],8);
	}else
		memset(attrs, 0, 8);
	 
	n = sprintf(str,"select * from POLICY_RES,POLICY_OBJ where POLICY_RES.Policy_ID=POLICY_OBJ.ID and POLICY_OBJ.Name='%s' ", objname);
	if(mysql_query(mydb ,str)){
		printf("mysql_query failed. %s",mysql_error(mydb));
                return -1;
        }   
	result = mysql_store_result(mydb);
	printf("----mysql_store_result finish-------\n");
	if( result ){
		/*
		printf("mysql query succ.\n");
		line = mysql_num_rows(result);
		printf("find affected line %d",line);
	        if(line == 0){
		        printf("don't find selected lines.");
			return 0;
	        }
	        else if(line< 0){
		        printf("query_policy_info failed.");
			return -1;
	        }

		*/	
	        printf("---query_policy_info---\n");
	  	
		while((row = mysql_fetch_row(result))){
	         	// do something with data
		 	res_type = atoi(row[1]);
			res_id = atoi(row[2]);
			printf(" res_type:%d, res_id =%d\n",res_type, res_id);
			switch (res_type){
			case RES_TYPE_KEYWORD:						
				if(i == 0)
					knum = sprintf(kres_id, "%d",res_id);
				else
					knum+=sprintf(kres_id+knum, ",%d",res_id);
				i++;
				break;
				
			case RES_TYPE_PATTERN:
				if(j == 0)
					pnum = sprintf(pres_id, "%d",res_id);
				else
					pnum += sprintf(pres_id + pnum, ",%d",res_id);			
				j++;
				break;

			case RES_TYPE_FILETYPE:
				printf("----filetype.\n");
				if(k == 0)
					fnum = sprintf(fres_id, "%d",res_id);
				else
					fnum += sprintf(fres_id + fnum, ",%d",res_id);					
				k++;	
				printf("fres_id:%s\n",fres_id);
				break;
			}
		}
		//һ?????Զ????п????ж????ؼ??ʶ??������???ģʽʶ??????.....
		if(i > 0){
			bzero(sub_str, sizeof(sub_str));
			sprintf(sub_str, "select distinct Info from KEYWORD_INFO where keywordObj_ID in (");
			strcat(sub_str, kres_id);
			strcat(sub_str,") group by KEYWORD_INFO.info");
			printf("keyword query  sub_str:%s\n",sub_str);
			query_keyword_info(mydb, sub_str, key_num, keywords);
			printf("query keyword num: %d\n",*key_num);
		}
		if(j > 0){
			
			bzero(sub_str, sizeof(sub_str));
			sprintf(sub_str, "select * from PATTERN_OBJ where Id in (");
			strcat(sub_str, pres_id);
			strcat(sub_str,")");
			printf("pattern query  sub_str:%s\n",sub_str);
			
			query_pattern_info( mydb, sub_str, pattern_num, pattern_list);
		}
		if(k > 0){
			bzero(sub_str, sizeof(sub_str));
			sprintf(sub_str,"select * from FILETYPE_INFO,FILETYPE_ASSIGN where FILETYPE_INFO.Id = FILETYPE_ASSIGN.FileType_Id and FILETYPE_ASSIGN.FileObj_Id in (");
			strcat(sub_str, fres_id);
			strcat(sub_str,")");
			printf("filetype query  sub_str:%s\n",sub_str);
			query_filetype_info( mydb, sub_str, filetype_num, filetype_list);
		}
		
		mysql_free_result(result);  
	}
	else
		printf("mysql_query failed.\n");
	return 0;
}
#endif

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

//只释放MPOLICY结构体的空间，头指针需要单独释放，因为头指针可能不是malloc分配的

POLICY *GetPolicy(char *policyname, int count )
{
	POLICY *p = NULL, *policy = NULL;
	int ret = 0;
	int key_num = 0, pattern_num = 0, filetype_num = 0;
	char attrs[8];
	char *keywordlist = NULL, *patternlist = NULL, *filetypelist = NULL;
	

	if( policyname == NULL || count > 5 )
		return NULL;

	p = search_policy_in_policylist( policyname);
	if( p )
		return p;

	bzero(attrs, sizeof(attrs));
	//don't find 
	pthread_rwlock_unlock(&rwlock);//????????
	ret = query_policy_info(mydb, policyname,attrs, &key_num, &keywordlist, &pattern_num, &patternlist, &filetype_num, &filetypelist);
	if( ret < 0 ){
		pthread_rwlock_rdlock(&rwlock);//???¼Ӷ???
		return NULL;
	}
	ret = preprocess( &policy, policyname,attrs, filetype_num, filetypelist, key_num, keywordlist, pattern_num, (struct pattern_info *)patternlist);
	if( ret < 0){
		printf("Error: preprocess policy %s failed, errno:%d\n",policyname, ret);
		if(filetypelist)
			free(filetypelist);
		if(keywordlist)
			free(keywordlist);
		if(patternlist && pattern_num > 0)
			freePatternInfo(patternlist,pattern_num);
		pthread_rwlock_rdlock(&rwlock);//???¼Ӷ???
		return NULL;
	}
	pthread_rwlock_wrlock(&rwlock);//????д?????ղŵĽ????????Ѿ???ֹ??????
	add_policy_into_policylist(policy, &ret);	
	pthread_rwlock_unlock(&rwlock); //????д??

	//#????λ?ÿ????????̻߳?ȡд?�����???????в???ʧЧ???Ӷ??ղż???��???еĲ????Ѿ?????????
	pthread_rwlock_rdlock(&rwlock);//???¼Ӷ?????ע?????????????˶?????Ҳ???ܱ?֤?????Ѿ?????t???У???Ϊ???ܱ????ø??º????ͷŵ?
	return GetPolicy(policyname,count+1 ); //???¶?t?���?Է?ֹt?���???߳?ʧЧ,?????ϸ??????ã????в??ϵݹ鵼??ջ?????ķ???
}

struct list_head * getMPOLICY( char * policynames )
{
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
		policy = GetPolicy(policyname,0);
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
		policy = GetPolicy(policyname,0);
		if( policy == NULL ){
			free(mpolicy);
			return mpolicy_head ;
		}
		mpolicy->policy = policy ;
		_list_add( &mpolicy->list , mpolicy_head );
	}
	return mpolicy_head ;
}
void freePOLICY( POLICY * policy )
{
	int keywordnum = 0 ;
	unsigned char * keyword = NULL ;
	if( policy == NULL ){
		return ;
	}
	if( policy->policyname ){
		free( policy->policyname );
		policy->policyname = NULL ;
	}
	if( policy->filetype ){
		free(policy->filetype);
		policy->filetype = NULL ;
	}
	if( policy->wm ){
		keywordnum = policy->wm->keywordnum ;
		keyword = policy->wm->keyword ;
		if( keyword ){
			free(keyword);
			policy->wm->keyword = NULL ;
		}
		freeWM( policy->wm );
		policy->wm = NULL ;
	}
	
	if( policy->pattinfo ){
		freePatternInfo(policy->pattinfo,policy->patternnum);
	}
	if( policy->prepatt ){
		freePrePatt(policy->prepatt);
		free(policy->prepatt);
	}
	free(policy);
	return ;
}
/*
void freePOLICY( POLICY * policy )
{
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
*/

void freeMPOLICY(struct list_head * mpolicy_head )
{
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

int updatePOLICY()
{
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
