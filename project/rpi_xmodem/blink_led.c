
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "stdio.h"
#include "pico/cyw43_arch.h"

//Unlike the original Raspberry Pi Pico, the on-board LED on Pico W is not connected 
//to a pin on RP2040, but instead to a GPIO pin on the wireless chip

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
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);

    while(1)
    {
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

    //Configure ADC
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4); 


    while (true) {
        uint16_t raw = adc_read();
        // multicore_fifo_push_blocking(raw);

        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        printf("status %d \n\r", cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
        sleep_ms(1000);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        printf("status %d \n\r", cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
        // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        // printf("LED ON\n\r");
        // sleep_ms(1000);

        // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        // printf("LED OFF\n\r");
        // sleep_ms(1000);
    }
}