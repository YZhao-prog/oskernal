#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p1[2]; // father -> child  0: read  1: write
    int p2[2]; // child -> father
    pipe(p1);
    pipe(p2);

    if (fork() != 0) {
        close(p1[0]);
        close(p2[1]);
        char data = 'a';
        if (write(p1[1], &data, 1) != 1) {
            fprintf(2, "error write father -> child\n");
            exit(1);
        }
        char buf;
        if (read(p2[0], &buf, 1) != 1) {
            fprintf(2, "error read father <- child\n");
            exit(1);
        }
        if (buf == 'b') {
            fprintf(1, "%d: received pong\n", getpid());
        }
        close(p1[1]);
        close(p2[0]);
    } else {
        close(p1[1]);
        close(p2[0]);
        char buf;
        if (read(p1[0], &buf, 1) != 1) {
            fprintf(2, "error read father -> child\n");
            exit(1);
        }
        if (buf == 'a') {
            fprintf(1, "%d: received ping\n", getpid());
        }
        char data = 'b';
        if (write(p2[1], &data, 1) != 1) {
            fprintf(2, "error write father <- child\n");
            exit(1);
        }
        close(p1[0]);
        close(p2[1]);
    }
    exit(0);
}