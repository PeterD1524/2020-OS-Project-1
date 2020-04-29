#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/linkage.h>
#include <linux/types.h>
asmlinkage int PRINTK(const char* TAG, pid_t PID, struct timespec* ST, struct timespec* FT) {
    return printk("%s %lld %lld.%lld %lld.%lld\n", TAG, (long long)PID, (long long)ST->tv_sec, (long long)ST->tv_nsec, (long long)FT->tv_sec, (long long)FT->tv_nsec);
}
