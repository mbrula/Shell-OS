// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <fileDescriptors.h>
#include <process.h>

#include <keyboard.h>

static unsigned char map[90]={'`','\e','1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
        'C','a','s','d','f','g','h','j','k','l',';','\'', 0, 'S','\\','z','x','c','v','b','n','m',',','.','/','S',0,0/*ALT*/,' ',
        'M',0,0,0,0,0,0,0,0,0,0,0,0,0,UP_KEY,0,0,LEFT_KEY,0,RIGHT_KEY,0,0,DOWN_KEY };

static unsigned char shift_map[90]={'~','\e','!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','q','w','e','r','t','y','u','i','o','p','{','}','\n',
        'C','a','s','d','f','g','h','j','k','l',':','\"', 0, 'S','|','z','x','c','v','b','n','m','<','>','?','S',0,0/*ALT*/,' ',
        'M',0,0,0,0,0,0,0,0,0,0,0,0,0,UP_KEY,0,0,LEFT_KEY,0,RIGHT_KEY,0,0,DOWN_KEY };

/* Returns the recieved character in UpperCase */
static char toUpper(char car);

// ESC = \e
// BACK = \b
// SHIFT = S
// CAPS = M
// CTRL = C
// ALT = A
// UP = U
// LEFT = L
// RIGHT = R
// DOWN = D

/* Handle keyboard in */
void keyboard_handler() {
    static int caps = 1, shift = 1, release_caps = 1, ctrl = -1;
    unsigned char key = read_port(0x60);

    if ((key >= 90 && key < 0x80) || key >= 0x80 + 90)
        return;

    char car;
    if (key < 0x80){ // Pressed key
        if (shift == 1)
            car = map[key];
        else
            car = shift_map[key];
        switch (car){
            case 'S':   // SHIFT
                shift = -1;
                break;
            case 'M':   // CAPS
                if (release_caps){
                    caps *= -1;
                    release_caps = 0;
                }
                break;
            case 'C':   // CTRL
                ctrl = 1;
                break;
            default:
                if (ctrl == 1) {
                    if (car == 'c') {           // CTRL + C 
                        car = SIGINT;
                        sig_int();
                    } else if (car == 'd') {    // CTRL + D
                        car = EOF;
                        write(STDIN, &car, 1);
                    }
                } else {
                    /* Check if MAYUS is On */
                    if (shift * caps == -1){
                        car = toUpper(car);
                    }
                    /* If valid character, write in fileDescriptor buffer */
                    if (car != 0)
                        write(STDIN, &car, 1);
                }
                break;
        }
    } else { // Released key
        car = map[key - 0x80];
        switch (car){
            case 'S':   // SHIFT
                shift = 1;
                break;
            case 'M':   // CAPS
                release_caps = 1;
                break;
            case 'C':   // CTRL
                ctrl = -1;
                break;
        }
    }
}

/* Returns the recieved character in UpperCase */
static char toUpper(char car) {
    if (car >= 'a' && car <= 'z')
        car -= ('a' - 'A');
    return car;
}