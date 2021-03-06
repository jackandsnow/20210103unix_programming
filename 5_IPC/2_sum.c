/*************************************************************************
	> File Name: 1_sum.c
	> Author: wangxue
	> Mail: 
	> Created Time: Thu Jan 21 15:25:36 2021
 ************************************************************************/

#include <stdio.h>
#include "head.h"

//#define INS 5

char num_file[] = "./.num"; // only save data
char lock_file[] = "./.lock"; // like a box for lock

struct Msg {
    int num;
    int sum;
};

struct Msg now_num;

size_t set_num(struct Msg *msg){
    FILE *f = fopen(num_file, "w");
    if (f == NULL) {
        fclose(f);
        perror("fopen error \n");
        return -1;
    }
    //write content
    size_t nwrite = fwrite(msg, 1, sizeof(struct Msg), f);
    fclose(f);
    return nwrite;
}

size_t get_num(struct Msg *msg) {
    FILE *f = fopen(num_file, "r");
    if (f == NULL) {
        perror("fopen error \n");
        return -1;
    }
    size_t nread = fread(msg, 1, sizeof(struct Msg), f);
    fclose(f);
    return nread;
}

// Atomic read and write
void do_add(int end, int pid_num) {
    while (1) {
        FILE *lock = fopen(lock_file, "w");
        if (lock == NULL) {
            perror("flock error! \n");
            exit(1); // dead
        }
        // add the lock
        flock(lock->_fileno, LOCK_EX);

        if (get_num(&now_num) < 0) {
            // do not read the data, you can try next time, but lock have to shut
            // only shut the file - lock, the LOCK_EX can release
            fclose(lock);
            continue;
        }
        // boundary condition
        if (now_num.num > end) {
            fclose(lock);
            break;
        }
        // sum add
        now_num.sum += now_num.num;
        now_num.num++;
        printf("The %dth child get lock, now = %d, sum = %d\n", pid_num, now_num.num, now_num.sum);
        set_num(&now_num);
        // release lock
        //flock(lock->_fileno, LOCK_UN);
        fclose(lock);
        sleep(2);
    }
}

int main(int argc, char *argv[]) {
    // get args
    int opt, start = 0, end = 0, INS = 0;
    while ((opt = getopt(argc, argv, "s:e:i:")) != -1 ) {
        switch (opt) {
            case 's':
                start = atoi(optarg);
                break;
            case 'e':
                end = atoi(optarg);
                break;
            case 'i':
                INS = atoi(optarg);
                break;
            default:
                fprintf(stderr, "usage : %s -s start -e end \n", argv[0]);
                exit(1);
        }
    }
    printf("start = %d \nend = %d \n", start, end);
    // init now_num's value
    now_num.num = 0;
    now_num.sum = 0;
    set_num(&now_num);

    // create child process
    pid_t pid;
    int x = 0;
    for(int i = 1; i <= INS; i++) {
        if ((pid = fork()) < 0) {
            perror("fork() error \n");
            // process kill
            exit(1);
        }
        if (pid == 0) {
            // child's x = i
            x = i;
            break;
        }
    }
    // judge whether child, do different task
    if (pid != 0) {
        for (int i = 0; i < INS; i++) {
            wait(NULL); // wait child to dead
        }
        get_num(&now_num);
        printf("sum = %d\n", now_num.sum);
        // printf("I am parent \n");
        // sleep(100);
    } else {
        // child tag
        // printf("I am %dth child \n",x);
        do_add(end, x);
    }
    return 0;
}

