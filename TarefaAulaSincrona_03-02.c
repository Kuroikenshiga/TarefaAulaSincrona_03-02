#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pio_led.pio.h"
#include "hardware/timer.h"
#include "inc/font.h"
#include "inc/ssd1306.h"
#include "hardware/uart.h"

const uint OUT_PIN = 7, PIN_BTN_A = 5, PIN_BTN_B = 6, PIN_LED_B = 12, PIN_LED_G = 11, I2C_SCL = 15, I2C_SDA = 14, UART_TX = 0, UART_RX = 1;
const uint MATRIX_DIMENSION = 5;
#define I2C_PORT i2c1
#define endereco 0x3C

// Apelida o tipo de dado "uinsigned char" para "uchar"
typedef unsigned char uchar;

uint32_t LAST_EVENT_A = 0, LAST_EVENT_B = 0;

// Matriz com todos os desenhos dos números
const float draws[10][5][5] = {

    // Desenhos que serão usados nos padrões de leds

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.0, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.1, 0.1, 0.0, 0.0},
     {0.1, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.1, 0.1, 0.1, 0.0}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.0},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.0},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.0},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.1, 0.0},
     {0.0, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.1, 0.0, 0.0, 0.0},
     {0.1, 0.0, 0.0, 0.0, 0.0}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1}},
    ///////////////////////////////////////////////////////////////////
};

PIO pio;
uint offset;
uint sm;
ssd1306_t ssd;

// Função que retorna a cor rgb em formato de um inteiro de 32 bits
uint32_t rgbColor(float r, float g, float b)
{
    // Trata os valores recebidos nos parâmetros da função para que não ultrapassem o valor 1 e também não fiquem abaixo do valor 0
    uchar R = (r > 1 || r < 0 ? 0.1 : r) * 255;
    uchar G = (g > 1 || g < 0 ? 0.1 : g) * 255;
    uchar B = (b > 1 || b < 0 ? 0.1 : b) * 255;

    return (R << 24) | (G << 16) | (B << 8);
}

void showNumberDraw(PIO *pio, uint *sm, int drawIndex);

void gpio_irq_handler(uint gpio, uint32_t events);

// Responsável por configurar todos os pinos e recursos que serão usados no projeto
void setup_project();

int main()
{

    stdio_init_all();

    setup_project();

    while (true)
    {
        if (stdio_usb_connected())
        {
            char c = '\0';
            // Lê um caractere da UART
            if (scanf("%c", &c) == 1)
            {
                if (c >= '0' && c <= '9')
                {
                    showNumberDraw(&pio, &sm, c - '0');
                }
            }
            char stringMsg[] = "Caractere Digitado  \0";
            stringMsg[(sizeof(stringMsg) / sizeof(char)) - 2] = c;
            ssd1306_draw_string(&ssd, stringMsg, 1, 1);
            ssd1306_send_data(&ssd);
        }
   
    }
    sleep_ms(50);
    return 0;
}
void showNumberDraw(PIO *pio, uint *sm, int drawIndex)
{
    // Verifica se o ponteiro 'drawIndex' aponta para um valor inválido
    if (drawIndex == -1)
        return;
    // Liga os Leds no padrão do desenho representado na matriz
    for (int j = 0; j < MATRIX_DIMENSION; j++)
    {
        for (int k = 0; k < MATRIX_DIMENSION; k++)
        {
            // Envia as informações de cor e intensidade do led para a maquina de estado
            pio_sm_put_blocking(*pio, *sm, rgbColor(draws[drawIndex][MATRIX_DIMENSION - 1 - j][(j + 1) % 2 == 0 ? k : MATRIX_DIMENSION - k - 1], 0.0, 0.0));
        }
    }
}
void gpio_irq_handler(uint gpio, uint32_t events)
{

    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (gpio == PIN_BTN_A)
    {
        // Condição para fazer o deboucing do botão A
        if (current_time - LAST_EVENT_A > 200000)
        {
            LAST_EVENT_A = current_time;
            gpio_put(PIN_LED_G,!gpio_get(PIN_LED_G));
            ssd1306_draw_string(&ssd, gpio_get(PIN_LED_G)?"Led VERDE ligado\0":"Led VERDE desligado\0", 1, 24);
            ssd1306_send_data(&ssd);
        }
    }

    else if (gpio == PIN_BTN_B)
    {
        // Condição para fazer o deboucing do botão B
        if (current_time - LAST_EVENT_B > 200000)
        {
            
            LAST_EVENT_B = current_time;
            gpio_put(PIN_LED_B,!gpio_get(PIN_LED_B));
            ssd1306_draw_string(&ssd, gpio_get(PIN_LED_B)?"Led AZUL ligado\0":"Led AZUL desligado\0", 1, 40);
            ssd1306_send_data(&ssd);
        }
    }
}
void setup_project()
{
    // Inicio da configuração inicial do firmaware
    pio = pio0;
    set_sys_clock_khz(128000, false);

    gpio_init(PIN_BTN_A);
    gpio_init(PIN_BTN_B);
    gpio_init(PIN_LED_G);
    gpio_init(PIN_LED_B);

    gpio_set_dir(PIN_BTN_A, GPIO_IN);
    gpio_set_dir(PIN_BTN_B, GPIO_IN);
    gpio_set_dir(PIN_LED_G, GPIO_OUT);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);

    gpio_pull_up(PIN_BTN_A);
    gpio_pull_up(PIN_BTN_B);
    // Configuração do PIO
    offset = pio_add_program(pio, &pio_led_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_led_program_init(pio, sm, offset, OUT_PIN);
    // Habilita a interrupção no GPIO
    gpio_set_irq_enabled_with_callback(PIN_BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Fim da configuração inicial do firmaware

    // Configuração do I2C e do monitor
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração do UART para testes no wokwi
    uart_init(uart0, 115200);
    gpio_set_function(UART_RX, GPIO_FUNC_UART);
    gpio_set_function(UART_TX, GPIO_FUNC_UART);
    uart_set_fifo_enabled(uart0, true);

    // Inicializa todos os leds apagados
    for (int k = 0; k < 25; k++)
    {
        // Envia as informações de cor e intensidade do led para a maquina de estado
        pio_sm_put_blocking(pio, sm, 0.0);
    }
}