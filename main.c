#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "ssd1306.h"  // Biblioteca para o display SSD1306 (I2C)

// Definição dos pinos
#define LED_RED_PIN 11     // GPIO 11 para o LED Vermelho
#define LED_GREEN_PIN 12   // GPIO 12 para o LED Verde
#define LED_BLUE_PIN 13    // GPIO 13 para o LED Azul
#define JOYSTICK_BUTTON_PIN 22  // GPIO 22 para o botão do joystick
#define BUTTON_A_PIN 5     // GPIO 5 para o botão A
#define JOYSTICK_X_PIN 26  // GPIO 26 para o eixo X do joystick
#define JOYSTICK_Y_PIN 27  // GPIO 27 para o eixo Y do joystick

// Definições para o display SSD1306
#define SSD1306_I2C_ADDRESS 0x3C
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// Variáveis globais
bool green_led_state = false;  // Estado do LED Verde
bool pwm_enabled = true;       // Estado do PWM (ligado/desligado)
uint8_t border_style = 0;      // Estilo da borda do display

// Função para configurar o PWM nos pinos dos LEDs
void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_num, 4095);  // 12 bits de resolução
    pwm_set_enabled(slice_num, true);
}

// Função para ajustar o brilho do LED via PWM
void set_led_brightness(uint pin, uint16_t value) {
    pwm_set_gpio_level(pin, value);
}

// Função de interrupção para o botão do joystick
void joystick_button_irq(uint gpio, uint32_t events) {
    static uint64_t last_press_time = 0;
    if (to_ms_since_boot(get_absolute_time()) - last_press_time > 200) {  // Debouncing
        green_led_state = !green_led_state;  // Alterna o estado do LED Verde
        gpio_put(LED_GREEN_PIN, green_led_state);

        // Alterna o estilo da borda do display
        border_style = (border_style + 1) % 3;
        last_press_time = to_ms_since_boot(get_absolute_time());
    }
}

// Função de interrupção para o botão A
void button_a_irq(uint gpio, uint32_t events) {
    static uint64_t last_press_time = 0;
    if (to_ms_since_boot(get_absolute_time()) - last_press_time > 200) {  // Debouncing
        pwm_enabled = !pwm_enabled;  // Alterna o estado do PWM
        if (!pwm_enabled) {
            set_led_brightness(LED_RED_PIN, 0);
            set_led_brightness(LED_BLUE_PIN, 0);
        }
        last_press_time = to_ms_since_boot(get_absolute_time());
    }
}

// Função principal
int main() {
    // Inicialização do hardware
    stdio_init_all();
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Configuração dos LEDs
    setup_pwm(LED_RED_PIN);
    setup_pwm(LED_BLUE_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    // Configuração dos botões com interrupções
    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &joystick_button_irq);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_a_irq);

    // Inicialização do display SSD1306
    ssd1306_t disp;
    ssd1306_init(&disp, SSD1306_WIDTH, SSD1306_HEIGHT, SSD1306_I2C_ADDRESS, i2c_default);
    ssd1306_clear(&disp);

    // Loop principal
    while (true) {
        // Leitura dos valores do joystick
        adc_select_input(0);
        uint16_t x_value = adc_read();  // Eixo X
        adc_select_input(1);
        uint16_t y_value = adc_read();  // Eixo Y

        // Controle dos LEDs RGB via PWM
        if (pwm_enabled) {
            set_led_brightness(LED_RED_PIN, x_value);
            set_led_brightness(LED_BLUE_PIN, y_value);
        }

        // Atualização do display SSD1306
        ssd1306_clear(&disp);
        int x_pos = (x_value * SSD1306_WIDTH) / 4095;  // Mapeia o eixo X para a largura do display
        int y_pos = (y_value * SSD1306_HEIGHT) / 4095; // Mapeia o eixo Y para a altura do display
        ssd1306_draw_square(&disp, x_pos, y_pos, 8, 8, 1);  // Desenha um quadrado 8x8

        // Aplica o estilo da borda
        if (border_style == 1) {
            ssd1306_draw_rectangle(&disp, 0, 0, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 1);
        } else if (border_style == 2) {
            ssd1306_draw_circle(&disp, SSD1306_WIDTH / 2, SSD1306_HEIGHT / 2, 30, 1);
        }

        ssd1306_show(&disp);
        sleep_ms(10);  // Atraso para evitar flicker
    }
}
