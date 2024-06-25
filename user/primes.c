#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void filter(int* f_p) {
    int p;
    read(f_p[0], &p, sizeof(p));
    if (p == -1) {
        close(f_p[0]);
        exit(0);
    }

    fprintf(1, "prime %d\n", p);

    int n_p[2];
    pipe(n_p);

    if (fork() == 0) {
        close(n_p[1]);
        close(f_p[0]);
        filter(n_p);
        close(n_p[0]);
    } else {
        close(n_p[0]);
        int buf = 0;
        while (1) {
            read(f_p[0], &buf, sizeof(buf));
            if (buf == -1) {
                break;
            }
            if (buf % p != 0) {
                write(n_p[1], &buf, sizeof(buf));
            }
        }
        int end_buf = -1;
        write(n_p[1], &end_buf, sizeof(end_buf));
        close(n_p[1]);
        wait(0);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    int in_p[2];
    pipe(in_p);

    if (fork() == 0) {
        close(in_p[1]);
        filter(in_p);
        close(in_p[0]);
    } else {
        close(in_p[0]);
        for (int i = 2; i <= 35; i++) {
            write(in_p[1], &i, sizeof(i));
        }
        int end_i = -1;
        write(in_p[1], &end_i, sizeof(end_i));
        close(in_p[1]);
        wait(0);
    }
    exit(0);
}