#include "agent.h"
#include <stdio.h>
#include <unistd.h>
#include "shared.h"


void block_all_wallets(int *wallets);

void set_price(struct Agent *agent);

void wait();

void make_sure_everyone_was_informed(struct Agent *agent);

void inform_smokers(int *msq_id, int product_type, int price);

static struct Msg msg;

char text_buf[30];

void agent_do(struct Agent *agent) {
    set_price(agent);
    wait();
}

void set_price(struct Agent *agent) {
    block_all_wallets(agent->wallets);

    int product = get_random(0, 2);
    int price = get_random(1, 5);
    if (product == 0) {
        agent->tobacco_price = price;
    } else if (product == 1) {
        agent->paper_price = price;
    } else if (product == 2) {
        agent->matches_price = price;
    }

    sprintf(text_buf, "\nAgent set price of %s to %d.\n", smokers[product].name, price);
    color_print(text_buf, agent->text_color);

    inform_smokers(agent->smoker_queues, product, price);

    make_sure_everyone_was_informed(agent);
}

void inform_smokers(int *msq_id, int product_type, int price) {
    for (int i = 0; i < SMOKERS; i++) {
        msg.type = INFORM;
        msg.content.product_type = product_type;
        msg.content.price = price;
        send_msg(*(msq_id + i), msg);
    }
}

void block_all_wallets(int *wallets) {
    for (int i = 0; i < SMOKERS; i++) {
        set_sem_val(*(wallets + i), WALLET_BLOCK, 1);
    }
}

void make_sure_everyone_was_informed(struct Agent *agent) {
    int checked[3] = {0, 0, 0};
    int empty = 0;
    while (empty != 3) {
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
    sleep(get_random(15, 30));
}
