#include "smoker.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"


void receive_message(struct Smoker *smoker);

void buy(struct Smoker *smoker);

void buy_one(int p_type, struct Smoker *smoker);

void update_prices(struct Smoker *smoker);

int smoke_condition_satisfied(struct Smoker *smoker);

int check_if_affordable(struct Smoker *smoker);

void create_cigarette(struct Smoker *smoker);

void smoke(struct Smoker *smoker);

int check_if_affordable_one(struct Smoker *smoker, int product);

char text_buf[80];

void smoker_do(struct Smoker *smoker) {
    update_prices(smoker);
    smoke(smoker);
    receive_message(smoker);
}

void update_prices(struct Smoker *smoker) {
    struct Msg agent_msg;

    if (check_queue_size(smoker->agent_queue) > 0) {
        agent_msg = get_msg(smoker->agent_queue);
        *(smoker->prices + agent_msg.content.product_type) = agent_msg.content.price;

        if (verbose == 1) {
            sprintf(text_buf, "%s smoker was informed that %s now costs %d.",
                    smokers[smoker->smoker_type].name,
                    smokers[smoker->smoker_type].name,
                    agent_msg.content.price);
            color_print(text_buf, smoker->text_color);
        }
    }
}

void sell(struct Smoker *smoker, struct Msg request) {
    struct Msg response;

    int price = *(smoker->prices + smoker->smoker_type);
    int buyer_wallet_id = request.content.wallet_id;
    int buyer_queue = *(smoker->exchange_queues + request.content.sender_type);

    response.content.sender_type = smoker->smoker_type;

    if (get_sem_val(smoker->wallet_id, WALLET_BLOCK) == 0) {
        sem_op(buyer_wallet_id, WALLET_OP, -price);
        sem_op(smoker->wallet_id, WALLET_OP, price);

        response.type = PRODUCT_SOLD;
        send_msg(buyer_queue, response);

        if (verbose == 1) {
            sprintf(text_buf, "%s smoker sold his resource to %s smoker for %d.",
                    smokers[smoker->smoker_type].name,
                    smokers[request.content.sender_type].name,
                    price);
            color_print(text_buf, smoker->text_color);
        }
    } else {
        response.type = TRANSACTION_CANCELLED;
        if (verbose == 1) {
            sprintf(text_buf, "%s <--> %s smokers transaction was cancelled due to price change.",
                    smokers[smoker->smoker_type].name,
                    smokers[request.content.sender_type].name);
            color_print(text_buf, smoker->text_color);
        }
        send_msg(buyer_queue, response);
    }
}

void receive_message(struct Smoker *smoker) {
    struct Msg request;
    int smoker_queue = *(smoker->exchange_queues + smoker->smoker_type);

    if (check_queue_size(smoker_queue) != 0) {
        request = get_msg(smoker_queue);

        if (request.type == PRODUCT_SOLD) {
            (smoker->cigarette_case + request.content.sender_type)->value += 1;

            if (verbose == 1) {
                sprintf(text_buf, "%s smoker has bought %s.",
                        smokers[smoker->smoker_type].name,
                        smokers[request.content.sender_type].name);
                color_print(text_buf, smoker->text_color);
            }
        } else if (request.type == TRANSACTION_CANCELLED) {
            (smoker->cigarette_case + request.content.sender_type)->status = FREE;
        } else if (request.type == BUY_PRODUCT) {
            sell(smoker, request);
        } else {
            perror("That line of code shouldn't have been reached! [smoker.c:92]\n");
            exit(EXIT_FAILURE);
        }
    }
}

void buy(struct Smoker *smoker) {
    for (int p_type = 0; p_type < PRODUCTS; p_type++) {
        if ((p_type != smoker->smoker_type) && (smoker->cigarette_case + p_type)->status != PENDING) {
            if (verbose == 1) {
                sprintf(text_buf, "%s is trying to buy %s",
                        smokers[smoker->smoker_type].name,
                        smokers[p_type].name);
                color_print(text_buf, smoker->text_color);
            }
            buy_one(p_type, smoker);
        }
    }
}

void buy_one(int p_type, struct Smoker *smoker) {
    (smoker->cigarette_case + p_type)->status = PENDING;

    struct Msg request;
    request.type = BUY_PRODUCT;
    request.content.sender_type = smoker->smoker_type;
    request.content.wallet_id = smoker->wallet_id;

    send_msg(*(smoker->exchange_queues + p_type), request);
}

int check_if_affordable(struct Smoker *smoker) {
    int sum = 0;
    for (int p_type = 0; p_type < PRODUCTS; p_type++) {
        if ((p_type != smoker->smoker_type) && ((smoker->cigarette_case + p_type)->status != PENDING)) {
            sum += *(smoker->prices + p_type);
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
        sprintf(text_buf, "%s is now smoking", smokers[smoker->smoker_type].name);
        color_print(text_buf, smoker->text_color);
        sleep(get_random(2, 8));
    } else if (check_if_affordable(smoker) == 1) {
        buy(smoker);
    }
}

int smoke_condition_satisfied(struct Smoker *smoker) {
    for (int i = 0; i < PRODUCTS; i++) {
        if ((smoker->cigarette_case + i)->value == 0) {
            return 0;
        }
    }
    return 1;
}

void create_cigarette(struct Smoker *smoker) {
    for (int i = 0; i < PRODUCTS; i++) {
        if (i != smoker->smoker_type) {
            (smoker->cigarette_case + i)->status = FREE;
            (smoker->cigarette_case + i)->value -= 1;
        }
    }
}

void set_wallet(struct Smoker *smoker, int money) {
    set_sem_val(smoker->wallet_id, WALLET_OP, money);
}
