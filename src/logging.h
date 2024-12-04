#ifndef LOGGING_H
    #define LOGGING_H
    #define MAX_LOG_LEN 10000
    enum {INFO_LEVEL, ERROR_LEVEL};

    void logger(const char* message, const int level);
    void log_msg(const char* message);
    void log_error(const char* message);
#endif