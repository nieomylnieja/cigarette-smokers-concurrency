#include "agent.h"
#include <stdio.h>
#include <zconf.h>
#include "shared.h"

void set_price(struct Agent *agent) {
    block_all_wallets(agent->wallets);

    int product = get_random(0,2);
    int price = get_random(1,5);
    if (product == 0) {
        agent->tobacco_price = price;
    } else if (product == 1) {
        agent->paper_price = price;
    } else if (product == 2) {
        agent->matches_price = price;
    }
    printf("\nAgent set price of %s to %d.\n\n", products[product].name, price);

    send_msg_to_many(agent->smoker_queues, product, price);

    make_sure_everyone_was_informed(agent);
}

void block_all_wallets(int *wallets) {
    for (int i = 0; i < SMOKERS; i++) {
        set_sem_val(*(wallets + i), WALLET_BLOCK, 1);
    }
}

void make_sure_everyone_was_informed(struct Agent *agent) {
    int checked[3] = {0, 0, 0};
    int empty = 0;
    while(empty != 3) {
        for (int i = 0; i < SMOKERS; i++) {
            if (check_queue_size(agent->smoker_queues[i]) == 0 && checked[i] == 0) {
                set_sem_val(*(agent->wallets + i), WALLET_BLOCK, 0);
                checked[i] = 1;
                empty++;
            }
        }
    }
}

void wait() {
    sleep(get_random(10, 20));
}
