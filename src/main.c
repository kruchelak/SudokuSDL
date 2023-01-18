#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <SDL2_gfxPrimitives.h>
#include <limits.h>
#include <errno.h>

#define MAX_SURFACES_X 9
#define MAX_SURFACES_Y 9
#define WINDOW_WIDTH 450
#define WINDOW_HEIGHT 450
//https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
#define ENOUGH snprintf(NULL, 0,"%d",42)
int startNumbers[9][9];
int correctNumbers[9][9];

typedef struct rect{
    bool isClicked;
    SDL_Rect rect;

}rect;
rect rects[9][9];


void getNumbers(int level){
    //stworzenie sciezki do pliku, bufor 27 poniewaz taka jest dlugosc stringa + znak \0
    printf("%d\n", level);
    char pathstart[27];
    char pathend[25];
    snprintf(pathstart, 27, "./assets/levels/start%d.txt", level);

    snprintf(pathend, 25, "./assets/levels/end%d.txt", level);
    printf("%s\n", pathstart);
    printf("%s\n", pathend);

    FILE *start = fopen(pathstart, "r");
    FILE *correct = fopen(pathend,"r");
    if (start == NULL || correct == NULL) {
        // wystąpił błąd podczas otwierania pliku
        fprintf(stderr, "Nie udało się otworzyć pliku: %s\n", strerror(errno));
        return;
    }
    //wkladanie liczb z pliku do tablicy
    for(int i = 0; i<9; i++){
        for(int j = 0; j<9; j++){
            fscanf(start, "%d", &startNumbers[j][i]);
            fscanf(correct, "%d", &correctNumbers[j][i]);


        }
    }
    fclose(start);
    fclose(correct);


}



void createRects(SDL_Renderer* renderer){
    // ustawianie fontu
    TTF_Font *font = TTF_OpenFont("./assets/OpenSans/OpenSans-Regular.ttf", 128);
    if (!font) {
        fprintf(stderr, "Nie udało się otworzyć fontu: %s\n", TTF_GetError());
        exit(1);
    }
    //ustawianie koloru tekstu
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* textSurfaces[MAX_SURFACES_X][MAX_SURFACES_Y];

    for(int i = 0; i<9 ; i++){
        for(int j = 0; j<9; j++) {
            rects[i][j].rect = (SDL_Rect) {i * (WINDOW_WIDTH / 9), j * (WINDOW_HEIGHT / 9), 50, 50};


            if (rects[i][j].isClicked == true) {
            SDL_SetRenderDrawColor(renderer, 0, 150, 50, 10);
            SDL_RenderFillRect(renderer, &rects[i][j].rect);
        }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(renderer, &rects[i][j].rect);
            }

            rects[i][j].isClicked = false;



//            SDL_RenderDrawRect(renderer, &rects[i][j].rect);

            //przeksztalca cyfre w stringa,
            char number_str[ENOUGH+1];
            sprintf(number_str, "%d", startNumbers[i][j]);


            // tworzenie powierzchni rysowania dla tekstu

            textSurfaces[i][j] = TTF_RenderText_Solid(font, number_str, textColor);
            if (!textSurfaces[i][j]) {
                fprintf(stderr, "Nie udało się stworzyć powierzchni tekstu: %s\n", TTF_GetError());
                exit(1);
            }

            // tworzenie tekstury z powierzchni, brak tekstury jesli pole = 0
            SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, textSurfaces[i][j]);
            if (startNumbers[i][j] != 0) {
                SDL_RenderCopy(renderer, text_texture, NULL, &rects[i][j].rect);
            }

        }

    }


    TTF_CloseFont(font);
    for (int i = 0; i < MAX_SURFACES_X; i++) {
        for (int j = 0; j < MAX_SURFACES_Y; j++) {
            SDL_FreeSurface(textSurfaces[i][j]);
        }
    }
}
void createLines( SDL_Renderer* renderer){
    for(int i =1 ;i<3 ;i++) {
        thickLineRGBA(renderer, (WINDOW_WIDTH/3)*i, 0, (WINDOW_WIDTH/3)*i, WINDOW_HEIGHT, 5, 0, 0, 0, 255);
        thickLineRGBA(renderer, 0, (WINDOW_HEIGHT/3)*i, WINDOW_WIDTH, (WINDOW_HEIGHT/3)*i, 5, 0, 0, 0, 255);

    }
}
int pickLevel(void){

    int level=0;

    printf("Wybierz poziom 1-3\n");
    while(level <1 || level >3){

        scanf("%d", &level);
        if(level <1 || level >3)
         printf("Zły znak.Wybierz liczbe od 1 do 3.\n");

    }


    return level;
}
void updateScreen(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderClear(renderer);
    //rysowanie kwadratow
    createRects(renderer);
    // render linii
    createLines(renderer);

    SDL_RenderPresent(renderer);
}
void isMouseInRect(SDL_Renderer* renderer) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX,&mouseY);
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            SDL_Point mousePoint = {mouseX, mouseY};
            if (SDL_PointInRect(&mousePoint, &rects[i][j].rect)) {
                printf("Kliknięcie nastąpiło w prostokącie (%d, %d)\n", i, j);

                if(startNumbers[i][j] == 0) {
                    rects[i][j].isClicked = true;
                    SDL_StartTextInput();
                }
                else {
                    printf("kliknieto zly prostokat\n");
                    updateScreen(renderer);
                }

            }

        }
    }

}

//x,y recta w tablicy rectow,
// prawdopodobnie nadal lepiej byloby przekazywac tego recta przez funckje znajdujaca nacisnietego,
// trzeba pomyslec czy sie oplaca to zmieniac
//
int x=0;
int y=0;
void updateRect(SDL_Renderer* renderer, char *string, int option){

    if(option == 1){
//        printf("kliknieto\n");
        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 9; j++) {
                if(rects[i][j].isClicked == true){
                    x=i;
                    y=j;
                    updateScreen(renderer);
                }

            }
        }
    }
    if(option == 2){
        SDL_StopTextInput();
//        printf("%d",atoi(string));
//        printf("%d", correctNumbers[x][y]);
        if(atoi(string) == correctNumbers[x][y] && startNumbers[x][y] != correctNumbers[x][y]){
            printf("wpisana dobra liczbe\n");
        }
        else
            printf("wpisano zla liczbe\n");

        updateScreen(renderer);

    }

}



//fills the clicked rect with color, checks after input if the input is correct, and isnt the starting input
int main(void) {




    //wczytanie poziomu

    getNumbers(pickLevel());

    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Sudoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_MOUSE_FOCUS);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //ustawianie koloru tla


    updateScreen(renderer);



    //czyszczenie pamieci
    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                isMouseInRect(renderer);
                updateRect(renderer,"", 1);
                break;
            case SDL_TEXTINPUT:


                updateRect(renderer, event.text.text, 2);
                break;
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return 0;
}
