#include "smoker.h"
#include "agent.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "shared.h"

int verbose;

void set_flags(int argc, char **argv);

int main(int argc, char **argv) {
    set_flags(argc, argv);

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
        struct Agent agent;
        agent.smoker_queues = agent_smoker_queues;
        agent.wallets = wallets;
        agent.text_color = YELLOW;

        while (1) {
            agent_do(&agent);
        }
    }

    int smoker_type = smokers[0].id;

    for (int i = 1; i < SMOKERS; i++) {
        if (fork() == 0) {
            smoker_type = smokers[i].id;
            break;
        }
    }

    struct Smoker smoker;

    struct Product cigarette_case[PRODUCTS] = {
            {0, FREE},
            {0, FREE},
            {0, FREE},
    };
    cigarette_case[smoker_type].value = INFINITE;

    smoker.id = getpid();
    smoker.smoker_type = smoker_type;
    smoker.agent_queue = *(agent_smoker_queues + smoker_type);
    smoker.cigarette_case = cigarette_case;
    smoker.wallet_id = *(wallets + smoker_type);
    smoker.prices = initial_prices;
    smoker.exchange_queues = exchange_queues;
    smoker.text_color = smokers[smoker_type].text_color;

    set_wallet(&smoker, initial_wallets[smoker_type]);

    sleep(1);

    while (1) {
        smoker_do(&smoker);
    }
}

void set_flags(int argc, char **argv) {
    int option;
    while ((option = getopt(argc, argv, "v")) != -1) {
        switch (option) {
            case 'v':
                verbose = 1;
                color_print("USING VERBOSE", YELLOW);
                break;
            default :
                verbose = 0;
                color_print("USING NON-VERBOSE", YELLOW);
                break;
        }
    }
}