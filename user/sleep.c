#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("You should pass an argument!\n");
        exit(1);
    }

    int sleeptime = atoi(argv[0]);
    sleep(sleeptime);
    printf("(nothing happens for a little while)\n");
    exit(0);
}