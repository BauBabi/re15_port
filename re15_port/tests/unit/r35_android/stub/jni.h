/* Runde 35 Spur N (Vorbild Gegenpruefung R4-2): JNI-Attrappe, nur was android_glue.c apk_assets() ruft */
#ifndef H_JNI_H
#define H_JNI_H
typedef void *jobject;
typedef void *jclass;
typedef void *jmethodID;
typedef unsigned char jboolean;
struct JNINativeInterface;
typedef const struct JNINativeInterface *JNIEnv;
struct JNINativeInterface {
    jclass (*GetObjectClass)(JNIEnv *, jobject);
    jmethodID (*GetMethodID)(JNIEnv *, jclass, const char *, const char *);
    jobject (*CallObjectMethod)(JNIEnv *, jobject, jmethodID, ...);
    jboolean (*ExceptionCheck)(JNIEnv *);
    void (*ExceptionClear)(JNIEnv *);
    jobject (*NewGlobalRef)(JNIEnv *, jobject);
    void (*DeleteLocalRef)(JNIEnv *, jobject);
};
#endif
