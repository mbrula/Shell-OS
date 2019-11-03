#ifndef SHELL_H_
#define SHELL_H_

#define NO_CMD          -1
#define HELP_CMD        0
#define DATE_CMD        1
#define TIME_CMD        2
#define SLEEP_CMD       3
#define CLEAR_CMD       4
#define BEEP_CMD        5
#define DOOR_CMD        6
#define DIV_ZERO_CMD    7
#define INV_OP_CMD      8
#define EXIT_CMD        9

#define PROMPT_STRING   "mjj@itba.edu.ar $ "
#define BEEP_FREQ       400
#define DOOR_FREQ       600

#define SEM_PIPE_NAME "/pipeCmd_"

void initShell();
void initScreen();
int getCommand(char * input);
void executeCommand(int command, int param1, int param2);

// Si una funcion que no deberia recibir parametros los recibe, los ignora
// Si una funcion que debia recibir parametros no los recibe, no se ejecuta

void help_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void date_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void time_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void sleep_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void clear_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void beep_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void door_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void div_zero_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void inv_op_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void exit_cmd(int argc, char * argv[], int ground, int inFd, int outFd);

void mem_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void ps_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void kill_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void block_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void nice_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void loop_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void cat_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void wc_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void filter_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void sem_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void pipe_cmd(int argc, char * argv[], int ground, int inFd, int outFd);
void phylo_cmd(int argc, char * argv[], int ground, int inFd, int outFd);

void sync_cmd(int argc, char * argv[], int ground, int inFd, int outFd);

#endif /* SHELL_H_ */