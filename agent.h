#ifndef SMOKERS_AGENT_H
#define SMOKERS_AGENT_H

struct Agent {
    int tobacco_price;
    int paper_price;
    int matches_price;
    int *smoker_queues;
    int *wallets;
};

void agent_do(struct Agent *agent);

#endif //SMOKERS_AGENT_H
