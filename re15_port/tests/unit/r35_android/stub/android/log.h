#ifndef H_LOG_H
#define H_LOG_H
enum { ANDROID_LOG_INFO = 4, ANDROID_LOG_ERROR = 6 };
int __android_log_print(int prio, const char *tag, const char *fmt, ...);
#endif
