//wokwi-speed 100

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

#define BUTTON 5
#define BLUE_LED 11
#define RED_LED 12
#define GREEN_LED 13

void init_pin() {
    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON); //a função habilita o resistor pull-up interno

    gpio_init(BLUE_LED);
    gpio_set_dir(BLUE_LED, GPIO_OUT);
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);

    //Usando gpio_put(), garantimos que os leds iniciem desligados
    gpio_put(BLUE_LED, 0);
    gpio_put(RED_LED, 0);
    gpio_put(GREEN_LED, 0);
}

//Resumi em uma única função que mantenha os leds ligados
void leds_on() {
    gpio_put(BLUE_LED, 1);
    gpio_put(RED_LED, 1);
    gpio_put(GREEN_LED, 1);
}

volatile bool button_pressed = false;
volatile bool delay_active = false;

void gpio_callback(uint gpio, uint32_t events) {
    if (!delay_active && gpio == BUTTON) {
        button_pressed = true;
    }
}

int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    static int state = 0;

    if (state == 0) {
        gpio_put(BLUE_LED, 0);
        state = 1;
        add_alarm_in_ms(3000, turn_off_callback, NULL, true); //essa função agenda o próximo estágio do alarme (espera 3s para apagar o próximo led)
    } else if (state == 1) {
        gpio_put(RED_LED, 0); 
        state = 2;
        add_alarm_in_ms(3000, turn_off_callback, NULL, true); //essa função agenda o próximo estágio do alarme (espera 3s para apagar o próximo led)
    } else if (state == 2) {
        gpio_put(GREEN_LED, 0); 
        delay_active = false; //Serve para ativar o botão novamente, pois o código não irá reiniciar caso já esteja funcionando
        state = 0;
    }

    return 0; //Retorna 0 para indicar que o alarme não deve se repetir, somente se o botão for apertado novamente E o processo tenha sido concluído
}

int main() {

    bool ok;

    stdio_init_all();
    init_pin();
    
    //configuração do clock
    ok = set_sys_clock_khz(128000, false);

    //configura a interrupção para o botão
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true) {
        if (button_pressed) {
            button_pressed = false;
            delay_active = true;

            leds_on(); //Acende todos os leds para que assim a função turn_off_callback possa funcionar

            //Inicia o primeiro estágio do temporizador (aguarda 3s para apagar o led azul e assim a função turn_off_callback se repete)
            add_alarm_in_ms(3000, turn_off_callback, NULL, true);
        }
    }
}

//função opcional de debounce para o botão, para evitar fenomeno de trepidação
void debounce() {
    static uint32_t last_time = 0;
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if (gpio_get(BUTTON) == 0 && current_time - last_time > 200) {
        last_time = current_time;
    }
}