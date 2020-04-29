#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void unitoftime() {
    volatile unsigned long i;
    for (i = 0; i < 1000000UL; i++)
        ;
}
struct process {
    int index;
    char N[32];
    int R, T;
    pid_t pid;
};
int main() {
    int n = 100;
    int* time = mmap(NULL, sizeof(*time), PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int* array = mmap(NULL, n * sizeof(*array), PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < n; i++) {
        array[i] = i;
    }
    struct process* ARRAY = mmap(NULL, n * sizeof(*ARRAY), PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < n; i++) {
        ARRAY[i].index = i * i * i;
    }
    *time = 0;
    int t = 2000;
    pid_t p = fork();
    if (p == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (p == 0) {
        while (*time < t) {
            sleep(1);
            printf("%d\n", *time);
            for (int i = 0; i < n; i++) {
                printf("%d ", array[i]);
            }
            printf("\n");
            for (int i = 0; i < n; i++) {
                printf("%d ", ARRAY[i].index);
            }
            printf("\n");
        }
        _exit(EXIT_SUCCESS);
    } else {
        for (int i = 0; i < t; i++) {
            unitoftime();
            array[i % n] = array[i % n] + 1;
            ARRAY[i % n].index = ARRAY[i % n].index + 1;
            *time = *time + 1;
        }
        wait(NULL);
        munmap(time, sizeof(*time));
    }
}