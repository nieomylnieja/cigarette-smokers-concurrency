#include "agent.h"
#include <stdio.h>
#include <zconf.h>
#include "shared.h"

void set_price(struct Agent *agent) {
    set_sem_val(agent->block_transactions,0, 1);

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

    check_if_received_by_all(agent);

    set_sem_val(agent->block_transactions, 0,0);
}

void check_if_received_by_all(struct Agent *agent) {
    int all_empty = 0;
    while(all_empty == 0) {
        int empty = 0;
        for (int i = 0; i < SMOKERS; i++) {
            if (check_queue_size(agent->smoker_queues[i]) == 0) {
                empty++;
            }
        }
        if (empty == 3) {
            all_empty = 1;
        }
    }
}

void wait() {
    sleep(get_random(10, 20));
}
