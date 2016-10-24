#include <jni.h>
#ifndef _SWJNI_H_
#define _SWJNI_H_

int getMimeType(char **pmime, int * pmimelen ,JNIEnv* env,jobject obj , jmethodID getMimeTypeId  ,jbyteArray jbytes,jstring *jstrmime);

int getContent(char **ptext, int * ptextlen ,JNIEnv* env,jobject obj , jmethodID getContentId  ,jbyteArray jbytes,jstring *jstrtext);

int getFileMimeType(char **pmime, int * pmimelen ,JNIEnv* env,jobject obj , jmethodID getFileMimeTypeID  ,jbyteArray jbytes,jstring *jstrmime);

int getFileContent(char **ptext, int * ptextlen ,JNIEnv* env,jobject obj , jmethodID getFileContentID  ,jbyteArray jbytes,jstring *jstrtext);

JNIEnv* create_vm();

jclass create_class(JNIEnv* env,char *className);

jmethodID get_method(JNIEnv* env,jclass cls,char *methodName,char *key);

jobject getInstance(JNIEnv* env, jclass obj_class);

#endif
