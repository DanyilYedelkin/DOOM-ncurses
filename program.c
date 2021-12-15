/* 
    for compile and run you need to use this:   gcc program.c $(ncursesw5-config --cflags) -o program -lncursesw -lm
                                        ./program moves.txt
*/


#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include <math.h>

void MakeScreen();
void painting();


// create our main function 
int main(int argc, char* argv[]){
    //open the file, which we write into console parametr
    if(argc != 2){
        printf("Wrong number of parameters.\n");
        return 1;
    }
    FILE *fp = fopen(argv[1], "w");
    
    if(fp == NULL){
        printf("Error opening file.\n");
        fclose(fp);
        return 1;
    }


    // creating a variable, which controles our moves
    int moves = 0;
    // creating a console screen size X (columns) 
    int ShirinaKarty;
    // creating console screen size Y (rows)   
    int VisotaKarty;   
    // creating World dimensions            
    int mapWidth = 16;       
    int mapHeight = 16;

    // creating a player start position 
    float PervoyePolozenie = 8.0f;   
    float VtoroyePolozenie = 8.0f;
    // ceating a player start rotation 
    float TreteyePolozenie = 2.0f;      
    // creating a field of view    
    float zrenie = 3.14159f / 2.0f; 
    // creating a maximum rendering distance 
    float distancia = 16.0f; 

    char stena = '#';

    int vichod = 1;

    // creating a world map, where '#' - wall, '.' - space 
    char karta[] =                   
    "################\
     #..............#\
     #..............#\
     #...########...#\
     #.......#..#...#\
     #..........#...#\
     #..............#\
     #...########...#\
     #...#...#..#...#\
     #...#...#..#...#\
     #..............#\
     #..............#\
     #...########...#\
     #..............#\
     #..............#\
     ################";


    // Needed to output unicode characters 
    setlocale(LC_ALL, "");
    
    // Initialize ncurses screen
    MakeScreen();
    getmaxyx(stdscr, VisotaKarty, ShirinaKarty);   

    // creating a game function
    while(vichod == 1){    
        for(int colona = 0; colona < ShirinaKarty; colona++){
            // for each column, calculate the projected ray angle into world space
            float dalnostZrenia = (TreteyePolozenie - zrenie / 2.0f) + ((float)colona / (float)ShirinaKarty) * zrenie;            
            float rastoyanie = 0.0f;
            
            int StenkaVLob = 0;
            
            // creating unit vector for ray in player space 
            float ZrenieX = sinf(dalnostZrenia); 
            float ZrenieY = cosf(dalnostZrenia);
            
            // find distance to wall
            while(!StenkaVLob && rastoyanie < distancia){
                // crate resolution 
                rastoyanie = rastoyanie + 0.5f;
                
                int proverkaColony = (int)(PervoyePolozenie + ZrenieX * rastoyanie);

                int proverkaRow = (int)(VtoroyePolozenie + ZrenieY * rastoyanie);
                
                // test our code if ray is out of bounds 
                if(proverkaColony < 0 || (proverkaColony > mapWidth + 1) || proverkaRow < 0 || (proverkaRow > mapHeight + 1)){
                    StenkaVLob = 1;
                    rastoyanie = distancia;
                } else{
                    if(karta[proverkaColony * mapWidth + proverkaRow] == stena){
                        StenkaVLob = 1;
                    }
                }
            }    
            
            // calculate distance to ceiling and floor 
            int ceiling = (float)(VisotaKarty / 2.0f) - VisotaKarty / ((float)rastoyanie);
            int floor = VisotaKarty - ceiling;
            
            // shader walls based on distance 
            const wchar_t* shade;

            // if we close to the wall 
            if(rastoyanie <= distancia / 4.0){
                shade = L"\x2588";
            } 
            // if we further to the wall 
            else if(rastoyanie <= distancia / 3.0){
                shade = L"\x2593";

            } else if(rastoyanie <= distancia / 2.0){
                shade = L"\x2592";

            } else if(rastoyanie <= distancia){
                shade = L"\x2591";

            } else{
                shade = L" ";

            }
            
            for(int row = 0; row < VisotaKarty; row++){

                if(ceiling  + 1 > row){
                    mvaddch(row, colona, '`');

                } else if(row > ceiling  && row < floor + 1){
                    mvaddwstr(row, colona, shade);

                } else{
                    // Shade floor based on distance
                    float b = 1.0f - (((float)row - VisotaKarty / 2.0f) / ((float)VisotaKarty / 2.0f));
                    
					if (b < 0.25){
                        shade = L"#";

                    } else if (b < 0.5){
                        shade = L"x";

                    } else if (b < 0.75){
                        shade = L".";

                    } else if (b < 0.9){
                        shade = L"-";

                    } else{
                        shade = L" ";

                    }
                    
                    mvaddwstr(row, colona, shade);
                }
            }
            
            refresh();
        }
        
        int input;
        
        switch((input = getch())){
            //work with time
            timeout(300);

            // Move forward and collisions 
            case KEY_UP:
            case 'W':
            case 'w': 
                PervoyePolozenie = PervoyePolozenie + sinf(TreteyePolozenie) * 0.5f;

                VtoroyePolozenie = VtoroyePolozenie + cosf(TreteyePolozenie) * 0.5f;
                
                if(karta[(int)PervoyePolozenie * mapWidth + (int)VtoroyePolozenie] == stena){
                    PervoyePolozenie = PervoyePolozenie - sinf(TreteyePolozenie) * 0.5f;
                    VtoroyePolozenie = VtoroyePolozenie - cosf(TreteyePolozenie) * 0.5f;
                } 
                moves++;
                
                break;
            
            case KEY_DOWN:
            case 'S':
            case 's':
                // Move backward and collisions 
                PervoyePolozenie = PervoyePolozenie - sinf(TreteyePolozenie) * 0.5f;
                VtoroyePolozenie = VtoroyePolozenie - cosf(TreteyePolozenie) * 0.5f;
                
                if(karta[(int)PervoyePolozenie * mapWidth + (int)VtoroyePolozenie] == stena){
                    PervoyePolozenie = PervoyePolozenie + sinf(TreteyePolozenie) * 0.5f;
                    VtoroyePolozenie = VtoroyePolozenie + cosf(TreteyePolozenie) * 0.5f;
                }
                moves++;
                
                break;
            
            case KEY_LEFT:
            case 'A':
            case 'a': 
                // Rotate left 
                TreteyePolozenie = TreteyePolozenie - 0.1f;
                moves++;

                break;

            case KEY_RIGHT:
            case 'D':
            case 'd':
            // Rotate right 
                TreteyePolozenie = TreteyePolozenie + 0.1f;
                moves++;

                break;
        
            default:{
                // Any key to exit
                vichod = 0;

                break;
            }
        }    
    }
    
    //make the color for game scene
    attroff(COLOR_PAIR(1));

    endwin();     

    //write into the file number of our moves
    fprintf(fp, "Number of moves: %d", moves);
    fclose(fp);

    return 0;   
}

void MakeScreen(){
    // Needed to output unicode characters 
    setlocale(LC_ALL, "");
    
    // Initialize ncurses screen 
    initscr();
    noecho();
    curs_set(0);
    painting();
    keypad(stdscr, TRUE);
}
void painting(){
    start_color();
    init_color(COLOR_GREEN, 0, 700, 200);
	init_pair(1, COLOR_CYAN, COLOR_BLACK);

	attron(COLOR_PAIR(1));
}
