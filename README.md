# Controller interface for nethack

This is a project to add controller bidings to nethack, via some simple menus.

## Development

Currently this project is very early in development. Currently I am still
experimenting, and am not really sure what direction the project is going to go.
The general idea is to make a modal interface where a button either executes a
key macro, or opens a menu to access more macros. Menus are simply a display of
what each key does in the current mode.

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

It works! Start a game of nethack with `sh start.sh`, and after your
controller is plugged in, run `./main | sh pipe.sh`. Might have to modify
source code to add support to your controller, or various features.
