extern crate sdl2;

use sdl2::event::Event as SDLEvent;

fn main() {
    println!("Hello, world!");

    sdl2::hint::set("SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS", "1");

    let sdl = sdl2::init().unwrap();
    let joy_sys = sdl.joystick().unwrap();

    if joy_sys.open(0).is_err() {
        panic!("Error: no controller found!");
    }

    let mut event_pump = sdl.event_pump().unwrap();
    'main: loop {
        for event in event_pump.wait_iter() {
            match event {
                SDLEvent::Quit {..} => { break 'main; }
                SDLEvent::JoyAxisMotion {..} => { println!("JoyAxisMotion"); }
                SDLEvent::JoyHatMotion {..} => { println!("JoyHatMotion"); }
                SDLEvent::JoyButtonDown {..} => { println!("JoyButtonDown"); }
                SDLEvent::JoyButtonUp {..} => { println!("JoyButtonUp"); }
                SDLEvent::JoyDeviceAdded {..} => { println!("JoyDeviceAdded"); }
                SDLEvent::JoyDeviceRemoved {..} => { println!("JoyDeviceRemoved"); }
                _ => {}
            }
        }
    }
}

