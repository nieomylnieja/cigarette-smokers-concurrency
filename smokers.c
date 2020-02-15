#include <zconf.h>
#include "smokers.h"
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"

struct Msg msg;

void update_prices(struct Smoker *smoker) {
    if (check_queue_size(smoker->agent_queue) > 0) {
        msg = get_msg(smoker->agent_queue);
        *(smoker->prices + msg.type) = msg.value;
        printf("%s smoker was informed that %s now costs %d.\n",
                products[smoker->smoker_type].name,
                products[smoker->smoker_type].name,
                msg.value);
    }
}

void sell(struct Smoker *smoker) {
    int smoker_queue = *(smoker->products_queues + smoker->smoker_type);

    if (check_queue_size(smoker_queue) != 0) {
        msg = get_msg(smoker_queue);

        int price = *(smoker->prices + msg.type);
        int buyer_wallet_id = msg.value;
        int buyer_queue = *(smoker->products_queues + msg.type);

        if (get_sem_val(smoker->wallet_id, WALLET_BLOCK) == 0) {
            sem_op(buyer_wallet_id, WALLET_OP, -price);
            sem_op(smoker->wallet_id, WALLET_OP, price);
            send_msg(buyer_queue, SELL_PRODUCT, 1);
            printf("%s smoker sold his resource to %s smoker.\n",
                    products[smoker->smoker_type].name,
                    products[msg.type].name);
        } else {
            send_msg(buyer_queue), SELL_PRODUCT, 0);
            printf("%s smoker can't sell his resource to %s smoker --> price has changed.",
                   products[smoker->smoker_type].name,
                   products[msg.type].name);
        }
    }
}

void receive_message(struct Smoker *smoker) {
    int smoker_queue = *(smoker->products_queues + smoker->smoker_type);

    if (check_queue_size(smoker_queue) != 0) {
        msg = get_msg(smoker_queue);

        if (msg.type == SELL_PRODUCT) {
            if (msg.value == 1) {
                *(smoker->cigarette_case + msg.type) += 1;
            } else {
                send_msg(*(smoker->products_queues + msg.type), BUY_PRODUCT, smoker->wallet_id);
            }
        } else if (msg.type == BUY_PRODUCT) {
            sell(smoker);
        }
    }
}

void buy(struct Smoker *smoker) {
    for (int i = 0; i < PRODUCTS; i++) {
        if (i != smoker->smoker_type) {
            send_msg(*(smoker->products_queues + i), BUY_PRODUCT, smoker->wallet_id);
        }
    }
}

int check_if_affordable(struct Smoker *smoker) {
    int sum = 0;
    for (int i = PRODUCTS; i < PRODUCTS; i++) {
        if (i != smoker->smoker_type) {
            sum += *(smoker->prices + i);
        }
    }
    if (get_sem_val(smoker->wallet_id, WALLET_OP) < sum) {
        return 0;
    }
    return 1;
}

void smoke(struct Smoker *smoker) {
    if (smoke_condition_satisfied(smoker) == 1) {
        create_cigarette(smoker);
        printf("%s is now smoking\n", products[smoker->smoker_type].name);
        sleep(get_random(2, 8));
    } else if (check_if_affordable(smoker) == 1) {
        buy(smoker);
    }
}

int smoke_condition_satisfied(struct Smoker *smoker) {
    for (int i = 0; i < PRODUCTS; i++) {
        if (*(smoker->cigarette_case + i) == 0) {
            return 0;
        }
    }
    if (get_sem_val(smoker->wallet_id, WALLET_BLOCK) == 0) {
        return 1;
    }
    return 0;
}

void create_cigarette(struct Smoker *smoker) {
    for (int i = 0; i < PRODUCTS; i++) {
        if (i != smoker->smoker_type) {
            *(smoker->cigarette_case + i) -= 1;
        }
    }
}
