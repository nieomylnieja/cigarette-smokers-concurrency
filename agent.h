#ifndef SMOKERS_AGENT_H
#define SMOKERS_AGENT_H

struct Agent {
    int tobacco_price;
    int paper_price;
    int matches_price;
    int *smoker_queues;
    int *wallets;
};

void block_all_wallets(int *wallets);

void set_price(struct Agent *agent);

void wait();

void make_sure_everyone_was_informed(struct Agent *agent);

#endif //SMOKERS_AGENT_H
