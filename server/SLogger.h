//
// Created by 李勃鋆 on 24-8-5.
//

#ifndef SLOGGER_H
#define SLOGGER_H
#include "thread_pool.h"

#define logDebug(conn,fmt, ...)SLoggerInstance()-> log(LOG_DEBUG, __FILE__, __LINE__,conn, fmt, ##__VA_ARGS__)
#define logInfo(conn,fmt, ...) SLoggerInstance()-> log(LOG_INFO, __FILE__, __LINE__,conn, fmt, ##__VA_ARGS__)
#define logWarn(conn,fmt, ...) SLoggerInstance()-> log(LOG_WARN, __FILE__, __LINE__,conn, fmt, ##__VA_ARGS__)
#define logError(conn,fmt, ...) SLoggerInstance()-> log(LOG_ERROR, __FILE__, __LINE__,conn, fmt, ##__VA_ARGS__)
#define logNotice(conn,fmt, ...) SLoggerInstance()-> log(LOG_NOTICE, __FILE__, __LINE__,conn, fmt, ##__VA_ARGS__)

typedef enum my_log_level {
	//日志级别
	LOG_NOTICE, //通知用户的信息
    LOG_INFO, //用户输入的信息
    LOG_DEBUG, //调试
	LOG_WARN, //警告
	LOG_ERROR //错误
} log_level;

typedef struct S_Logger {
	log_level _level; //日志输出级别

	void (*log)(log_level level, const char *file, const int line, MYSQL *conn, const char *fmt, ...);

	void (*setLevel)(log_level level);

	log_level (*level)();
} SLogger;

SLogger *SLoggerInstance();
#endif //SLOGGER_H

