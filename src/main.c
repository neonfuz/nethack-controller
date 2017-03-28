#include <SDL2/SDL.h>
#include <assert.h>

#include "event.h"

// Software is being developed on a sytsem with terrible drivers that sometimes make neutral = 128, 129 deadzone should fix this.
#define DEADZONE 129

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

typedef struct Menu_s {
    char title[9];
    struct {
        int hat;
        unsigned shift :1;
        unsigned ctrl :1;
    } state;
    struct Menu_s *up, *down, *left, *right, *a, *b, *x, *y, *l, *r, *select, *start;
} Menu;

Menu printMenu(Menu *m) {

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

Event generateEvent(SDL_Event e) {
    static int hat = SDL_HAT_CENTERED;
    int newHat = hat;

    static const char buttonMap[] = {
        [0] = 'a', [1] = 'b', [2] = 'x', [3] = 'y',
        [4] = 'L', [5] = 'R',
        [6] = 'o', [7] = 's',
    };

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
        if(newHat == hat) {
            return (Event){ .type = E_NULL };
        } else {
            hat = newHat;
            return (Event){ .hat = { E_HAT, newHat } };
        }
        break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        /* Simple button event converter */
        return (Event){ .button = {
                E_BUTTON, buttonMap[e.jbutton.button], e.jbutton.state }};
    default:
        return (Event){ .type = E_NULL};
    }
}

Menu applyEvent(Event e, Menu m)
{
    switch(e.type) {
    case E_HAT:
        m.state.hat = e.hat.newHat;
        return m;
    case E_BUTTON:
        switch(e.button.key) {
        case 'L': m.state.ctrl = e.button.state; break;
        case 'R': m.state.shift = e.button.state; break;
        case 'a':
            printf("send-keys %s%c\n",
                   m.state.ctrl ? "C-" : "",
                   sdlToHack(m.state.hat, m.state.shift));
            break;
        case 'b': printf("send-keys ,\n"); break;
        case 's':  printf("send-keys Enter\n"); break;
        case 'o':  printf("send-keys \\;\n"); break;
        }
    case E_NULL: return m;
    }
}

int main(int argc, char **argv)
{
    SDL_SetHint("SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "1");

    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);

    Menu m = {
        "movemode",
        { 0 },
        .up =    &(Menu){"moveup"},
        .down =  &(Menu){"movedown"},
        .left =  &(Menu){"moveleft"},
        .right = &(Menu){"moverght"},
        .a =     &(Menu){"step"},
    };

    SDL_GameControllerAddMappingsFromFile("dist/SDL_GameControllerDB/gamecontrollerdb.txt");

    assert(1 == SDL_NumJoysticks());

    SDL_Joystick *joy = SDL_JoystickOpen(0);

    int hat = SDL_HAT_CENTERED;
    SDL_bool shift = 0;

    SDL_Event e;
    while(SDL_WaitEvent(&e)) {
        switch(e.type) {
        case SDL_QUIT: goto QUIT;
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYAXISMOTION:
            m = applyEvent(generateEvent(e), m);
            break;
        default: break;
        }
    }

    fprintf(stderr, "Error: Broke out of event loop early: \"%s\"\n",
            SDL_GetError());

QUIT:

    SDL_Quit();
    return 0;
}
