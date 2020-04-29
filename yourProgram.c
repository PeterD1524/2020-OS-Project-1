#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define PRINTK 334
#define GETNSTIMEOFDAY 335
#define TIMECPU 0
#define FORKCPU 1
#define AAAAACPU 2
#define CHILDCPU 3
#define UNFORKED -1
#define FORKED 0
#define DEAD 1
struct process {
    int index;
    char N[32];
    int R, T;
    pid_t pid;
    int state;
};
int comparebyR(const void* a, const void* b) {
    struct process* A = (struct process*)a;
    struct process* B = (struct process*)b;
    if (A->R < B->R) {
        return -1;
    } else if (A->R == B->R) {
        if (A->index < B->index) {
            return -1;
        } else if (A->index == B->index) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}
int comparebyT(const void* a, const void* b) {
    struct process* A = (struct process*)a;
    struct process* B = (struct process*)b;
    if (A->T < B->T) {
        return -1;
    } else if (A->T == B->T) {
        if (A->index < B->index) {
            return -1;
        } else if (A->index == B->index) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}
void unitoftime() {
    volatile unsigned long i;
    for (i = 0; i < 1000000UL; i++)
        ;
}
int main() {
    struct sched_param* param = malloc(sizeof(struct sched_param));
    param->sched_priority = 0;
    unsigned long long* unitsoftimesincethestartofthemainprocess =
        mmap(NULL, sizeof(*unitsoftimesincethestartofthemainprocess),
             PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *unitsoftimesincethestartofthemainprocess = 0;
    int* done = mmap(NULL, sizeof(*done), PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *done = 0;
    int running = -1;
    int runninglastT = -1;
    pid_t p = fork();
    if (p == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (p == 0) {
        cpu_set_t AAAAACPUset;
        CPU_ZERO(&AAAAACPUset);
        CPU_SET(AAAAACPU, &AAAAACPUset);
        if (sched_setaffinity(getpid(), sizeof(AAAAACPUset), &AAAAACPUset) ==
            -1) {
            perror("sched_setaffinity");
            exit(EXIT_FAILURE);
        }
        char* schedulingpolicy[] = {"FIFO", "RR", "SJF", "PSJF"};
        char S[4];
        int N;
        scanf("%s%d", S, &N);
        struct process* processes =
            mmap(NULL, N * sizeof(*processes), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        for (int i = 0; i < N; i++) {
            processes[i].index = i;
            processes[i].pid = -1;
            processes[i].state = UNFORKED;
            scanf("%s%d%d", processes[i].N, &processes[i].R, &processes[i].T);
        }
        // printf("%s\n%d\n", S, N);
        for (int i = 0; i < N; i++) {
            // printf("%s %d %d\n", processes[i].N, processes[i].R, processes[i].T);
        }
        qsort(processes, N, sizeof(*processes), comparebyR);
        pid_t pp = fork();
        if (pp == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pp == 0) {
            cpu_set_t FORKCPUset;
            CPU_ZERO(&FORKCPUset);
            CPU_SET(FORKCPU, &FORKCPUset);
            if (sched_setaffinity(getpid(), sizeof(FORKCPUset), &FORKCPUset) ==
                -1) {
                perror("sched_setaffinity");
                exit(EXIT_FAILURE);
            }
            unsigned long long qind = 0;
            while (qind < N) {
                unsigned long long timestamp =
                    *unitsoftimesincethestartofthemainprocess;
                if (timestamp >= processes[qind].R) {
                    // printf("qind: %lld\ntimestamp: %lld\n", qind, timestamp);
                    char NAME[32];
                    strcpy(NAME, processes[qind].N);
                    struct process* fp = &processes[qind];
                    qind++;
                    pid_t PID;
                    PID = fork();
                    struct timespec* ST = malloc(sizeof(struct timespec));
                    syscall(GETNSTIMEOFDAY, ST);
                    // struct timeval* ST = malloc(sizeof(struct timeval));
                    // gettimeofday(ST, NULL);
                    if (PID == -1) {
                        perror("fork");
                        exit(EXIT_FAILURE);
                    }
                    unsigned long long starttimestamp =
                        *unitsoftimesincethestartofthemainprocess;
                    if (PID == 0) {
                        fp->state = FORKED;
                        int T = fp->T;
                        for (int i = 0; i < T; i++) {
                            unitoftime();
                        }
                        struct timespec* FT = malloc(sizeof(struct timespec));
                        syscall(GETNSTIMEOFDAY, FT);
                        // struct timeval* FT = malloc(sizeof(struct timeval));
                        // gettimeofday(FT, NULL);
                        char TAG[] = "[Project1]";
                        syscall(PRINTK, TAG, getpid(), ST, FT);
                        /*
                        printf("%s %d %ld.%ld %ld.%ld %lld %lld\n", TAG,
                               getpid(), ST->tv_sec, ST->tv_usec, FT->tv_sec,
                               FT->tv_usec, starttimestamp,
                               *unitsoftimesincethestartofthemainprocess);
                        */
                        fp->state = DEAD;
                        printf("%s %d\n", NAME, getpid());
                        exit(EXIT_SUCCESS);
                    } else {
                        if (sched_setscheduler(PID, SCHED_IDLE, param) == -1) {
                            // perror("sched_setscheduler");
                            if (errno != ESRCH) {
                                exit(EXIT_FAILURE);
                            }
                        }
                        cpu_set_t CHILDCPUset;
                        CPU_ZERO(&CHILDCPUset);
                        CPU_SET(CHILDCPU, &CHILDCPUset);
                        if (sched_setaffinity(PID, sizeof(CHILDCPUset),
                                              &CHILDCPUset) == -1) {
                            perror("sched_setaffinity");
                            exit(EXIT_FAILURE);
                        }
                        fp->pid = PID;
                        signal(SIGCHLD, SIG_IGN);
                    }
                }
            }
        } else {
            unsigned long long pt = 0;
            if (strcmp(S, schedulingpolicy[0]) == 0) {
                while (pt < N) {
                    unsigned long long timestamp =
                        *unitsoftimesincethestartofthemainprocess;
                    if (timestamp >= processes[pt].R) {
                        if (processes[pt].pid != -1) {
                            // printf("     %d\n", processes[pt].pid);
                            if (sched_setscheduler(processes[pt].pid,
                                                   SCHED_OTHER, param) == -1) {
                                // perror("sched_setscheduler");
                                exit(EXIT_FAILURE);
                            };
                            while (processes[pt].state != DEAD) {
                            }
                            // printf("     %d\n", processes[pt].pid);
                            pt++;
                        }
                    }
                }
            } else if (strcmp(S, schedulingpolicy[1]) == 0) {
                int queue[N];
                int queuestart = 0;
                int queueend = N - 1;
                int queuesize = 0;
                int finished = 0;
                int init[N];
                for (int i = 0; i < N; i++) {
                    init[i] = 0;
                }
                int inited = 0;
                while (finished < N) {
                    unsigned long long timestamp =
                        *unitsoftimesincethestartofthemainprocess;
                    if (inited != N) {
                        for (int i = 0; i < N; i++) {
                            if (init[i] == 0 && timestamp >= processes[i].R) {
                                while (processes[i].pid == -1) {
                                }
                                if (queuesize == N) {
                                    // printf("queue\n");
                                    exit(EXIT_FAILURE);
                                }
                                queueend = (queueend + 1) % N;
                                queue[queueend] = i;
                                queuesize = queuesize + 1;
                                init[i] = 1;
                                inited = inited + 1;
                            }
                        }
                    }
                    int next = -1;
                    if (queuesize != 0) {
                        if (running == -1) {
                            if (processes[queue[queuestart]].T > 0) {
                                next = queue[queuestart];
                                queuestart = (queuestart + 1) % N;
                                queuesize = queuesize - 1;
                            }
                        } else {
                            if (timestamp - runninglastT >= 500) {
                                if (processes[queue[queuestart]].T > 0) {
                                    next = queue[queuestart];
                                    queuestart = (queuestart + 1) % N;
                                    queuesize = queuesize - 1;
                                }
                            } else {
                                next = running;
                            }
                        }
                    }
                    if (next != -1) {
                        if (next != running) {
                            if (running != -1) {
                                if (sched_setscheduler(processes[running].pid,
                                                       SCHED_IDLE,
                                                       param) == -1) {
                                    // perror("sched_setscheduler");
                                    // printf("SCHED_IDLE %d\n", running + 1);
                                    if (errno != ESRCH) {
                                        exit(EXIT_FAILURE);
                                    }
                                };
                                processes[running].T =
                                    processes[running].T -
                                    (*unitsoftimesincethestartofthemainprocess -
                                     runninglastT);
                                if (processes[running].T <= 0) {
                                    // printf("%d +1\n", running + 1);
                                    while (processes[running].state != DEAD) {
                                    }
                                    finished++;
                                }
                            }
                            // printf("%d -> %d %lld\n", running + 1, next + 1, *unitsoftimesincethestartofthemainprocess);
                            if (sched_setscheduler(processes[next].pid,
                                                   SCHED_OTHER, param) == -1) {
                                // perror("sched_setscheduler");
                                // printf("SCHED_OTHER %d\n", next + 1);
                                exit(EXIT_FAILURE);
                            };
                            runninglastT =
                                *unitsoftimesincethestartofthemainprocess;
                            if (running != -1) {
                                if (processes[running].T > 0) {
                                    if (queuesize == N) {
                                        // printf("queue\n");
                                        exit(EXIT_FAILURE);
                                    }
                                    queueend = (queueend + 1) % N;
                                    queue[queueend] = running;
                                    queuesize = queuesize + 1;
                                }
                            }
                            running = next;
                        }
                    }
                    if (finished == N - 1) {
                        if (processes[running].T -
                                (*unitsoftimesincethestartofthemainprocess -
                                 runninglastT) <=
                            0) {
                            // printf("%d +1\n", running + 1);
                            while (processes[running].state != DEAD) {
                            }
                            finished++;
                        }
                    }
                }
            } else if (strcmp(S, schedulingpolicy[2]) == 0) {
                int finished = 0;
                while (finished < N) {
                    int next = -1;
                    unsigned long long timestamp =
                        *unitsoftimesincethestartofthemainprocess;
                    for (int i = 0; i < N; i++) {
                        if (timestamp >= processes[i].R) {
                            while (processes[i].pid == -1) {
                            }
                        }
                        if (processes[i].pid == -1 || processes[i].T == 0) {
                            continue;
                        }
                        if (next == -1 || processes[i].T < processes[next].T) {
                            next = i;
                        }
                    }
                    if (next != -1) {
                        if (sched_setscheduler(processes[next].pid, SCHED_OTHER,
                                               param) == -1) {
                            // perror("sched_setscheduler");
                            exit(EXIT_FAILURE);
                        };
                        while (processes[next].state != DEAD) {
                        }
                        processes[next].T = 0;
                        finished++;
                    }
                }
            } else if (strcmp(S, schedulingpolicy[3]) == 0) {
                int finished = 0;
                while (finished < N) {
                    int next = -1;
                    int runningRT;
                    if (running != -1) {
                        runningRT = processes[running].T;
                        processes[running].T =
                            processes[running].T -
                            (*unitsoftimesincethestartofthemainprocess -
                             runninglastT);
                    }
                    for (int i = 0; i < N; i++) {
                        if (processes[i].pid == -1 || processes[i].T <= 0) {
                            continue;
                        }
                        if (next == -1 || processes[i].T < processes[next].T) {
                            next = i;
                        }
                    }
                    if (running != -1) {
                        processes[running].T = runningRT;
                    }
                    if (next != -1) {
                        if (next != running) {
                            if (running != -1) {
                                if (sched_setscheduler(processes[running].pid,
                                                       SCHED_IDLE,
                                                       param) == -1) {
                                    // perror("sched_setscheduler");
                                    // printf("SCHED_IDLE %d\n", running + 1);
                                    if (errno != ESRCH) {
                                        exit(EXIT_FAILURE);
                                    }
                                };
                                processes[running].T =
                                    processes[running].T -
                                    (*unitsoftimesincethestartofthemainprocess -
                                     runninglastT);
                                if (processes[running].T <= 0) {
                                    // printf("%d +1\n", running + 1);
                                    while (processes[running].state != DEAD) {
                                    }
                                    finished++;
                                }
                            }
                            // printf("%d -> %d\n", running + 1, next + 1);
                            if (sched_setscheduler(processes[next].pid,
                                                   SCHED_OTHER, param) == -1) {
                                // perror("sched_setscheduler");
                                // printf("SCHED_OTHER %d\n", next + 1);
                                exit(EXIT_FAILURE);
                            };
                            runninglastT =
                                *unitsoftimesincethestartofthemainprocess;
                            running = next;
                        }
                    }
                    if (finished == N - 1) {
                        if (processes[running].T -
                                (*unitsoftimesincethestartofthemainprocess -
                                 runninglastT) <=
                            0) {
                            // printf("%d +1\n", running + 1);
                            while (processes[running].state != DEAD) {
                            }
                            finished++;
                        }
                    }
                }
            }
            *done = 1;
        }
    } else {
        cpu_set_t TIMECPUset;
        CPU_ZERO(&TIMECPUset);
        CPU_SET(TIMECPU, &TIMECPUset);
        if (sched_setaffinity(getpid(), sizeof(TIMECPUset), &TIMECPUset) ==
            -1) {
            perror("sched_setaffinity");
            exit(EXIT_FAILURE);
        }
        while (*done != 1) {
            unitoftime();
            *unitsoftimesincethestartofthemainprocess =
                *unitsoftimesincethestartofthemainprocess + 1;
            /*
            if (*unitsoftimesincethestartofthemainprocess % 100 == 0) {
                printf("current time: %lld\n", *unitsoftimesincethestartofthemainprocess);
            }
            */
        }
    }
}
