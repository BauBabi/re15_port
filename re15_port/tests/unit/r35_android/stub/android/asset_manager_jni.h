#ifndef H_AMJ_H
#define H_AMJ_H
#include <jni.h>
#include <android/asset_manager.h>
AAssetManager *AAssetManager_fromJava(JNIEnv *env, jobject am);
#endif
