//
// Created by 李勃鋆 on 24-8-5.
//

#include "SLogger.h"
#include "mysql.h"
FILE *fp = NULL;
static SLogger *pthis = NULL;

const char *getLogLevel(const log_level level) {
	//获取日志级别
	switch (level) {
		case LOG_DEBUG:
			return "DEBUG";
		case LOG_INFO:
			return "INFO";
		case LOG_WARN:
			return "WARN";
		case LOG_ERROR:
			return "ERROR";
		case LOG_NOTICE:
			return "NOTICE";
		default:
			return "UNKNOWN";
	}
}

char *getDate() {
	const time_t t = time(NULL);
	const struct tm *tm = localtime(&t);
	static char str[128];
	strftime(str, 128, "%Y-%m-%d %H:%M:%S", tm);
	return str;
}

static void myLog(const log_level level, const char *file, const int line, MYSQL *conn, const char *fmt, ...) {
	if (level < pthis->_level) {
		return;
	}
	va_list list;
	va_start(list, fmt);

	char buf[1024];
	vsnprintf(buf, 1024, fmt, list);

	va_end(list);

	char msg[2048];
	sprintf(msg, "[%s]  %s:%d %s %s\n", getLogLevel(level), file, line, getDate(), buf);
	if (fp != NULL) {
		fprintf(fp, "%s", msg);
        fflush(fp);  // 刷新缓冲区，将数据写入文件
	}

	// char *sql1 = "SELECT * FROM log";
	// char *sql2 = "INSERT INTO log (log_level, file, line, times, info) VALUES ('INFO', 'main.c', 10, '2024-08-06 14:24:19', 'ls')";
	// char sql[2048];
	// sprintf(sql, "INSERT INTO log (log_level, file, line, times, info) VALUES ('%s', '%s', %d, now(), '%s')", getLogLevel(level), file, line, buf);
	// printf("sql = %s", sql);
	// int page = 1; // 要显示的页码
	// execute_query(conn, sql1, page);
	// int len = execute_insert(conn, sql2);
	// printf("len = %d", len);

	char sql[2048];
	sprintf(sql, "INSERT INTO log (log_level, file, line, times, info) VALUES ('%s', '%s', %d, now(), '%s')", getLogLevel(level), file, line, buf);

	execute_insert(conn, sql);
}

void setLevel(const log_level level) {
	pthis->_level = level;
}

log_level level() {
	return pthis->_level;
}

SLogger *SLoggerInstance() {
	// static SLogger logger;
	// logger.log = myLog;
	// return &logger;
	if (pthis == NULL) {
		static SLogger logger;
		pthis = &logger;

		logger.log = myLog;
		logger.setLevel = setLevel;
		logger.level = level;
	}
	return pthis;
}

