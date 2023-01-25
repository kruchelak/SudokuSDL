#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h> //i know i shouldn't
#include <SDL2_gfxPrimitives.h>
#include <errno.h>

#define MAX_SURFACES_X 9
#define MAX_SURFACES_Y 9
#define WINDOW_WIDTH 450
#define WINDOW_HEIGHT 450
//https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
#define ENOUGH snprintf(NULL, 0,"%d",42)
int startNumbers[9][9]; //start board
int correctNumbers[9][9]; //complete board

typedef struct rect{
    bool isClicked;
    SDL_Rect rect;

}rect;
rect rects[9][9];


void get_numbers(int level){
    //creates path to file, bufor equals 27 : string length + \0 sign
    printf("%d\n", level);
    char pathstart[27];
    char pathend[25];
    snprintf(pathstart, 27, "./assets/levels/start%d.txt", level);
    snprintf(pathend, 25, "./assets/levels/end%d.txt", level);

    FILE *start = fopen(pathstart, "r");
    FILE *correct = fopen(pathend,"r");
    if (start == NULL || correct == NULL) {
        //error while opening the file
        fprintf(stderr, "Can't open the file %s\n", strerror(errno));
        return;
    }
    //puts numbers from files to arrays
    for(int i = 0; i<9; i++){
        for(int j = 0; j<9; j++){
            fscanf(start, "%d", &startNumbers[j][i]);
            fscanf(correct, "%d", &correctNumbers[j][i]);


        }
    }
    fclose(start);
    fclose(correct);


}
int check_win(void){
    //compares current state of board with correct board
    int a = memcmp(correctNumbers, startNumbers, sizeof(correctNumbers));
    if(!a)
        return 0; //arrays are equal
    else
        return 1; //error, arrays are not equal
}


void create_rects(SDL_Renderer* renderer){
    //sets font
    TTF_Font *font = TTF_OpenFont("./assets/OpenSans/OpenSans-Regular.ttf", 128);
    if (!font) {
        fprintf(stderr, "Can't open font file: %s\n", TTF_GetError());
        exit(1);
    }
    //sets text color
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* textSurfaces[MAX_SURFACES_X][MAX_SURFACES_Y];

    for(int i = 0; i<9 ; i++){
        for(int j = 0; j<9; j++) {
            rects[i][j].rect = (SDL_Rect) {i * (WINDOW_WIDTH / 9), j * (WINDOW_HEIGHT / 9), 50, 50};
            //sets green background if rect is clicked
            if (rects[i][j].isClicked == true) {
            SDL_SetRenderDrawColor(renderer, 0, 150, 50, 10);
            SDL_RenderFillRect(renderer, &rects[i][j].rect);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &rects[i][j].rect);
            }

            rects[i][j].isClicked = false;

            //number to string, needed to put it onto the surface
            char number_str[ENOUGH+1];
            sprintf(number_str, "%d", startNumbers[i][j]);


            //creates a surface for text
            textSurfaces[i][j] = TTF_RenderText_Solid(font, number_str, textColor);
            if (!textSurfaces[i][j]) {
                fprintf(stderr, "Can't create text surface: %s\n", TTF_GetError());
                exit(1);
            }

            //creates texture from surface, no texture if number = 0
            SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, textSurfaces[i][j]);
            if (startNumbers[i][j] != 0) {
                SDL_RenderCopy(renderer, text_texture, NULL, &rects[i][j].rect);
            }

        }


    }
    if(!check_win()){
        printf("YOU WON!");
    }

    TTF_CloseFont(font);
    for (int i = 0; i < MAX_SURFACES_X; i++) {
        for (int j = 0; j < MAX_SURFACES_Y; j++) {
            SDL_FreeSurface(textSurfaces[i][j]);
        }
    }
}
//creates 2 vertical, 2 horizontal thick lines between 3x3 squares
void create_lines(SDL_Renderer* renderer){
    for(int i =1 ;i<3 ;i++) {
        thickLineRGBA(renderer, (WINDOW_WIDTH/3)*i, 0, (WINDOW_WIDTH/3)*i, WINDOW_HEIGHT, 5, 0, 0, 0, 255);
        thickLineRGBA(renderer, 0, (WINDOW_HEIGHT/3)*i, WINDOW_WIDTH, (WINDOW_HEIGHT/3)*i, 5, 0, 0, 0, 255);

    }
}
//shows instructions, waits for user input
int pick_level(void){

    int level=0;

    printf("Enter level difficulty\nEasy - 1\nMedium - 2\nHard - 3\n");
    while(level <1 || level >3){

        scanf("%d", &level);
        if(level <1 || level >3)
         printf("Wrong input. Pick a number between 1 and 3\n");

    }


    return level;
}
//creates new screen everytime this function is called
void update_screen(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderClear(renderer);
    create_rects(renderer);
    create_lines(renderer);

    SDL_RenderPresent(renderer);
}
//checks in which rect click occurred
void check_mouse_in_rect(SDL_Renderer* renderer) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX,&mouseY);
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            SDL_Point mousePoint = {mouseX, mouseY};
            if (SDL_PointInRect(&mousePoint, &rects[i][j].rect)) {
                if(startNumbers[i][j] == 0) {
                    rects[i][j].isClicked = true;
                    SDL_StartTextInput();
                }
                else {
                    update_screen(renderer);
                }

            }

        }
    }

}


int x=0;
int y=0;
void update_rect(SDL_Renderer* renderer, char *string, int option){
//option 1 is a mouse click
    if(option == 1){
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                if(rects[i][j].isClicked == true){
                    x=i;
                    y=j;

                    update_screen(renderer);
                }

            }
        }
    }
//option 2 is a text input
    if(option == 2){
        SDL_StopTextInput();
        if(atoi(string) == correctNumbers[x][y] && startNumbers[x][y] != correctNumbers[x][y]){
            startNumbers[x][y] =atoi(string);
            printf("Correct Number\n\n");
            check_win();
        }
        else
            printf("Wrong input\n");

        update_screen(renderer);

    }

}



int main(void) {




    //picking level,passes level number to get_numbers function
    get_numbers(pick_level());

    bool quit = false;
    SDL_Event event;
    //initialises SDL, creates windows and renderer for sdl, bla bla bla

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Sudoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_MOUSE_FOCUS);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    update_screen(renderer);



    //event handling
    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                check_mouse_in_rect(renderer);
                update_rect(renderer, "", 1);
                break;
            case SDL_TEXTINPUT:


                update_rect(renderer, event.text.text, 2);
                break;
        }
    }
    //cleansing memory
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return 0;
}
