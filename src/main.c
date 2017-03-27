#include <SDL2/SDL.h>
#include <assert.h>

// Software is being developed on a sytsem with terrible drivers that sometimes make neutral = 128, 129 deadzone should fix this.
#define DEADZONE 129

#define KEY_UP "k"
#define KEY_DOWN "j"
#define KEY_LEFT "h"
#define KEY_RIGHT "l"

static const
char sdlToHack(int sdl_hat) {
    ;switch(sdl_hat) {
    case SDL_HAT_CENTERED: return '.';
    case SDL_HAT_UP: return 'k';
    case SDL_HAT_RIGHT: return 'l';
    case SDL_HAT_DOWN: return 'j';
    case SDL_HAT_LEFT: return 'h';
    case SDL_HAT_RIGHTUP: return 'u';
    case SDL_HAT_RIGHTDOWN: return 'n';
    case SDL_HAT_LEFTUP: return 'y';
    case SDL_HAT_LEFTDOWN: return 'b';
    default: return '.';
    }
};

#define UPDATE_FREQUENCY 1000


typedef union {
    char *str;
} Arg;

typedef struct Menu_s {
    char title[9];
    struct Menu_s (*func)(struct Menu_s *context, Arg *arg);
    Arg arg;
    struct Menu_s *up, *down, *left, *right, *a, *b, *x, *y, *l, *r, *select, *start;
} Menu;

Menu printMenu(Menu *m, Arg *arg) {

#define GETTITLE(BUTTON) ((m->BUTTON->title != NULL) ? (m->BUTTON->title) : "")

    fprintf(stderr, "\
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
           GETTITLE(l), m->title, GETTITLE(r),
           GETTITLE(up), GETTITLE(x),
           GETTITLE(left), GETTITLE(right), GETTITLE(y), GETTITLE(a),
           GETTITLE(down), GETTITLE(select), GETTITLE(start), GETTITLE(b) );

#undef GETTITLE

    return *m;
}

Menu runMenu(Menu *m) {
    return m->func(m, &m->arg);
}

int main(int argc, char **argv)
{
    SDL_SetHint("SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "1");

    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

    Menu m = {
        "MoveMode",
        printMenu, NULL,
        .up = &(struct Menu_s){"MoveUp"},
        .down = &(struct Menu_s){"MoveDown"},
        .left = &(struct Menu_s){"MoveLeft"},
        .right = &(struct Menu_s){"MoveRght"},
        .a = &(struct Menu_s){"Step"},
    };
    runMenu(&m);

    SDL_GameControllerAddMappingsFromFile("dist/SDL_GameControllerDB/gamecontrollerdb.txt");

    assert(1 == SDL_NumJoysticks());

    SDL_Joystick *joy = SDL_JoystickOpen(0);

    int hat = SDL_HAT_CENTERED;

    SDL_Event e;
    while (1) {
        while(SDL_WaitEventTimeout(&e, UPDATE_FREQUENCY)) {
//            fprintf(stderr, "Event with type %x\n", e.type);
            switch(e.type) {
            case SDL_QUIT: goto QUIT;
            case SDL_JOYAXISMOTION:
//                printf("Axis: %d, Value: %d\n", e.jaxis.axis, e.jaxis.value);
                switch(e.jaxis.axis) {
                case 0: // Horizontal
                    if (e.jaxis.value > DEADZONE) { // Right
                        hat &= SDL_HAT_UP | SDL_HAT_DOWN;
                        hat |= SDL_HAT_RIGHT;
                    } else if (e.jaxis.value < -DEADZONE) { // Left
                        hat &= SDL_HAT_UP | SDL_HAT_DOWN;
                        hat |= SDL_HAT_LEFT;
                    } else {
                        hat &= SDL_HAT_UP | SDL_HAT_DOWN;
                    }
                    break;
                case 1: // Vertical
                    if (e.jaxis.value > DEADZONE) { // Down
                        hat &= SDL_HAT_LEFT | SDL_HAT_RIGHT;
                        hat |= SDL_HAT_DOWN;
                    } else if (e.jaxis.value < -DEADZONE) { // Up
                        hat &= SDL_HAT_LEFT | SDL_HAT_RIGHT;
                        hat |= SDL_HAT_UP;
                    } else {
                        hat &= SDL_HAT_LEFT | SDL_HAT_RIGHT;
                    }
                    break;
                default: break;
                }
                break;
            case SDL_JOYBUTTONDOWN:
                switch(e.jbutton.button) {
                case 0:  printf("send-keys %c\n", sdlToHack(hat)); break;
                case 11: printf("send-keys %s\n", KEY_UP); break;
                case 12: printf("send-keys %s\n", KEY_DOWN); break;
                case 13: printf("send-keys %s\n", KEY_LEFT); break;
                case 14: printf("send-keys %s\n", KEY_RIGHT); break;
                default: fprintf(stderr, "other button %d pressed.\n", e.jbutton.button);
                }
                fflush(stdout);
                break;
            default: break;
            }
        }
    }
QUIT:

    SDL_Quit();
    return 0;
}
