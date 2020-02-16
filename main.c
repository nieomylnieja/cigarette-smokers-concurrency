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

    int agent_smoker_queues[SMOKERS];
    int exchange_queues[PRODUCTS];
    int wallets[SMOKERS];

    create_mqs(exchange_queues, PRODUCTS);
    create_mqs(agent_smoker_queues, SMOKERS);
    create_sem_sets(wallets, SMOKERS, 3);

    int initial_wallets[SMOKERS] = {10, 10, 10};
    int initial_prices[PRODUCTS] = {2, 2, 2};

    if (fork() == 0) {
        printf("AGENT: %d\n", getpid());
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

    printf("%s: %d\n", products[smoker_type].name, getpid());

    struct Smoker smoker;

    int cigarette_case[3] = {2, 2, 2};
    cigarette_case[smoker_type] = INFINITE;

    smoker.id = getpid();
    smoker.smoker_type = smoker_type;
    smoker.agent_queue = *(agent_smoker_queues + smoker_type);
    smoker.cigarette_case = cigarette_case;
    smoker.wallet_id = *(wallets + smoker_type);
    smoker.prices = initial_prices;
    smoker.exchange_queues = exchange_queues;

    set_wallet(&smoker, initial_wallets[smoker_type]);

    while(1) {
        update_prices(&smoker);
        smoke(&smoker);
        receive_message(&smoker);
    }
}
