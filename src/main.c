#include <SDL2/SDL.h>
#include <assert.h>

// Software is being developed on a sytsem with terrible drivers that sometimes make neutral = 128, 129 deadzone should fix this.
#define DEADZONE 129

#define KEY_UP "k"
#define KEY_DOWN "j"
#define KEY_LEFT "h"
#define KEY_RIGHT "l"

static const
char sdlToHack(int sdl_hat, SDL_bool shift) {
    switch(sdl_hat) {
    case SDL_HAT_CENTERED:  return shift ? '.' : 's';
    case SDL_HAT_UP:        return shift ? 'K' : 'k';
    case SDL_HAT_RIGHT:     return shift ? 'L' : 'l';
    case SDL_HAT_DOWN:      return shift ? 'J' : 'j';
    case SDL_HAT_LEFT:      return shift ? 'H' : 'h';
    case SDL_HAT_RIGHTUP:   return shift ? 'U' : 'u';
    case SDL_HAT_RIGHTDOWN: return shift ? 'N' : 'n';
    case SDL_HAT_LEFTUP:    return shift ? 'Y' : 'y';
    case SDL_HAT_LEFTDOWN:  return shift ? 'B' : 'b';
    default:                return shift ? '.' : 's'; // Dead code?
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

typedef enum { E_NULL, E_HAT, E_BUTTON } EventType;

typedef struct {
    EventType type;
} GenericEvent;

typedef struct {
    EventType type;
    int newHat;
} HatEvent;

typedef struct {
    EventType type;
    char key;
    SDL_bool state;
} ButtonEvent;

typedef union {
    EventType type;
    GenericEvent generic;
    HatEvent hat;
    ButtonEvent button;
} Event;

Event generateEvent(SDL_Event e) {
    static int hat = SDL_HAT_CENTERED;
    int newHat = hat;

    switch (e.type) {
    case SDL_JOYAXISMOTION:
        /* Handle joystick motion events by updating newHat, return hat event
         * only if hat != newHat */
        switch(e.jaxis.axis) {
        case 0: // Horizontal
            if (e.jaxis.value > DEADZONE) // Right
                newHat = (hat & (SDL_HAT_UP | SDL_HAT_DOWN)) | SDL_HAT_RIGHT;
            else if (e.jaxis.value < -DEADZONE) // Left
                newHat = (hat & (SDL_HAT_UP | SDL_HAT_DOWN)) | SDL_HAT_LEFT;
            else
                newHat = hat & (SDL_HAT_UP | SDL_HAT_DOWN);
            break;
        case 1: // Vertical
            if (e.jaxis.value > DEADZONE) // Down
                newHat = (hat & (SDL_HAT_LEFT | SDL_HAT_RIGHT)) | SDL_HAT_DOWN;
            else if (e.jaxis.value < -DEADZONE) // Up
                newHat = (hat & (SDL_HAT_LEFT | SDL_HAT_RIGHT)) | SDL_HAT_UP;
            else
                newHat = hat & (SDL_HAT_LEFT | SDL_HAT_RIGHT);
            break;
        }
        if(newHat == hat) return (Event){ .type = E_NULL };
        else return (Event){ .hat = { E_HAT, newHat } };
        break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        /* Simple button event converter */
        return (Event){ .button = {
                E_BUTTON, e.jbutton.button, e.jbutton.state }};
    default:
        return (Event){ .type = E_NULL};
    }
}

Menu applyEvent(Event e, Menu m) {
    return m;
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
    SDL_bool shift = 0;

    SDL_Event e;
    while (1) {
        while(SDL_WaitEventTimeout(&e, UPDATE_FREQUENCY)) {
//            fprintf(stderr, "Event with type %x\n", e.type);
            switch(e.type) {
            case SDL_QUIT: goto QUIT;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYAXISMOTION:
generateEvent(e);
                break;
            default: break;
            }
        }
    }
QUIT:

    SDL_Quit();
    return 0;
}
