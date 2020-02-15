#ifndef SMOKERS_AGENT_H
#define SMOKERS_AGENT_H

struct Agent {
    int tobacco_price;
    int paper_price;
    int matches_price;
    int *smoker_queues;
    int block_transactions;
};

void set_price(struct Agent *agent);
void wait();
void check_if_received_by_all(struct Agent *agent);

#endif //SMOKERS_AGENT_H
