#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <SDL2_gfxPrimitives.h>
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


void get_numbers(int level){
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
int check_win(void){
    int a = memcmp(correctNumbers, startNumbers, sizeof(correctNumbers));
    if(!a)
        return 0; // arrays are equal
    else
        return 1; //error, arrays are not equal
}


void create_rects(SDL_Renderer* renderer){
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
    if(!check_win()){
        printf("WYGRANA!");
    }

    TTF_CloseFont(font);
    for (int i = 0; i < MAX_SURFACES_X; i++) {
        for (int j = 0; j < MAX_SURFACES_Y; j++) {
            SDL_FreeSurface(textSurfaces[i][j]);
        }
    }
}
void create_lines(SDL_Renderer* renderer){
    for(int i =1 ;i<3 ;i++) {
        thickLineRGBA(renderer, (WINDOW_WIDTH/3)*i, 0, (WINDOW_WIDTH/3)*i, WINDOW_HEIGHT, 5, 0, 0, 0, 255);
        thickLineRGBA(renderer, 0, (WINDOW_HEIGHT/3)*i, WINDOW_WIDTH, (WINDOW_HEIGHT/3)*i, 5, 0, 0, 0, 255);

    }
}
int pick_level(void){

    int level=0;

    printf("Wybierz poziom 1-3\n");
    while(level <1 || level >3){

        scanf("%d", &level);
        if(level <1 || level >3)
         printf("Zły znak.Wybierz liczbe od 1 do 3.\n");

    }


    return level;
}
void update_screen(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderClear(renderer);
    //rysowanie kwadratow
    create_rects(renderer);
    // render linii
    create_lines(renderer);

    SDL_RenderPresent(renderer);
}
void check_mouse_in_rect(SDL_Renderer* renderer) {
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
                    update_screen(renderer);
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

                    update_screen(renderer);
                }

            }
        }
    }
    if(option == 2){
        SDL_StopTextInput();
//        printf("%d",atoi(string));
//        printf("%d", correctNumbers[x][y]);
        if(atoi(string) == correctNumbers[x][y] && startNumbers[x][y] != correctNumbers[x][y]){
            startNumbers[x][y] =atoi(string);
            printf("wpisana dobra liczbe\n\n");
            check_win();
        }
        else
            printf("wpisano zla liczbe\n");

        update_screen(renderer);

    }

}



int main(void) {




    //wczytanie poziomu

    get_numbers(pick_level());

    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Sudoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_MOUSE_FOCUS);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //ustawianie koloru tla


    update_screen(renderer);



    //czyszczenie pamieci
    while (!quit) {
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
//                check_win();
                check_mouse_in_rect(renderer);
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
