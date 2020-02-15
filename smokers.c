#include <zconf.h>
#include "smokers.h"
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"

struct Msg msg;

void update_prices(struct Smoker *smoker) {
    if (check_queue_size(smoker->agent_queue) > 0) {
        msg = get_msg(smoker->agent_queue);
        printf("%s smoker was informed that %s now costs %d.\n",
                products[smoker->smoker_type].name,
                products[smoker->smoker_type].name,
                msg.value);
    }
}

void sell(struct Smoker *smoker) {
    if (get_sem_val(smoker->agent_block, 0) == 0) {
        printf("%s smoker sold \n", products[smoker->smoker_type].name);
    } else {
        printf("%s smoker IS BLOCKED\n", products[smoker->smoker_type].name);
    }
}

void send_request(int msqid, int smoker_id) {
    send_msg(msqid, smoker_id, smoker_id);
}

void buy(struct Smoker *smoker) {
    if (get_sem_val(smoker->agent_block, 0) == 0) {
        printf("%s smoker bought \n", products[smoker->smoker_type].name);
    } else {
        printf("%s smoker IS BLOCKED\n", products[smoker->smoker_type].name);
    }
}

void smoke(struct Smoker *smoker) {
    if (smoke_condition_satisfied(smoker) == 1) {
        create_cigarette(smoker);
        printf("%s is now smoking\n", products[smoker->smoker_type].name);
        sleep(get_random(2, 8));
    }
}

int smoke_condition_satisfied(struct Smoker *smoker) {
    for (int i = 0; i < PRODUCTS; i++) {
        if (*(smoker->cigarette_case + i) == 0) {
            return 0;
        }
    }
    if (get_sem_val(smoker->agent_block, 0) == 0) {
        return 1;
    }
    return 0;
}

void create_cigarette(struct Smoker *smoker) {
    for (int i = 0; i < PRODUCTS; i++) {
        if (*(smoker->cigarette_case + i) != smoker->smoker_type) {
            printf("%d");
            *(smoker->cigarette_case + i) -= 0;
        }
    }
}
