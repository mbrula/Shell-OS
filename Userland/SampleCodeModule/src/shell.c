#include <clib.h>
#include <shell.h>
#include <utils.h>

static char * command_strings[] = {"help", "date", "time", "sleep", "clear", "beep", "door", "div_zero", "inv_op", "exit"};
static int command_count = 10;
void (* command_functions[]) (void) = {help_cmd, date_cmd, time_cmd, sleep_cmd, clear_cmd, beep_cmd, door_cmd, div_zero_cmd, inv_op_cmd, exit_cmd};

#define MAX_LENGTH  50

static void newLine(){
    putchar('\n');
}

void initShell() {
    initScreen();
    int command = NO_CMD;
    char input[MAX_LENGTH];

    while (command != EXIT_CMD) {
        puts(PROMPT_STRING);
        gets(input, MAX_LENGTH);
        command = getCommand(input);
        executeCommand(command);
        if (command != CLEAR_CMD) newLine();
    }
	
	exit();
}

void initScreen() {
    clearScreen();
    puts("Bienvenido al programa. El comando help lo ayudara\n");
}

int getCommand(char * input){
    for (int i = 0; i < command_count; i++) {
        if (strcmp(input, command_strings[i]) == 0)
            return i;
    }
    return NO_CMD;
}

void executeCommand(int command) {
    if (command != NO_CMD)
        command_functions[command]();
    else
        puts("\nInvalid command");
}

void help_cmd() {
    puts("\nLos comandos validos son los siguientes: ");
    puts("\nhelp ~ Muestra los comandos validos");
    puts("\ndate ~ Muestra la fecha actual");
    puts("\ntime ~ Muestra la hora actual");
    puts("\nsleep ~ Frena el funcionamiento un numero de segundos a ingresar");
    puts("\nclear ~ Limpia la consola");
    puts("\nbeep ~ Emite un sonido");
    puts("\ndoor ~ Hay alguien en la puerta");
    puts("\ndiv_zero ~ Ejecuta una division por cero");
    puts("\ninv_op ~ Ejecuta una operacion de codigo invalido ");
    puts("\nexit ~ Termina la ejecucion");
}

void date_cmd() {
    char date[11];
    puts("\nHoy es  ");
	puts(getDate(date));
}

void time_cmd() {
    char time[9];
    puts("\nSon las  ");
	puts(getTime(time));
}

void sleep_cmd() {
    char car;
    do{
        puts("\nIngrese el numero de segundos que desea esperar [0-9]: ");
        car = getche();
        putchar(car);
    } while (!isNumber(car));
    int millis = (car - '0') * 1000;
    sleep(millis);
}

void clear_cmd() {
    clearScreen();
}

void beep_cmd() {
    beep(BEEP_FREQ, 300);
}

void door_cmd() {
    beep(DOOR_FREQ, 300);
    sleep(300);
    beep(DOOR_FREQ, 150);
    sleep(150);
    beep(DOOR_FREQ, 150);
    sleep(150);
    beep(DOOR_FREQ, 150);
    sleep(250);
    beep(DOOR_FREQ, 150);
    sleep(800);
    beep(DOOR_FREQ, 150);
    sleep(150);
    beep(DOOR_FREQ, 150);
}

void div_zero_cmd() {
// Hace falta el driver de video, paja cambiar exceptions.c
    int a = 10, b = 0;
    int c = a / b;
    // perror("\nERROR DIVISION POR CERO");
}

void inv_op_cmd() {
// Hace falta el driver de video, paja cambiar exceptions.c
    uint64_t invalid = 0xFFFFFFFFFFFF;
	uint64_t * ptr = &invalid;
	((void(*)())ptr)();
    // perror("\nERROR OPCODE INVALIDO");
}

void exit_cmd() {
    puts("\nHasta Luego");
}