#include "swjni.h"
extern  JavaVM* jvm;
/*
int AttachCurrentThread(JavaVM * jvm , JNIEnv* env ){
	if( jvm == NULL ){
		return -1 ;
	}
	if( (*jvm)->AttachCurrentThread(jvm,&env,NULL) != JNI_OK ){
		printf("AttachCurrentThread error errno=%d\n",errno);
		return -1;
	}
	if((*env)->ExceptionOccurred(env)){
        printf("in invokebyte have exception while AttachCurrentThread\n");
		return -1;
    }
	return 0 ;
}
*/
int getMimeType(char **pmime, int * pmimelen ,JNIEnv* env,jobject obj , jmethodID getMimeTypeId  ,jbyteArray jbytes,jstring *jstrmime){
	jstring jstrmime_ret =  (*env)->CallObjectMethod(env,obj,getMimeTypeId,jbytes);//传入参数调用方法
		if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while CallObjectMethod\n");
                return -1;
        }
		if( jstrmime_ret == NULL ){
			printf("get mimetype error\n");
			
			return -1;
		}
		
		char * mime = (char *)(*env)->GetStringUTFChars(env,jstrmime_ret,0);
		if((*env)->ExceptionOccurred(env)){
        	        printf("in invokebyte have exception while GetStringUTFChars\n");
            		return -1;
        }
		
		jsize mimelen = (*env)->GetStringUTFLength(env,jstrmime_ret);
		if((*env)->ExceptionOccurred(env)){
            printf("in invokebyte have exception while GetStringUTFLength\n");
			(*env)->ReleaseStringUTFChars(env,jstrmime_ret,mime);
			return -1;
        }
		*pmime = mime ;
		*pmimelen = mimelen ;
		*jstrmime = jstrmime_ret ;
		return 0 ;
}
int getContent(char **ptext, int * ptextlen ,JNIEnv* env,jobject obj , jmethodID getContentId  ,jbyteArray jbytes,jstring *jstrtext){
	jstring jstr_ret =  (*env)->CallObjectMethod(env,obj,getContentId,jbytes);//传入参数调用方法
		if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while CallObjectMethod\n");
                return -1;
        }
		if( jstr_ret ==NULL){
			printf("jstr_ret is null\n");
			return -1;
		}
		
     			char * ret = (char *)(*env)->GetStringUTFChars(env,jstr_ret,0);
			if((*env)->ExceptionOccurred(env)){
         		        printf("in invokebyte have exception while GetStringUTFChars\n");
                		return -1;
        		}
			jsize len = (*env)->GetStringUTFLength(env,jstr_ret);
			if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while GetStringUTFLength\n");
				(*env)->ReleaseStringUTFChars(env,jstr_ret,ret);
                return -1;
            }
	//		printf("len=%d the result is:%s\n",len,ret);//打印出调用的结果
			*ptext = ret ;
			*ptextlen = len ;
			*jstrtext = jstr_ret ;
			return 0 ;
}

