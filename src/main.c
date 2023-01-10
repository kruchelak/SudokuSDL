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
#define ENOUGH snprintf(NULL, 0,"%d",42)


SDL_Rect rects[9][9];
int clickedRect[9][9];


void getNumbers(int level, int startNumbers[9][9]){
    //stworzenie sciezki do pliku, bufor 25 poniewaz taka jest dlugosc stringa + znak \0
    printf("%d\n", level);
//    char path[29];
//    snprintf(path, 29, "./Levels/Level_%d/start_%d.txt", poziom, poziom);
//    printf("%s\n", path); // dziala
    FILE *file = fopen("/Users/kundelski/CLionProjects/SudokuSDL/assets/levels/start1.txt","r");
    if (file == NULL) {
        // wystąpił błąd podczas otwierania pliku
        fprintf(stderr, "Nie udało się otworzyć pliku: %s\n", strerror(errno));
        return;
    }
    //wkladanie liczb z pliku do tablicy
    for(int i = 0; i<9; i++){
        for(int j = 0; j<9; j++){
            fscanf(file, "%d", &startNumbers[j][i]);
        }
    }
    fclose(file);

}

void updateRect(void){
    SDL_StopTextInput();


}
void checkClickInRects(int mouseX, int mouseY) {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            SDL_Point mousePoint = {mouseX, mouseY};
            if (SDL_PointInRect(&mousePoint, &rects[i][j])) {
                printf("Kliknięcie nastąpiło w prostokącie (%d, %d)\n", i, j);
                SDL_StartTextInput();


            }
        }
    }
}
void isMouseInRect(void) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX,&mouseY);
    checkClickInRects(mouseX, mouseY);
}
void createRects(SDL_Renderer* renderer, int startNumbers[9][9]){
    // ustawianie fontu
    TTF_Font *font = TTF_OpenFont("./assets/pacifico/Pacifico.ttf", 24);
    if (!font) {
        fprintf(stderr, "Nie udało się otworzyć fontu: %s\n", TTF_GetError());
        exit(1);
    }
    //ustawianie koloru tekstu
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* textSurfaces[MAX_SURFACES_X][MAX_SURFACES_Y];

    for(int i = 0; i<9 ; i++){
        for(int j = 0; j<9; j++) {
            rects[i][j] = (SDL_Rect){i * (WINDOW_WIDTH / 9), j * (WINDOW_HEIGHT / 9), 50, 50};

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            SDL_RenderDrawRect(renderer, &rects[i][j]);

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
                SDL_RenderCopy(renderer, text_texture, NULL, &rects[i][j]);
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
int main(void) {
    //wczytanie poziomu
//    int level;
//    scanf("%d", &level);
    int startNumbers[9][9];
    getNumbers(1, startNumbers);



    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Sudoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_MOUSE_FOCUS);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //ustawianie koloru tla
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderClear(renderer);
    //rysowanie kwadratow
    createRects(renderer, startNumbers);
    // render linii
    createLines(renderer);



    SDL_RenderPresent(renderer);

    //czyszczenie pamieci
    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                isMouseInRect();
                break;
            case SDL_TEXTINPUT:
                printf("test");
                updateRect();
                break;
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return 0;
}
