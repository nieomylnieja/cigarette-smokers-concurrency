#ifndef SMOKERS_SHARED_H
#define SMOKERS_SHARED_H

#define PRODUCTS 3
#define SMOKERS PRODUCTS

struct Product {
    int id;
    char *name;
};

extern const struct Product products[PRODUCTS];

struct Msg {
    int type;
    int value;
};

// SEMAPHORE OPERATIONS
int create_sem_set(int num_sems);

void set_sem_val(int sem_id, int sem_num, int value);

void sem_op(int sem_id, int op);

int get_sem_val(int sem_id, int sem_num);

// QUEUE OPERATIONS
void create_mqs(int *buf, int size);

int create_mq();

void send_msg(int queue, int type, int value);

void send_msg_to_many(int *queue, int type, int value);

struct Msg get_msg(int queue);

int check_queue_size(int msq_id);

// UTILS
int get_random(int min, int max);

#endif //SMOKERS_SHARED_H
