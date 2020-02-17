#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


const struct Smokers smokers[SMOKERS] = {
        {TOBACCO, "tobacco", CYAN},
        {PAPER,   "paper", MAGENTA},
        {MATCHES, "matches", BLUE},
};

static int colors[4] = {36, 35, 34, 33};

static struct msqid_ds buf;

static struct sembuf sem;

static struct Msg msg;

void create_sem_sets(int *buf, int num_sem_sets, int num_sems) {
    for (int i = 0; i < num_sem_sets; i++) {
        buf[i] = create_sem_set(num_sems);
    }
}

int create_sem_set(int num_sems) {
    int sem_id = semget(IPC_PRIVATE, num_sems, IPC_CREAT | 0600);
    if (sem_id == -1) {
        fprintf(stderr, "Error: %s | semget | Process: %d\n", strerror(errno), getpid());
        exit(EXIT_FAILURE);
    }
    return sem_id;
}

void set_sem_val(int sem_id, int sem_num, int value) {
    if (semctl(sem_id, sem_num, SETVAL, value) == -1) {
        fprintf(stderr, "Error: %s | semctl - SETVAL | Process: %d\n", strerror(errno), getpid());
        exit(EXIT_FAILURE);
    }
}

int get_sem_val(int sem_id, int sem_num) {
    int value = semctl(sem_id, sem_num, GETVAL);
    if (value == -1) {
        fprintf(stderr, "Error: %s | semctl - GETVAL | Process: %d\n", strerror(errno), getpid());
        exit(EXIT_FAILURE);
    }
    return value;
}

void sem_op(int sem_id, int sem_num, int op) {
    sem.sem_num = sem_num;
    sem.sem_flg = 0;
    sem.sem_op = op;

    semop(sem_id, &sem, 1);
}

void create_mqs(int *buf, int size) {
    for (int i = 0; i < size; i++) {
        buf[i] = create_mq();
    }
}

int create_mq() {
    int queue = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (queue == -1) {
        fprintf(stderr, "Error: %s | msgget | Process: %d\n", strerror(errno), getpid());
        exit(EXIT_FAILURE);
    }
    return queue;
}

int check_queue_size(int msq_id) {
    int stats = msgctl(msq_id, IPC_STAT, &buf);
    if (stats == -1) {
        fprintf(stderr, "Error: %s | msgctl - QNUM | Process: %d\n", strerror(errno), getpid());
        exit(EXIT_FAILURE);
    }
    int qnum = (uint) (buf.msg_qnum);
    return qnum;
}

void send_msg(int msq_id, struct Msg msg) {
    if (msgsnd(msq_id, &msg, sizeof(msg), 0) == -1) {
        fprintf(stderr, "Error: %s | msgsnd | Process: %d\n", strerror(errno), getpid());
        exit(EXIT_FAILURE);
    }
}

struct Msg get_msg(int msq_id) {
    if (msgrcv(msq_id, &msg, sizeof(msg), 0, 0) == -1) {
        fprintf(stderr, "Error: %s | msgrcv | Process: %d\n", strerror(errno), getpid());
        exit(EXIT_FAILURE);
    }
    return msg;
}

int get_random(int min, int max) {
    return (random() % (max - min + 1)) + min;
}

void color_print(char *text, int color) {
    printf("\033[0;%dm", colors[color]);
    printf("%s\n", text);
    printf("\033[0m");
}
