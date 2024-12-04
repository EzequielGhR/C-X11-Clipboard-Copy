#include <stdio.h>
#include <time.h>
#include "logging.h"

void logger(const char* message, const int level){
    // Create timestamp
    time_t now;
    time(&now);
    struct tm* time_struct_ptr = localtime(&now);

    // Format date to string
    char time_buff[100];
    strftime(time_buff, MAX_LOG_LEN, "%Y-%m-%dT%H:%M:%S", time_struct_ptr);

    // Error level goes directly to stderr stream
    char* level_text = level == INFO_LEVEL ? "INFO" : "ERROR";
    FILE* stream = level == INFO_LEVEL ? stdout : stderr;

    // Print the message to stream
    fprintf(stream, "%s - %s: %s\n", level_text, time_buff, message);
}


void log_error(const char* message){
    logger(message, ERROR_LEVEL);
}


void log(const char* message){
    logger(message, INFO_LEVEL);
}
