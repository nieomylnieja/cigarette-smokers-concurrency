#include <stdio.h>
#include <zconf.h>
#include "smokers.h"
#include "agent.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "shared.h"


int main() {
    srandom(time(NULL));

    int smokers_exchange_queues[PRODUCTS];
    int agent_smoker_queues[SMOKERS];
    int wallets[SMOKERS];

    create_mqs(smokers_exchange_queues, PRODUCTS);
    create_mqs(agent_smoker_queues, SMOKERS);
    create_sem_sets(wallets, SMOKERS, 2);

    if (fork() == 0) {
        struct Agent agent;
        agent.smoker_queues = agent_smoker_queues;
        agent.wallets = wallets;

        while(1) {
            set_price(&agent);
            wait();
        }
    }

    int smoker_type = products[0].id;

    for(int i = 1; i < SMOKERS; i++) {
        if (fork() == 0) {
            smoker_type = products[i].id;
            break;
        }
    }

    struct Smoker smoker;

    int cigarette_case[3] = {2, 2, 2};
    cigarette_case[smoker_type] = INFINITE;

    smoker.id = getpid();
    smoker.smoker_type = smoker_type;
    smoker.agent_queue = *(agent_smoker_queues + smoker_type);
    smoker.cigarette_case = cigarette_case;
    smoker.wallet_id = *(wallets + smoker_type);

    while(1) {
        update_prices(&smoker);
        sleep(1);
        smoke(&smoker);
    }
}
