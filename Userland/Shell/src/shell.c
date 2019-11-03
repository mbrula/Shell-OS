// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <clib.h>
#include <shell.h>
#include <utils.h>

/* Valid command inputs */
static char * command_strings[] = {"help", "date", 
                                        "time", "sleep", "clear", "beep", "door", "div_zero", 
                                        "inv_op", "exit", "mem", "ps", "kill", "block", "nice", 
                                        "loop", "cat", "wc", "filter", "sem", "pipe", "phylo", "test"};
static int command_count = 23;
/* Command functions associated with command inputs */
void (* command_functions[]) (int argc, char * argv[], int ground, int inFd, int outFd) = {help_cmd, date_cmd, 
                                            time_cmd, sleep_cmd, clear_cmd, beep_cmd, door_cmd, div_zero_cmd, 
                                            inv_op_cmd, exit_cmd, mem_cmd, ps_cmd, kill_cmd, block_cmd, nice_cmd,
                                            loop_cmd, cat_cmd, wc_cmd, filter_cmd, sem_cmd, pipe_cmd, phylo_cmd, test_cmd};

/* Get command index from string input and save cursor in parameter */
static int getCommandIndex(char * input, int * cursor);

/* Actual number for pipes */
static int pipeNumber = 0;

// MAX_LENGTH esta en clib.h

/* Prints new line */
static void newLine(){
    putchar('\n');
}

int main (int argc, char const *argv[]) {
    initShell();
    return 0;
}

/* Main shell Core */
void initShell() {
    initScreen();
    int command = NO_CMD;
    char input[MAX_LENGTH] = {0};

    while (command != EXIT_CMD) {
        puts(PROMPT_STRING);
        /* Get an input line */
        gets(input, MAX_LENGTH);
        /* Process the input recieved */
        if ((command = getCommand(input)) == NO_CMD)
            puts("\nInvalid command");

        if (command != CLEAR_CMD) newLine();
    }
	
    /* Exit console */
	exit();
}

/* Initialize screen */
void initScreen() {
    clearScreen();
    puts("Bienvenido al programa. El comando help lo ayudara\n");
}

/* Process the input recieved and execute the corresponding commands */
int getCommand(char * input){
    int cursor = 0;
    char vec[MAX_LENGTH] = {0};
    int j = 0;
    for (j = 0; j < MAX_LENGTH; j++){
        vec[j] = 0;
    }

    char pipeName[20] = SEM_PIPE_NAME;
    char pipeNumberBuf[5];

    int command = NO_CMD;
    int nextStdin = STDIN;
    int nextCmd = 0;
    /* Command Loop */
    do {
        nextCmd = 0;
        /* Get the index of the written command if valid. Return if not valid */
        command = getCommandIndex(input + cursor, &cursor);
        if (command == NO_CMD)
            return command;

        char arg1[MAX_LENGTH] = {0}, arg2[MAX_LENGTH] = {0};
        char * argv[] = {arg1, arg2, 0};
        int argc = 0;
        int haveArgs = 0;
        /* Argument Loop */
        do {
            /* Get the next argument (Spaces split arguments) */
            if (input[cursor] == ' ')
                cursor++;

            haveArgs = strcpyUntilSpace(vec, input + cursor);
            cursor += haveArgs;
            /* There are still arguments to process */
            if (haveArgs != 0) {
                switch (vec[0]) {
                    case '&':
                        /* Create a background process that executes the command (if not a built-in) */
                        command_functions[command](argc, argv, BACKGROUND, nextStdin, STDOUT);
                        haveArgs = 0;
                        break;
                    case '|':
                        /* Create a special named pipe and run the first command, fixing its STDOUT to the pipe. */
                        /* A new command is expected after the pipe */
                        cursor++;
                        strcpy(pipeName, SEM_PIPE_NAME);
                        itoa(pipeNumber++, pipeNumberBuf, 10);
                        int pipeFd = newPipe(strcat(pipeName, pipeNumberBuf));
                        command_functions[command](argc, argv, FOREGROUND, nextStdin, pipeFd);
                        pipeClose(pipeFd);
                        /* Change STDIN for next command to read from pipe. */
                        nextStdin = pipeFd;
                        nextCmd = 1;
                        haveArgs = 0;
                        argc = 0;
                        break;
                    default:
                        /* Save the argument if room's available */
                        if (argc < 2)
                            strcpy(argv[argc++], vec);
                        break;
                }
            } else {
                /* Create a foreground process that executes the command (if not a built-in) */
                command_functions[command](argc, argv, FOREGROUND, nextStdin, STDOUT);
            }
        } while (haveArgs != 0);
    } while (nextCmd != 0);

    return command;
}

