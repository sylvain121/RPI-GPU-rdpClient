#pragma once

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_desktopStreamer_VideoStreamer */

#ifndef _Included_com_desktopStreamer_VideoStreamer
#define _Included_com_desktopStreamer_VideoStreamer
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_desktopStreamer_VideoStreamer
 * Method:    stream
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_desktopStreamer_VideoStreamer_stream(JNIEnv *, jobject);

void streamBytes(uint8_t *data, int size);

#ifdef __cplusplus
}
#endif

#endif