int getFileMimeType(char **pmime, int * pmimelen ,JNIEnv* env,jobject obj , jmethodID getFileMimeTypeID  ,jbyteArray jbytes,jstring *jstrmime){
	jstring jstrmime_ret =  (*env)->CallObjectMethod(env,obj,getFileMimeTypeID,jbytes);//传入参数调用方法
		if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while CallObjectMethod\n");
                return -1;
        }
		if( jstrmime_ret == NULL ){
			printf("get mimetype error\n");
			
			return -1;
		}
		
		char * mime = (char *)(*env)->GetStringUTFChars(env,jstrmime_ret,0);
		if((*env)->ExceptionOccurred(env)){
        	        printf("in invokebyte have exception while GetStringUTFChars\n");
            		return -1;
        }
		
		jsize mimelen = (*env)->GetStringUTFLength(env,jstrmime_ret);
		if((*env)->ExceptionOccurred(env)){
            printf("in invokebyte have exception while GetStringUTFLength\n");
			(*env)->ReleaseStringUTFChars(env,jstrmime_ret,mime);
			return -1;
        }
		*pmime = mime ;
		*pmimelen = mimelen ;
		*jstrmime = jstrmime_ret ;
		return 0 ;
}
int getFileContent(char **ptext, int * ptextlen ,JNIEnv* env,jobject obj , jmethodID getFileContentID  ,jbyteArray jbytes,jstring *jstrtext){
	jstring jstr_ret =  (*env)->CallObjectMethod(env,obj,getFileContentID,jbytes);//传入参数调用方法
		if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while CallObjectMethod\n");
                return -1;
        }
		if( jstr_ret ==NULL){
			printf("jstr_ret is null\n");
			return -1;
		}
		
     			char * ret = (char *)(*env)->GetStringUTFChars(env,jstr_ret,0);
			if((*env)->ExceptionOccurred(env)){
         		        printf("in invokebyte have exception while GetStringUTFChars\n");
                		return -1;
        		}
			jsize len = (*env)->GetStringUTFLength(env,jstr_ret);
			if((*env)->ExceptionOccurred(env)){
                printf("in invokebyte have exception while GetStringUTFLength\n");
				(*env)->ReleaseStringUTFChars(env,jstr_ret,ret);
                return -1;
            }
		//	printf("len=%d the result is:%s\n",len,ret);//打印出调用的结果
			*ptext = ret ;
			*ptextlen = len ;
			*jstrtext = jstr_ret ;
			return 0 ;
}

JNIEnv* create_vm() {
      //  JavaVM* jvm;
        JNIEnv* env = NULL ;
        JavaVMInitArgs args;
        JavaVMOption options[2];
        args.version = JNI_VERSION_1_6;
        args.nOptions = 2;
        options[0].optionString = "-Djava.class.path=./tika-secway-1.12-jar-release.jar";
	
	options[1].optionString = "-Xms4096m";
//	options[1].optionString = "-verbose:class";

        args.options = options;
        args.ignoreUnrecognized = JNI_FALSE;
        JNI_CreateJavaVM(&jvm, (void **)&env, &args);
		if( env == NULL ){
			return NULL ;
		}
	if((*env)->ExceptionOccurred(env)){
                printf("in create_vm have exception while JNI_CreateJavaVM\n");
                return NULL;
	}
        return env;
}

/**
* 根据全限定类名来获取类的定义
*/
jclass create_class(JNIEnv* env,char *className){
        jclass cls = (*env)->FindClass(env,className);
	if((*env)->ExceptionOccurred(env)){
                printf("in create_class have exception while FindClass\n");
                return NULL ;
        }
        if(cls == NULL){
                printf("class-[%s] find error\n",className);
                return NULL ;
        }
        return cls;
}

/**
*通过无参构造函数来获取对应类的实例
*/
jobject getInstance(JNIEnv* env, jclass obj_class)
{ 
        jmethodID construction_id = (*env)->GetMethodID(env,obj_class, "<init>", "()V");
	if((*env)->ExceptionOccurred(env) || construction_id == NULL ){
                printf("in getInstance have exception while GetMethodID\n");
                return NULL ;
        }
        jobject obj = (*env)->NewObject(env,obj_class, construction_id);
	if((*env)->ExceptionOccurred(env)){
                printf("in getInstance have exception while NewObject\n");
                return NULL ;
        }
        if(obj == NULL ){
                printf("java class instance failed\n");
                return NULL ;
        }
        return obj; 
}


/**
* 根据类\方法名\返回值\参数获取类中对应的方法定义
*/
jmethodID get_method(JNIEnv* env,jclass cls,char *methodName,char *key){
        jmethodID mid = (*env)->GetMethodID(env,cls,methodName,key);
		if((*env)->ExceptionOccurred(env)){
                printf("in get_method have exception while GetMethodID\n");
                return NULL ;
        }
        if(mid == NULL ){
                printf("method-%s is not found\n",methodName);
                return NULL ;
        }
        return mid;
}


/*
int ExceptionOccurred(JNIEnv* env){
	if((*env)->ExceptionOccurred(env)){
        printf("in invokebyte have exception while NewByteArray\n");
        return -1;
    }
	return 0 ;
}
*/