/* Compare input until space to check which command was entered */
static int getCommandIndex(char * input, int * cursor) {
    int initCursor = *cursor;
    int i = 0;
    for (i = 0; i < command_count; i++) {
        if (strcmpUntilSpace(input, command_strings[i], cursor) == 0){
            // (*cursor)++;
            return i;
        }
        *cursor = initCursor;
    }
    return NO_CMD;
}

/* Help Built-in Command - Shows available commands */
void help_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    putsFd("\nLos comandos validos son los siguientes: ", outFd);
    putsFd("\nhelp ~ Muestra los comandos validos", outFd);
    putsFd("\ndate ~ Muestra la fecha actual", outFd);
    putsFd("\ntime ~ Muestra la hora actual", outFd);
    putsFd("\nsleep ~ Frena el funcionamiento un numero de segundos a ingresar", outFd);
    putsFd("\nclear ~ Limpia la consola", outFd);
    putsFd("\nbeep ~ Emite un sonido", outFd);
    putsFd("\ndoor ~ Hay alguien en la puerta", outFd);
    putsFd("\ndiv_zero ~ Ejecuta una division por cero", outFd);
    putsFd("\ninv_op ~ Ejecuta una operacion de codigo invalido", outFd);

    putsFd("\nmem ~ Imprime el estado de la memoria", outFd);
    putsFd("\nps ~ Imprime la lista de todos los procesos con sus propiedades", outFd);
    putsFd("\nkill ~ Mata al proceso de ID recibido", outFd);
    putsFd("\nnice ~ Cambia la prioridad del proceso de ID recibido a la prioridad recibida", outFd);
    putsFd("\nblock ~ Cambia el estado del proceso de ID recibido entre BLOCKED y READY", outFd);
    putsFd("\nloop ~ Ejecuta un proceso que imprime su ID con un saludo cada 3s en un loop", outFd);
    putsFd("\ncat ~ Ejecuta un proceso que imprime el STDIN tal como lo recibe", outFd);
    putsFd("\nwc ~ Ejecuta un proceso que cuenta la cantidad de lineas del input", outFd);
    putsFd("\nfilter ~ Ejecuta un proceso que filtra las vocales del input", outFd);
    putsFd("\nsem ~ Imprime la lista de todos los semaforos con sus propiedades", outFd);
    putsFd("\npipe ~ Imprime la lista de todos los pipes con sus propiedades", outFd);
    putsFd("\nphylo ~ Ejecuta un proceso que implementa el problema de los filosofos comensales", outFd);

    putsFd("\nexit ~ Termina la ejecucion", outFd);
    putsFd("\n", outFd);
}

/* Date Built-in Command - Shows date */
void date_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    char date[11];
    printfFd(outFd, "\nHoy es %s\n", getDate(date));
}

/* Time Built-in Command - Shows time */
void time_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    char time[9];
    printfFd(outFd, "\nSon las %s\n", getTime(time));
}

/* Sleep Command - Creates a process that stays blocked n seconds */
void sleep_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    if (argc < 1 || atoi(argv[0]) < 0) {
        puts("\nIngreso invalido. Debe ingresar el numero de segundos que desea esperar como primer argumento.");
    } else {
        int pid = newProcess("SLEEP", argc, argv, ground, inFd, outFd);
        if (ground == BACKGROUND)
            printf("\nCreate %s. PID = %d", (pid == 0) ? "unsuccesfull":"successfull", pid);
    }
}

/* Clear Built-in Command - Clears terminal */
void clear_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    clearScreen();
}

/* Beep Built-in Command - Plays a beep */
void beep_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    beep(BEEP_FREQ, 300);
}

/* Door Built-in Command - Plays a sequence of beeps */
void door_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    beep(DOOR_FREQ, 300);
    sleep(300);
    beep(DOOR_FREQ, 150);
    sleep(150);
    beep(DOOR_FREQ, 150);
    sleep(100);
    beep(DOOR_FREQ, 150);
    sleep(250);
    beep(DOOR_FREQ, 150);
    sleep(800);
    beep(DOOR_FREQ, 150);
    sleep(150);
    beep(DOOR_FREQ, 150);
}

/* Div_Zero Built-in Command - Executes a division by zero to trigger an exception */
void div_zero_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    int a = 10, b = 0;
    a = a / b;
    printf("%d", a);
}

/* Inv_Op Built-in Command - Executes an invalid operation to trigger an exception */
void inv_op_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    uint64_t invalid = 0xFFFFFFFFFFFF;
	uint64_t * ptr = &invalid;
	((void(*)())ptr)();
}

