#ifndef SMOKERS_SHARED_H
#define SMOKERS_SHARED_H
// MSG CONTENT
#define SENDER_TYPE 0
#define WALLET_ID 1
#define PRICE 2
#define PRODUCT_TYPE 3
// PRODUCT TYPES
#define TOBACCO 0
#define MATCHES 1
#define PAPER 2
// OTHER
#define PRODUCTS 3
#define SMOKERS PRODUCTS
#define INFINITE 1
// MSG TYPES
#define INFORM 0
#define BUY_PRODUCT 1
#define TRANSACTION_CANCELLED 2
#define PRODUCT_SOLD 3
// WALLET SEM OPS
#define WALLET_BLOCK 0
#define WALLET_OP 1
#define PENDING 2

struct Product {
    int id;
    char *name;
};

extern const struct Product products[PRODUCTS];

struct Content {
    int sender_type;
    int wallet_id;
    int price;
    int product_type;
};

struct Msg {
    int type;
    struct Content content;
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

void send_msg(int msq_id, struct Msg msg);

struct Msg get_msg(int msq_id);

int check_queue_size(int msq_id);

// UTILS
int get_random(int min, int max);

#endif //SMOKERS_SHARED_H
