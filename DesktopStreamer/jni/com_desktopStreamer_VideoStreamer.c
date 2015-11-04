#include <iostream>
#include "utils.h"
#include "jni.h"
#include "video_streamer.h"
#include "com_desktopStreamer_VideoStreamer.h"
/* Header for class com_desktopStreamer_VideoStreamer */

JavaVM * g_vm;
jobject g_obj;
jmethodID g_mid;

JNIEXPORT void JNICALL Java_com_desktopStreamer_VideoStreamer_stream(JNIEnv *env, jobject obj)
{
    env->GetJavaVM(&g_vm);
    std::cout << "Starting native stream" << std::endl;
    /* get stream callback  method*/

    g_obj = env->NewGlobalRef(obj);
    jclass g_clazz = env->GetObjectClass(g_obj);
    if(g_clazz == NULL) {
     std::cout << "Failed to find class" << std::endl;
    }
    g_mid = env->GetMethodID(g_clazz, "streamByte", "([B)V");
    if(g_mid == NULL) {
        std::cout << "Unable to get method ref" << std::endl;
    }

    /* start stream */
    start_streaming();
}



void streamBytes(uint8_t *data, int size)
{

	JNIEnv * g_env;
    	// double check it's all ok
    	int getEnvStat = g_vm->GetEnv((void **)&g_env, JNI_VERSION_1_6);
    	if (getEnvStat == JNI_EDETACHED) {
    		std::cout << "GetEnv: not attached" << std::endl;
    		if (g_vm->AttachCurrentThread((void **) &g_env, NULL) != 0) {
    			std::cout << "Failed to attach" << std::endl;
    		}
    	} else if (getEnvStat == JNI_OK) {
    		//
    	} else if (getEnvStat == JNI_EVERSION) {
    		std::cout << "GetEnv: version not supported" << std::endl;
    	}

    	    jbyteArray packetBuffer = g_env->NewByteArray(size);
    	    g_env->SetByteArrayRegion(packetBuffer, 0, size, (jbyte*) data);
        	g_env->CallVoidMethod(g_obj, g_mid, packetBuffer);

    	if (g_env->ExceptionCheck()) {
    		g_env->ExceptionDescribe();
    	}

    	g_vm->DetachCurrentThread();

}