/* Exit Built-in Command - Prints a goodbye message. The main core will exit */
void exit_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    puts("\nHasta Luego");
}

/* --------------------------------------- */

/* Mem Built-in Command - Prints current memory status */
void mem_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    memStatus(outFd);
}

/* Ps Built-in Command - Prints current process list & status */
void ps_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    ps(outFd);
}

/* Kill Built-in Command - Kills the process with PID entered via arguments */
void kill_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    int pid;
    if (argc < 1 || (pid = atoi(argv[0])) < 0) {
        puts("\nIngreso invalido. Debe ingresar el ID del proceso que desea eliminar como primer argumento.");
    } else {
        if (pid <= 1) {
            puts("\nDelete unsuccesfull. Shell and Idle can't be killed.");
            return;
        }
        /* Returns the killed process's PID if successfull or 0 if not */
        int ret = kill(pid);
        printf("\nDelete %s", (ret == 0) ? "unsuccesfull":"successfull");
    }
}

/* Block Built-in Command - Changes state between blocked and ready of the process with PID entered via arguments */
void block_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    int pid;
    if (argc < 1 || (pid = atoi(argv[0])) < 0) {
        puts("\nIngreso invalido. Debe ingresar el ID del proceso que desea cambiar de estado como primer argumento.");
    } else {
        /* Returns 0 if successfull or 1 if not */
        int ret = changeState(pid);
        printf("\nChange %s", (ret != 0) ? "unsuccesfull":"successfull");
    }
}

/* Nice Built-in Command - Changes priority of the process with PID entered via arguments */
void nice_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    int pid, priority;
    if (argc < 2 || (pid = atoi(argv[0])) < 0 || (priority = atoi(argv[1])) < 0) {
        puts("\nIngreso invalido. Debe ingresar el ID del proceso que desea cambiar de estado como primer argumento y la nueva prioridad del \nproceso como segundo argumento.");
    } else {
        if (pid <= 1) {
            puts("\nDelete unsuccesfull. Shell and Idle can't be modified.");
            return;
        }
        /* Returns 0 if successfull, 1 if invalid PID or 2 if invalid priority number */
        int ret = setPriority(pid, priority); // Devuelve 0 si sale bien, 1 si no encuentra pid, 2 si error en prioridad
        switch (ret)
        {
            case 1:
                puts("\nPID Invalido.");
                break;
            case 2:
                puts("\nPrioridad Invalida.");
                break;
            default: // case 0
                puts("\nChange successfull");
                break;
        }
    }
}

/* Loop Command - Creates a process that prints a message and sleeps in an infinite loop */
void loop_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {     
    int pid = newProcess("LOOP", argc, argv, ground, inFd, outFd);
    if (ground == BACKGROUND)
        printf("\nCreate %s. PID = %d", (pid == 0) ? "unsuccesfull":"successfull", pid);
}

/* Cat Command - Creates a process that prints stdin as recieved */
void cat_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    if (ground == BACKGROUND) {
        puts("\nNo puedes correr este comando en background! Requiere ingreso por STDIN.");
    } else {
        newProcess("CAT", argc, argv, ground, inFd, outFd);
    }
}

/* WC Command - Creates a process that counts lines from STDIN */
void wc_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    if (ground == BACKGROUND) {
        puts("\nNo puedes correr este comando en background! Requiere ingreso por STDIN.");
    } else {
        newProcess("WC", argc, argv, ground, inFd, outFd);
    }
}

/* Filter Command - Creates a process that prints stdin filtering vowels */
void filter_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    if (ground == BACKGROUND) {
        puts("\nNo puedes correr este comando en background! Requiere ingreso por STDIN.");
    } else {
        newProcess("FILTER", argc, argv, ground, inFd, outFd);
    }
}

/* Sem Built-in Command - Prints current semaphore list & status */
void sem_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    semStatus(outFd);
}

/* Pipe Built-in Command - Prints current pipes list & status */
void pipe_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    pipeStatus(outFd);
}

/* Phylo Command - Creates a process that implements the phylosophers problem */
void phylo_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    if (ground == BACKGROUND) {
        puts("\nNo puedes correr este comando en background! Requiere ingreso por STDIN.");
    } else {
        newProcess("PHYLO", argc, argv, ground, inFd, outFd);
    }
}

void test_cmd(int argc, char * argv[], int ground, int inFd, int outFd) {
    if (ground == BACKGROUND) {
        puts("\nNo puedes correr este comando en background! Requiere ingreso por STDIN.");
    } else {
        newProcess("PROCESS_A", argc, argv, ground, inFd, outFd);
    }
}