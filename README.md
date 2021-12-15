# DOOM-ncurses
game DOOM in C with library ncurses

for compile and run you need to use this:   gcc program.c $(ncursesw5-config --cflags) -o program -lncursesw -lm
                                       && ./program moves.txt
