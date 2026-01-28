#include "tower_lib.h"


int main(void){
    //Spuštění, deklarace a nastavení
        //Spuštění aplikace
    App application;
    memset(&application, 0, sizeof(App));// Mem LeakFix - Vynuluje celou proměnnou 
    SDL_Event event;
    application.is_running=true;
    application.have_error=false;
    application.current_game_state=0;
    AppStartEndFunc(&application);
        //Načtení save file
    FILE *save;
    int top_score=0;
    LoadSave(&save, &top_score);
        //Nastavení hry
    ResetGame(&application, GAME_STATE_MAIN_MENU);



    //Načtení Assetů
    if(LoadAssets(&application)==false){
        application.is_running=false;
        application.have_error=true;
        printf("Textures are missing!\n");
    }


    //Běh aplikace
    BackgroundMusic(&application);
    while(application.is_running){
        
        //Action Eventy
        while (SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) application.is_running=false; //Vypnutí programu klikem na křížek
            //Zmáčknuti tlačítka na klávesnici
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE) application.is_running=false; // Vypnutí programu zmáčknutím klávesy "ESC"
                if(event.key.keysym.sym == SDLK_m) SDL_MinimizeWindow(application.window); // Minimalizace okna pomocí klávesy "m"
            }

            //Zmáčknutí tlačítka na myši
            if(event.type == SDL_MOUSEBUTTONDOWN){
                if(event.button.button == SDL_BUTTON_LEFT){
                    //Deklarace pro zkrácení podmínky - pozice myši
                    int mouse_x = event.button.x;
                    int mouse_y = event.button.y;

                    //Aplikace je v hlavním menu
                    if(application.current_game_state == GAME_STATE_MAIN_MENU){
                        
                        if(mouse_x >= 490 && mouse_x <= 790 && mouse_y >= 383 && mouse_y <= 503){// Zmáčknutí na tlačítko start
                            printf("Start tower defence\n");
                            application.current_game_state = GAME_STATE_PLAYING;
                        }else
                        if(mouse_x >= 490 && mouse_x <= 790 && mouse_y >= 480 && mouse_y <= 600){// Zmáčknutí na tlačítko quit
                            printf("Quit game\n");
                            application.is_running = false;
                        }
                    }else
                    if(application.current_game_state == GAME_STATE_PLAYING){

                        if(mouse_x >= 1170 && mouse_x <= 1260 && mouse_y >= 610 && mouse_y <= 700){// Zmáčknutí na tlačítko ChangeSpeed
                            application.game_state.is_speed_up_active = !application.game_state.is_speed_up_active;
                            printf("Speed changed: %d\n", application.game_state.is_speed_up_active);
                        }
                        else HandleTowerClick(&application, mouse_x, mouse_y);
                        
                    }else
                    if(application.current_game_state == GAME_STATE_END || application.current_game_state == GAME_STATE_WIN){
                       
                        if(mouse_x >= 490 && mouse_x <= 790 && mouse_y >= 383 && mouse_y <= 503){// Zmáčknutí na tlačítko start
                            printf("Restarting Game\n");
                            ResetGame(&application, GAME_STATE_END);
                            application.current_game_state = GAME_STATE_PLAYING;
                        }
                        else if(mouse_x >= 490 && mouse_x <= 790 && mouse_y >= 480 && mouse_y <= 600){// Zmáčknutí na tlačítko quit
                            printf("Quit game\n");
                            application.is_running = false;
                        }
                    }


                }
            }


        }

        //Grafika programu
        SDL_SetRenderDrawColor(application.renderer, 0, 0, 0, 255); // Černá
        SDL_RenderClear(application.renderer); // Smaž obrazovku

        //Hra
        if(application.current_game_state == GAME_STATE_MAIN_MENU){// Hlavní menu
            RenderMainMenu(&application, top_score);
        }else
        if(application.current_game_state == GAME_STATE_PLAYING){// Hra
            HandleWaves(&application);
            UpdateEnemies(&application.game_state);
            UpdateTowers(&application.game_state);

            if(application.game_state.player_health <= 0){
                application.current_game_state = GAME_STATE_END;
                SaveGame(&save,&application);
            }

            RenderGame(&application);
        }else
        if(application.current_game_state == GAME_STATE_WIN){
            RenderWin(&application);
        }
        else
        if(application.current_game_state == GAME_STATE_END){// End game
            RenderEndGame(&application);
        }

        SDL_RenderPresent(application.renderer); //Načte videoobraz
    }
    //Konec běhu aplikace



    // Konec programu
    if(application.have_error==true){
        AppStartEndFunc(&application);
        return 1;
    }
    else{
        SaveGame(&save,&application);
        AppStartEndFunc(&application);
        return 0;
    }
}