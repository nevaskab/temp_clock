#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#define RED_LED 11
#define YELLOW_LED 12
#define GREEN_LED 13

volatile int state_led = 0;


void init_pins() {
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_init(YELLOW_LED);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);
    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
}

//função que configura um temporizador periódico, ou seja, irá se repetir sem um ponto de partida ou um ponto final
bool repeating_timer_callback(struct repeating_timer *t) {
    gpio_put(RED_LED, 0);
    gpio_put(YELLOW_LED, 0);
    gpio_put(GREEN_LED, 0);

    if(state_led == 0) {
        gpio_put(RED_LED, 1);
    }
    else if(state_led == 1){
        gpio_put(YELLOW_LED, 1);
    }
    else {
        gpio_put(GREEN_LED, 1);
    }

    state_led = (state_led + 1) % 3;

    return true;
}


int main() {

    bool ok;

    stdio_init_all();
    init_pins();
    struct repeating_timer timer;

    //configuração de clock
    ok = set_sys_clock_khz(128000, false);

    //função que chama a função de temporizador periódico repeating_timer_callback além de determinar seu tempo de espera de 3s para cada estágio
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);

    //laço de repetição para exibir uma mensagem no console a cada 1s
    while (true) {
        printf("Você sabia? O semáforo de 3 cores foi criado em 1917. O primeiro semáforo criado em 1868 só possuia as cores vermelho e verde, além de usar luzes à gás!\n");
        sleep_ms(1000);
    }
}
