
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "stdio.h"
#include "pico/cyw43_arch.h"

//Unlike the original Raspberry Pi Pico, the on-board LED on Pico W is not connected 
//to a pin on RP2040, but instead to a GPIO pin on the wireless chip

#define UART_ID uart1
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5



void core1_interrupt_handler()
{
    printf("irq\n\r");
    while(multicore_fifo_rvalid())
    {
        uint16_t raw = multicore_fifo_pop_blocking();
        const float conversion_factor = 3.3f / (1<<12);
        float result = raw * conversion_factor;
        float temp = 27 - (result - 0.706)/0.001721;
        printf("Temp = %f c\n\r", temp);
    }

    multicore_fifo_clear_irq(); // clear interrupt
}

void core1_entry()
{
    printf("starting core 1\n\r");
    multicore_fifo_clear_irq();
    // irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    // irq_set_enabled(SIO_IRQ_PROC1, true);

    uint8_t occurence = 0;
    char s[18] = {0};
    while(1)
    {
        sprintf(s, "Hello, UART %d!\n\r", occurence);
        uart_puts(UART_ID, s);
        occurence ++;
        tight_loop_contents();
    }

}

//core0
int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return -1;
    }

    sleep_ms(1);

    multicore_launch_core1(core1_entry); //start core1

    //configure uart 2
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    //Configure ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4); 

    while (true) {
        uint16_t raw = adc_read();
        // multicore_fifo_push_blocking(raw);

        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
        printf("status %d \n\r", cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN)); //status 0 = eteint | 1 = allumer
    }
}
