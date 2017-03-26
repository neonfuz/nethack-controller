#include <SDL2/SDL.h>
#include <assert.h>

#define KEY_UP "k"
#define KEY_DOWN "j"
#define KEY_LEFT "h"
#define KEY_RIGHT "l"

#define UPDATE_FREQUENCY 1000

#define DEBUG 0

typedef struct Menu_s {
    char title[9];
    struct Menu_s *up, *down, *left, *right, *a, *b, *x, *y, *l, *r, *select, *start;
} Menu;

void printMenu(Menu m) {

#define GETTITLE(BUTTON) ((m.BUTTON->title != NULL) ? (m.BUTTON->title) : "")

    printf("\
 ______________________________________________________________\n\
/  %8s  /  L           %8s           R  \\  %8s  \\\n\
|-----------/                                      \\-----------|\n\
|       %8s                                %8s       |\n\
|          ^                                       Y           |\n\
|  %8s  %8s                      %8s  %8s  |\n\
|     <         >                             X         A      |\n\
|       %8s      %8s  %8s        %8s       |\n\
|          v             sel     start             B           |\n\
\\                                                              /\n\
 \\------------------------------------------------------------/\n",
           GETTITLE(l), m.title, GETTITLE(r),
           GETTITLE(up), GETTITLE(x),
           GETTITLE(left), GETTITLE(right), GETTITLE(y), GETTITLE(a),
           GETTITLE(down), GETTITLE(select), GETTITLE(start), GETTITLE(b) );

#undef GETTITLE
}

int main(int argc, char **argv)
{
    SDL_SetHint("SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "1");
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

    Menu m = {
        "MoveMode",
        .up = &(struct Menu_s){"MoveUp"},
        .down = &(struct Menu_s){"MoveDown"},
        .left = &(struct Menu_s){"MoveLeft"},
        .right = &(struct Menu_s){"MoveRight"},
        .a = &(struct Menu_s){"Step"},
    };
    printMenu(m);

    SDL_GameControllerAddMappingsFromFile("dist/SDL_GameControllerDB/gamecontrollerdb.txt");

    do {
        int joysticks = SDL_NumJoysticks();
        if(DEBUG) fprintf(stderr, "Detected %d controllers.\n", joysticks);
        assert(1 == joysticks);
    } while(0);

    SDL_Joystick *joy = SDL_JoystickOpen(0);

    if(DEBUG) {
        if (joy) {
            fprintf(stderr, "Opened Joystick 0\n");
            fprintf(stderr, "Name: %s\n", SDL_JoystickNameForIndex(0));
            fprintf(stderr, "Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
            fprintf(stderr, "Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
            fprintf(stderr, "Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
        } else {
            fprintf(stderr, "Couldn't open Joystick 0\n");
        }
    }

    SDL_Event e;
    while (1) {
        while(SDL_WaitEventTimeout(&e, UPDATE_FREQUENCY)) {
//            fprintf(stderr, "Event with type %x\n", e.type);
            switch(e.type) {
            case SDL_QUIT: goto QUIT;
            case SDL_JOYBUTTONDOWN:
                switch(e.jbutton.button) {
                case 11: printf("send-keys %s\n", KEY_UP); break;
                case 12: printf("send-keys %s\n", KEY_DOWN); break;
                case 13: printf("send-keys %s\n", KEY_LEFT); break;
                case 14: printf("send-keys %s\n", KEY_RIGHT); break;
                }
                break;
            default: break;
            }
        }
    }
QUIT:

    SDL_Quit();
    return 0;
}
