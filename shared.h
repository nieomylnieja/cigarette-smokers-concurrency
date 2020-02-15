#ifndef SMOKERS_SHARED_H
#define SMOKERS_SHARED_H

#define PRODUCTS 3
#define SMOKERS PRODUCTS
#define INFINITE 1
#define BUY_PRODUCT 0
#define SELL_PRODUCT 1
#define WALLET_BLOCK 0
#define WALLET_OP 1

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
void create_sem_sets(int *buf, int num_sem_sets, int num_sems);

int create_sem_set(int num_sems);

void set_sem_val(int sem_id, int sem_num, int value);

void sem_op(int sem_id, int sem_num, int op);

int get_sem_val(int sem_id, int sem_num);

// QUEUE OPERATIONS
void create_mqs(int *buf, int size);

int create_mq();

void send_msg(int msq_id, int type, int value);

void send_msg_to_many(int *msq_id, int type, int value);

struct Msg get_msg(int msq_id);

int check_queue_size(int msq_id);

// UTILS
int get_random(int min, int max);

#endif //SMOKERS_SHARED_H
