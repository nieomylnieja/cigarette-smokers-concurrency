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
    create_mqs(agent_smoker_queues, SMOKERS);
    int smokers_exchange_queues[PRODUCTS];
    create_mqs(smokers_exchange_queues, PRODUCTS);

    int block_all_transactions = create_sem_set(1);
    int wallets = create_sem_set(3);

    if (fork() == 0) {
        struct Agent agent;
        agent.smoker_queues = agent_smoker_queues;
        agent.block_transactions = block_all_transactions;

        while(1) {
            set_price(&agent);
            wait();
        }
    }

    int cigarette_case[3] = {2, 2, 2};
    int smoker_type = products[0].id;
    int wallet_id = 0;
    int queue_id = 0;

    for(int i = 1; i < SMOKERS; i++) {
        if (fork() == 0) {
            smoker_type = products[i].id;
            wallet_id = i;
            queue_id = i;
            break;
        }
    }

    struct Smoker smoker;

    cigarette_case[smoker_type] = 1;

    smoker.id = getpid();
    smoker.smoker_type = smoker_type;
    smoker.agent_queue = *(agent_smoker_queues + queue_id);
    smoker.agent_block = block_all_transactions;
    smoker.cigarette_case = cigarette_case;

    smoker.wallet.sem_id = wallets;
    smoker.wallet.sem_num = wallet_id;

    while(1) {
        update_prices(&smoker);
        sleep(1);
        smoke(&smoker);

    }
}
