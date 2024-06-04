# Simple SDL Game

This is a simple game developed to learn how to use the SDL library alongside the C programming language.

# Components

* **C file:** Contains all the game source code.
* **xml folder:** Contains the levels.xml file, which is responsible for setting the positions of items in each level.
* **assets folder:** Contains the images used in the game.
* **fonts folder:** Contains the font file used for text display.

The game currently contains 3 levels, but it is possible to add more via xml/levels.xml.

# Environment Setup
I used MSYS2 as a working environment and installed the necessary libraries using the pacman command:

`pacman -S mingw-w64-x86_64-SDL2_image`

`pacman -S mingw-w64-x86_64-SDL2_ttf`

`pacman -S mingw-w64-x86_64-SDL2_timer`

`pacman -S mingw-w64-x86_64-libxml2`

Any missing library will trigger an error while building the game. So you just need to install it with the pacmann command like the previous examples.
 
# Building the Game
To generate the .exe file, run the following command:

`make clean`
`make`

The game doesn't always run smoothly and probably contains many problems that could be detected during gameplay.

# Credits

I learned the basics of using SDL2 by watching the youtube playlist 'Learn Video Game Programming in C' by VertoStudio3D channel.

Link: https://youtube.com/@vertostudio3d?si=zmjcBuyEl1j5mU-u
