# WLS

# Controle de Joystick e LEDs RGB com Display SSD1306

Este projeto tem como objetivo controlar LEDs RGB e exibir a posição de um joystick em um display SSD1306 utilizando o Raspberry Pi Pico W. O código foi desenvolvido em C com o Pico SDK e integrado à placa BitDogLab.

---

## Funções Gerais

### 1. `setup_pwm(uint pin)`
- **Motivo**: Configura o PWM no pino especificado para controlar a intensidade dos LEDs RGB.
- **Uso**: Inicializa o PWM nos pinos dos LEDs Vermelho e Azul.

### 2. `set_led_brightness(uint pin, uint16_t value)`
- **Motivo**: Ajusta o brilho do LED via PWM com base no valor do joystick.
- **Uso**: Controla a intensidade dos LEDs Vermelho e Azul.

### 3. `joystick_button_irq(uint gpio, uint32_t events)`
- **Motivo**: Alterna o estado do LED Verde e muda o estilo da borda do display quando o botão do joystick é pressionado.
- **Uso**: Implementa a funcionalidade do botão do joystick com debouncing.

### 4. `button_a_irq(uint gpio, uint32_t events)`
- **Motivo**: Ativa ou desativa o controle PWM dos LEDs Vermelho e Azul quando o botão A é pressionado.
- **Uso**: Implementa a funcionalidade do botão A com debouncing.

### 5. `main()`
- **Motivo**: Função principal que coordena a leitura do joystick, o controle dos LEDs e a atualização do display.
- **Uso**: Inicializa o hardware, lê os valores do joystick, controla os LEDs e exibe a posição do joystick no display.

---

## Motivos das Funções

- **Controle de LEDs RGB**: Os LEDs são controlados via PWM para permitir variação suave da intensidade luminosa com base nos valores do joystick.
- **Display SSD1306**: A posição do joystick é representada por um quadrado móvel no display, com bordas que mudam de estilo ao pressionar o botão do joystick.
- **Interrupções**: Todas as funcionalidades dos botões são implementadas usando interrupções para garantir respostas rápidas e eficientes.
