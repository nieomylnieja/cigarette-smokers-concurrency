#ifndef SMOKERS_SMOKERS_H
#define SMOKERS_SMOKERS_H

struct Smoker {
    int id;
    int smoker_type;
    int agent_queue;
    int *prices;
    int *cigarette_case;
    int wallet_id;
    int *exchange_queues;
};

void smoker_do(struct Smoker *smoker);

void set_wallet(struct Smoker *smoker, int money);

#endif //SMOKERS_SMOKERS_H
