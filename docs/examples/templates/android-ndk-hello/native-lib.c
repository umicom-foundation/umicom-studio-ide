/*---------------------------------------------------------------------------
* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
*---------------------------------------------------------------------------*/
#include <jni.h>
JNIEXPORT jstring JNICALL
Java_com_umicom_hello_MainActivity_stringFromJNI(JNIEnv* env, jobject thiz){
  (void)thiz;
  return (*env)->NewStringUTF(env, "Hello from Android NDK (C) in Umicom Studio IDE!");
}
