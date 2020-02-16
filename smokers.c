#include <zconf.h>
#include "smokers.h"
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"

struct Msg agent_msg;
struct Msg request;
struct Msg response;

void update_prices(struct Smoker *smoker) {
    if (check_queue_size(smoker->agent_queue) > 0) {
        agent_msg = get_msg(smoker->agent_queue);
        *(smoker->prices + agent_msg.content.product_type) = agent_msg.content.price;
        printf("%s smoker was informed that %s now costs %d.\n",
                products[smoker->smoker_type].name,
                products[smoker->smoker_type].name,
                agent_msg.content.price);
    }
}

void sell(struct Smoker *smoker) {
    int smoker_queue = *(smoker->exchange_queues + smoker->smoker_type);

    if (check_queue_size(smoker_queue) != 0) {
        request = get_msg(smoker_queue);

        int price = *(smoker->prices + smoker->smoker_type);
        int buyer_wallet_id = request.content.wallet_id;
        int buyer_queue = *(smoker->exchange_queues + request.content.product_type);

        response.content.sender_type = smoker->smoker_type;

        if (get_sem_val(smoker->wallet_id, WALLET_BLOCK) == 0) {
            sem_op(buyer_wallet_id, WALLET_OP, -price);
            sem_op(smoker->wallet_id, WALLET_OP, price);

            response.type = PRODUCT_SOLD;
            send_msg(buyer_queue, response);

            printf("%s smoker sold his resource to %s smoker for %d.\n",
                    products[smoker->smoker_type].name,
                    products[request.content.sender_type].name,
                    price);
        } else {
            response.type = TRANSACTION_CANCELLED;
            send_msg(buyer_queue, response);

            printf("%s smoker can't sell his resource to %s smoker --> price has changed.\n",
                   products[smoker->smoker_type].name,
                   products[request.content.sender_type].name);
        }
    }
}

void receive_message(struct Smoker *smoker) {
    int smoker_queue = *(smoker->exchange_queues + smoker->smoker_type);

    if (check_queue_size(smoker_queue) != 0) {
        response = get_msg(smoker_queue);

        if (response.type == PRODUCT_SOLD) {
            *(smoker->cigarette_case + response.content.sender_type) += 1;

            printf("%s smoker has bought %s.\n",
                   products[smoker->smoker_type].name,
                   products[response.content.sender_type].name);
        } else if (response.type == TRANSACTION_CANCELLED) {
            buy_one(response.content.sender_type, smoker);
        } else if (response.type == BUY_PRODUCT) {
            sell(smoker);
        }
    }
}

void buy(struct Smoker *smoker) {
    set_sem_val(smoker->wallet_id, PENDING, 1);

    for (int i = 0; i < PRODUCTS; i++) {
        if (i != smoker->smoker_type) {
            buy_one(i, smoker);
        }
    }
}

void buy_one(int product_type, struct Smoker *smoker) {
    request.type = BUY_PRODUCT;
    request.content.sender_type = smoker->smoker_type;
    request.content.wallet_id = smoker->wallet_id;

    send_msg(*(smoker->exchange_queues + product_type), request);
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
        set_sem_val(smoker->wallet_id, PENDING, 0);
        printf("%s is now smoking\n", products[smoker->smoker_type].name);
        sleep(get_random(2, 8));
    } else if (check_if_affordable(smoker) == 1 && get_sem_val(smoker->wallet_id, PENDING) == 0) {
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

void set_wallet(struct Smoker *smoker, int money) {
    set_sem_val(smoker->wallet_id, WALLET_OP, money);
}
