#ifndef SMOKERS_SMOKERS_H
#define SMOKERS_SMOKERS_H

struct Wallet {
    int sem_id;
    int sem_num;
};

struct Smoker {
    int id;
    int smoker_type;
    int agent_queue;
    int agent_block;
    int *cigarette_case;
    struct Wallet wallet;
    int *products_queues;
};

void update_prices(struct Smoker *smoker);

void sell(struct Smoker *smoker);

void buy(struct Smoker *smoker);

int smoke_condition_satisfied(struct Smoker *smoker);

void create_cigarette(struct Smoker *smoker);

void smoke(struct Smoker *smoker);

#endif //SMOKERS_SMOKERS_H
