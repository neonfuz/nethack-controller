# Controller interface for nethack

This is a project to add controller bidings to nethack, via some simple menus.

## Development

Currently this project is very early in development. Currently I am still
experimenting, and am not really sure what direction the project is going to go.
The general idea is to make a modal interface where a button either executes a
key macro, or opens a menu to access more macros. Menus are simply a display of
what each key does in the current mode.

Also considering switching to rust, a heavy WIP version of this is in the
`rustport` folder.

### Resources

Here are a couple resources to aid in further development.

1. https://nethackwiki.com/wiki/Commands

   Lists of all regular commands for nethack

2. https://nethackwiki.com/wiki/Menu_controls

   List of menu specific controls for nethack

## Dependencies

These are the system dependencies for running this project:

* nethack
* tmux 

## Building

1. Update submodules to download dependencies

        git submodule update --init

2. Build binaries

        make

## Running

It works! Start a game of nethack with `./start.sh`, make sure your controller
is plugged in and run `./main`. Currently the software is developed for the
iBuffalo snes controller, on MacOS and Linux. You may need to modify the source
to get your controller to work, or to add more controls and features.
