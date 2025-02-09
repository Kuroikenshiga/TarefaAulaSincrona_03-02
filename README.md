## Tarefa da aula síncrona do dia 03 de fevereiro de 2025

### 🗒️Enunciado:
Para consolidar os conceitos sobre o uso de interfaces de comunicação serial no RP2040 e explorar
as funcionalidades da placa de desenvolvimento BitDogLab, propõe-se a realização da seguinte tarefa
individual prática.

### 📝Objetivos:
- Compreender o funcionamento e a aplicação de comunicação serial em microcontroladores.
- Aplicar os conhecimentos adquiridos sobre UART e I2C na prática.
- Manipular e controlar LEDs comuns e LEDs endereçáveis WS2812.
- Fixar o estudo do uso botões de acionamento, interrupções e Debounce.
- Desenvolver um projeto funcional que combine hardware e software.

### ❗Requisitos para execução do projeto (Software)
- Compilador Arm gcc instalado
- PICO SDK intalado
- Visual Studio Code instalado
- Extensão do Respberry Pi Pico instalada
- Extensão do Wokwi Simulator instalada e configurada
- Extensões Cmake e Cmake tools instaladas 
- Serial monitor instalado no Visual Studio Code
### ❗Requisitos para execução do projeto (Hardware)
- Kit de desenvolvimento BitDogLab

### Instruções para a execução do projeto

1. Baixe o projeto na sua maquina atráves do cmd pelo comando: 
    ```bash 
    git clone https://github.com/Kuroikenshiga/TarefaAulaSincrona_03-02.git
2. Abra o Visual Studio Code.
3. No menu lateral esquerdo, encontre a extensão do Respberry Pi Pico e clique nela.
4. Selecione a opção "Import Project"
5. No campo "Location" clique no botão "Change" e selecione o projeto que você acabou de baixar na sua maquina.
6. Clique no botão "Import"
7. Após carregar todos os arquivos necessários, clique na opção "Compile" presente no menu inferior direito.

### Instruções de uso do projeto

Utilize o Serial monitor e os botões para enviar caracteres e comandos para a placa e assim realizar diferentes comportamentos, sendo eles:<br>
1. Mostrar Mensagens apresentando o caractere digitado no monitor SSD1306 da placa BitDogLab;
2. Acender os leds endereçáveis WS2812 no padrão do formato númerico, caso seja passado um número no serial monitor;
3. Acender ou apagar os leds verde e azul apertando os botões 'A' e 'B' da placa.
4. Apresentar o estado dos leds no monitor SSD1306(Apagados/Acesos).

### Repositório de onde as fontes das letras minúsculas foram selecionadas para o projeto
### [oled-font-pack](https://github.com/lynniemagoo/oled-font-pack/tree/master?tab=MIT-1-ov-file)