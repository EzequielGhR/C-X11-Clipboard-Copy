#include <stdio.h>
#include <string.h>
#include "logging.h"
#include "clipboard.h"

#define MAX_STR_LEN 10000


int main(int argc, char** argv){
    if (argc < 2){
        char buff[MAX_STR_LEN];
        sprintf(buff, "Invalid amount of params (%d). Usage: %s <string>", argc, argv[0]);
        log_error(buff);
        return 1;
    }

    int input_length = 0;
    char input_buff[MAX_STR_LEN];

    strcpy(input_buff, argv[1]);

    for (int i=2; i<argc; i++){
        // Add Length of arguments plus a space
        input_length += strlen(argv[i]) + 1;

        // Check the supper bound
        if (input_length > MAX_STR_LEN){
            log_error("Provided input is too big");
            return 1;
        }

        // Concat space and arg
        strcat(input_buff, " ");
        strcat(input_buff, argv[i]);
    }

    copy_to_clipboard(input_buff);

    return 0;
}