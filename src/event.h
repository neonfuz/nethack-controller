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
