#ifndef SMOKERS_SMOKER_H
#define SMOKERS_SMOKER_H

struct Product {
    int value;
    int status;
};

struct Smoker {
    int id;
    int smoker_type;
    int agent_queue;
    int *prices;
    struct Product *cigarette_case;
    int wallet_id;
    int *exchange_queues;
    int text_color;
};

void smoker_do(struct Smoker *smoker);

void set_wallet(struct Smoker *smoker, int money);

#endif //SMOKERS_SMOKER_H
