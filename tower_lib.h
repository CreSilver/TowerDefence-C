#ifndef LIB_H_
#define LIB_H_


//KNIHOVNY
// Standardní C knihovny
#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
// SDL knihovny
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// Konstanty
    // Screen
    #define SCREEN_WIDTH 1280
    #define SCREEN_HEIGHT 720
    // Player
    #define PLAYER_START_HEALTH 20
    #define PLAYER_START_COINS 100
    // TimeScale
    #define STANDART_TIME_SCALE 1.0f
    #define FAST_TIME_SCALE 2.0f
    // Enemy
    #define NUM_OF_ENEMY_TYPES 4
    // Tower
    #define TOWER_COST_BUILD 50
    #define TOWER_COST_UPGRADE 82
    #define TOWER_SIZE 100
    #define TOWER_RANGE_BASE 200
    #define TOWER_DAMAGE_BASE 5
    #define TOWER_FIRE_RATE 28
    SDL_Point tower_positions[3] = { // Pozice věží - levý horná roh
    {272, 210},
    {550, 343},
    {866, 343}
};
    // GameState
    #define GAME_STATE_MAIN_MENU 0
    #define GAME_STATE_PLAYING 1
    #define GAME_STATE_END 2
    #define GAME_STATE_WIN 3
    // PathFinding
    #define PATH_POINTS_COUNT 6
    SDL_Point map_path[PATH_POINTS_COUNT] = {
        {0, 430},    
        {460, 430},  
        {460, 255},  
        {1047, 255},
        {1047, 530},
        {1280,530} 
    };
    // Číslo pí
    #ifndef M_PI
    #define M_PI 3.14159265358979323846
    #endif




//DEKLARACE
// Struktury
    //GAME
    // Enemy
    typedef struct Enemy_{
        // Position
            float x;
            float y;
            float angle;
        // NessData
            int max_hp;
            int actual_hp;
            int velocity;
            int coin_reward;
            int path_waypoint_index;
        // Odkaz na dalšího nepřítele
            struct Enemy_* next;
        // Textura
            SDL_Texture *enemy_textures;
    } Enemy;

    // Tower
    typedef struct Tower_{
        // Position
            int x;
            int y;
        // NessData
            int range;
            int damage;
            float fire_rate;
            float fire_rate_cooldown;
            int level;
            int last_target_x;
            int last_target_y;
        // Odkaz na dalšího nepřítele
            struct Tower_* next;
        // Textura
            SDL_Texture *tower_texture;
    } Tower;


    //CORE
    // GameState
    typedef struct GameState_{
        // Game state
            int player_health;
            int player_coins;
            int current_wave;
            bool is_speed_up_active;

            int enemies_to_spawn;
            Uint32 last_spawn_time;
            Uint32 spawn_interval;
        // For multiple thinks pointing
            Enemy* enemy_list; // Začátek seznamu nepřátel
            Tower* tower_list; // Začátek seznamu věží
    } GameState;

    // Assets
    typedef struct GameAssets_ {
        // Menu/game texture
            SDL_Texture *start_button_texture;
            SDL_Texture *quit_button_texture;
            SDL_Texture *menu_background_texture;
            SDL_Texture *game_map_texture;
            SDL_Texture *game_logo;
            SDL_Texture *fast_time;
            SDL_Texture *fast_time_actvated;
            SDL_Texture *tower_texture;
            SDL_Texture *enemy_types[NUM_OF_ENEMY_TYPES];
        // Font
            TTF_Font* main_font;
        // Audio
            Mix_Music *background_music;
    } GameAssets;

    // Game-structure
    typedef struct App_ {
        SDL_Renderer *renderer;
        SDL_Window *window;

        GameState game_state; // Aktuální herní staty
        GameAssets assets; // Textury

        int current_game_state;
        bool is_running;
        bool have_error;
    } App;




//Funkce
void CleanupAssets(App* self){
    if ((*self).assets.background_music) Mix_FreeMusic((*self).assets.background_music);
    if ((*self).assets.main_font) TTF_CloseFont((*self).assets.main_font);
    if ((*self).assets.game_logo) SDL_DestroyTexture((*self).assets.game_logo);
    if ((*self).assets.menu_background_texture) SDL_DestroyTexture((*self).assets.menu_background_texture);
    if ((*self).assets.quit_button_texture) SDL_DestroyTexture((*self).assets.quit_button_texture);
    if ((*self).assets.start_button_texture) SDL_DestroyTexture((*self).assets.start_button_texture);
    if ((*self).assets.game_map_texture) SDL_DestroyTexture((*self).assets.game_map_texture);
    if ((*self).assets.fast_time) SDL_DestroyTexture((*self).assets.fast_time);
    if ((*self).assets.fast_time_actvated) SDL_DestroyTexture((*self).assets.fast_time_actvated);
    if ((*self).assets.tower_texture) SDL_DestroyTexture((*self).assets.tower_texture);
    for(int i = 0; i < NUM_OF_ENEMY_TYPES; i++) {
        if ((*self).assets.enemy_types[i]) 
            SDL_DestroyTexture((*self).assets.enemy_types[i]);
    }
    Enemy *current_enemy = (*self).game_state.enemy_list;
    while (current_enemy != NULL) {
        Enemy *next_enemy = current_enemy->next;
        free(current_enemy); // Oprava leaku z NewEnemy
        current_enemy = next_enemy;
    }
    (*self).game_state.enemy_list = NULL;
    Tower *current_tower = (*self).game_state.tower_list;
    while (current_tower != NULL) {
        Tower *next_tower = (Tower*)current_tower->next;
        free(current_tower); // Uvolní paměť alokovanou v HandleTowerClick
        current_tower = next_tower;
    }
    (*self).game_state.tower_list = NULL;
}


bool FileExist(const char *filedir){
    FILE* file;
    file = fopen(filedir, "r");
    if (file != NULL) {
        fclose(file);
        return true;
    }
    return false;
}


bool LoadAssets(App *self){
    //Základní nastavení assetu
    TTF_Init(); // Vytvoření textu
    Mix_Init(MIX_INIT_OGG); // Vytvoření audia pro .ogg formát
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096); // Nastavení kvality audia
    Mix_VolumeMusic(80); // Nastavení hlasitosti audia

    //Načtení fontu
    if(FileExist("assets/Fonts/PixelCode/fonts/PixelCode.otf")==true) (*self).assets.main_font=TTF_OpenFont("assets/Fonts/PixelCode/fonts/PixelCode.otf",20);
    else return false;

    //Načtení Loga Hry
    if(FileExist("assets/GameLogo.png")==true) (*self).assets.game_logo = IMG_LoadTexture((*self).renderer,"assets/GameLogo.png");
    else return false;
    
    //Načtení audia
    if(FileExist("assets/Audio/MorningGarden.ogg")==true) (*self).assets.background_music = Mix_LoadMUS("assets/Audio/MorningGarden.ogg");
    else return false;
    
    //Načtení MainMenu textures
    if(FileExist("assets/MainMenu/Background.png")==true) (*self).assets.menu_background_texture = IMG_LoadTexture((*self).renderer,"assets/MainMenu/Background.png");
    else return false;
    if(FileExist("assets/MainMenu/QuitButtom.png")==true) (*self).assets.quit_button_texture = IMG_LoadTexture((*self).renderer,"assets/MainMenu/QuitButtom.png");
    else return false;
    if(FileExist("assets/MainMenu/StartButton.png")==true) (*self).assets.start_button_texture = IMG_LoadTexture((*self).renderer,"assets/MainMenu/StartButton.png");
    else return false;
    
    //Kontrola textur hry
    if(FileExist("assets/Game/game_map.png")==true) (*self).assets.game_map_texture = IMG_LoadTexture((*self).renderer,"assets/Game/game_map.png");
    else return false;
    if(FileExist("assets/Game/speed_button.png")==true) (*self).assets.fast_time = IMG_LoadTexture((*self).renderer,"assets/Game/speed_button.png");
    else return false;
    if(FileExist("assets/Game/speed_button_activated.png")==true) (*self).assets.fast_time_actvated = IMG_LoadTexture((*self).renderer,"assets/Game/speed_button_activated.png");
    else return false;
    if(FileExist("assets/Game/Enemy/01.png")==true) (*self).assets.enemy_types[0] = IMG_LoadTexture((*self).renderer,"assets/Game/Enemy/01.png");
    else return false;
    if(FileExist("assets/Game/Enemy/02.png")==true) (*self).assets.enemy_types[1] = IMG_LoadTexture((*self).renderer,"assets/Game/Enemy/02.png");
    else return false;
    if(FileExist("assets/Game/Enemy/03.png")==true) (*self).assets.enemy_types[2] = IMG_LoadTexture((*self).renderer,"assets/Game/Enemy/03.png");
    else return false;
    if(FileExist("assets/Game/Enemy/04.png")==true) (*self).assets.enemy_types[3] = IMG_LoadTexture((*self).renderer,"assets/Game/Enemy/04.png");
    else return false;
    if(FileExist("assets/Game/tower.png")==true) (*self).assets.tower_texture = IMG_LoadTexture((*self).renderer,"assets/Game/tower.png");
    else return false;

    return true;
}


void BackgroundMusic(App *self){
    Mix_PlayMusic((*self).assets.background_music, -1);
}


void LoadSave(FILE **save,int *top_score){ // Načtení save file (wave top_score) - pokud neexistuje tak se vytvoří
    if(FileExist("game.save")==false){
        *save = fopen("game.save", "w+");
        fclose(*save);
        *save = fopen("game.save", "r+");
    }
    else *save = fopen("game.save", "r+");
    if(fscanf(*save, "%d", top_score)==-1){
        *top_score=0;
        rewind(*save);
        fprintf(*save, "%d",*top_score);
        fflush(*save);
    }
    fclose(*save);
}


void SaveGame(FILE **save, App *self){ // Uložení top_score (wave) do save file
    *save = fopen("game.save", "r+");
    int mem=0;
    fscanf(*save, "%d", &mem);
    if(mem < (*self).game_state.current_wave){
        mem = (*self).game_state.current_wave-1;
        rewind(*save);
        fprintf(*save, "%d", mem);
        fflush(*save);
    }
    fclose(*save);
}


void SpawnEnemy(GameState *game, GameAssets *assets, int type){
    // Alokace paměti
    Enemy* new_enemy = malloc(sizeof(Enemy));
    

    // Startovní pozice (Bod 0 z tvé mapy)
    (*new_enemy).x = (float)map_path[0].x; 
    (*new_enemy).y = (float)map_path[0].y; 
    (*new_enemy).path_waypoint_index = 1; // cesta textury
    (*new_enemy).angle = 0.0; // směr textury


    // Nastavení textury
    (*new_enemy).enemy_textures = assets->enemy_types[type];


    // Statistiky podle typu
    // Typ 0: Normální
    if (type == 0) {
        (*new_enemy).max_hp = 15 + ((*game).current_wave * 5.2);
        (*new_enemy).velocity = 2; // Rychlost
        (*new_enemy).coin_reward = 5;
    } 
    // Typ 1: Rychlý (Scout)
    else if (type == 1) {
        (*new_enemy).max_hp = 10 + (((*game).current_wave+1) * 2);
        (*new_enemy).velocity = 4 + (((*game).current_wave * 0.5) * 0.2); // Rychlý!
        (*new_enemy).coin_reward = 8;
    }
    // Typ 2: Tank (Pomalý, drží hodně)
    else if (type == 2) {
        (*new_enemy).max_hp = 60 + (((*game).current_wave+1.2) * 10);
        (*new_enemy).velocity = 1; // Pomalý
        (*new_enemy).coin_reward = 15;
    }
    // Typ 3: "Strong"
    else {
        (*new_enemy).max_hp = 100 + (((*game).current_wave+1.5) * 15);
        (*new_enemy).velocity = 1; 
        (*new_enemy).coin_reward = 25;
    }
    (*new_enemy).actual_hp = (*new_enemy).max_hp;


    // Přidání do seznamu (Linked List)
    (*new_enemy).next = (*game).enemy_list; 
    (*game).enemy_list = new_enemy;       
}


void UpdateEnemies(GameState *game) {
    Enemy *current = (*game).enemy_list;
    Enemy *prev = NULL; 
    

    while (current != NULL) {
        int idx = (*current).path_waypoint_index;
        
        // Došel na konec NEBO má 0 životů
        bool reached_end = (idx >= PATH_POINTS_COUNT);
        bool is_dead = ((*current).actual_hp <= 0);

        if (reached_end || is_dead) {
            
            // Pokud došel na konec, ubere život hráči
            if (reached_end && !is_dead) {
                (*game).player_health--; 
            }

            // Smazat nepřítele ze seznamu
            Enemy *to_delete = current;
            if (prev == NULL) (*game).enemy_list = (*current).next;
            else prev->next = (*current).next;
            
            current = (*current).next;
            free(to_delete);
            continue; 
        }

        // Pohyb
        float target_x = (float)map_path[idx].x;
        float target_y = (float)map_path[idx].y;
        float dx = target_x - (*current).x;
        float dy = target_y - (*current).y;
        float distance = sqrt(dx*dx + dy*dy)-3;
        
        if(distance > 0) {
            (*current).angle = atan2(dy, dx) * (180.0 / M_PI);
        }
            
        float time_scale = STANDART_TIME_SCALE; // Zjištění zrychlení
        if ((*game).is_speed_up_active) {
            time_scale = FAST_TIME_SCALE;
        }
        float current_speed = (*current).velocity * time_scale;

        if (distance < current_speed) {
            (*current).x = target_x;
            (*current).y = target_y;
            (*current).path_waypoint_index++; 
        } 
        else {
            (*current).x += (dx / distance) * current_speed;
            (*current).y += (dy / distance) * current_speed;
        }

        prev = current;
        current = (*current).next;
    }
}


void HandleWaves(App *self) {
    Uint32 current_time = SDL_GetTicks(); // časovač
    GameState *game = &(*self).game_state;
    GameAssets *assets = &(*self).assets;


    if((*game).current_wave > 15){
        (*self).current_game_state = GAME_STATE_END;
        return;
    }


    // Spawn Enemy
    if ((*game).enemies_to_spawn > 0) {
        if (current_time > (*game).last_spawn_time + (*game).spawn_interval) { // jestliže uběhl time interval
            
            // Vybereme náhodný typ nepřítele (0 až 3)
            // V prvních wave jen lehčí, později těžší
            int type = 0;
            if ((*game).current_wave < 2) type = 0; // Jen základní
            else if ((*game).current_wave < 5) type = rand() % 2; // 0 nebo 1
            else type = rand() % 4; // Všichni

            SpawnEnemy(game, assets, type);
            
            (*game).last_spawn_time = current_time;
            (*game).enemies_to_spawn--;
        }
    }
    //Konec wave
    else if ((*game).enemy_list == NULL) {
        (*game).current_wave++;
        FILE *temp_save;
        SaveGame(&temp_save, self);
    

            // Zvýšíme počet nepřátel
        (*game).enemies_to_spawn = 5 + ((*game).current_wave * 2); 
        

            // Zrychlíme interval (nepřátelé chodí častěji), min 500ms
        (*game).spawn_interval = 1500 - ((*game).current_wave * 50);
        if((*game).spawn_interval < 500) (*game).spawn_interval = 500;
    }
}


void UpdateTowers(GameState *game) {
    float time_scale = STANDART_TIME_SCALE; // Zjištění zrychlení
    if ((*game).is_speed_up_active) {
        time_scale = FAST_TIME_SCALE;
    }
    Tower *t = (*game).tower_list;
    
    while (t != NULL) {
        // Cooldown zbraně
        if (t->fire_rate_cooldown > 0) {
            t->fire_rate_cooldown -= time_scale;
        }

        // Pokud je připravena střílet
        if (t->fire_rate_cooldown <= 0) {
            Enemy *e = (*game).enemy_list;
            Enemy *target = NULL;
            float min_dist = 10000.0f;

            // Hledání nejbližšího nepřítele v dosahu
            while (e != NULL) {
                // Střed věže
                float tx = t->x + (TOWER_SIZE / 2);
                float ty = t->y + (TOWER_SIZE / 2);
                
                // Vzdálenost k nepříteli
                float dx = e->x - tx;
                float dy = e->y - ty;
                float dist = sqrt(dx*dx + dy*dy);

                if (dist <= t->range && dist < min_dist) {
                    min_dist = dist;
                    target = e;
                }
                e = e->next;
            }

            // Výstřel
            if (target != NULL) {
                target->actual_hp -= t->damage;
                t->fire_rate_cooldown = t->fire_rate;
                
                t->last_target_x = (int)target->x + 25; // +25 je polovina velikosti nepřítele (50/2)
                t->last_target_y = (int)target->y + 25;

                if (target->actual_hp <= 0) {
                    (*game).player_coins += target->coin_reward;
                }
            }
        }
        t = (Tower*)t->next;
    }
}


void HandleTowerClick(App *self, int mouse_x, int mouse_y) {
    GameState *game = &(*self).game_state;
    
    for (int i = 0; i < 3; i++) {
        int tx = tower_positions[i].x;
        int ty = tower_positions[i].y;

        // Kontrola, zda jsme klikli na toto místo
        if (mouse_x >= tx && mouse_x <= tx + TOWER_SIZE &&
            mouse_y >= ty && mouse_y <= ty + TOWER_SIZE) {

            // 1. Zjistíme, zda tu už věž je
            Tower *current = (*game).tower_list;
            Tower *found_tower = NULL;
            while (current != NULL) {
                if (current->x == tx && current->y == ty) {
                    found_tower = current;
                    break;
                }
                current = (Tower*)current->next; 
            }

            // 2. Logika NÁKUPU (pokud věž není)
            if (found_tower == NULL) {
                if ((*game).player_coins >= TOWER_COST_BUILD) {
                    (*game).player_coins -= TOWER_COST_BUILD;

                    // Vytvoření nové věže
                    Tower *new_tower = malloc(sizeof(Tower));
                    new_tower->x = tx;
                    new_tower->y = ty;
                    new_tower->range = TOWER_RANGE_BASE;
                    new_tower->damage = TOWER_DAMAGE_BASE;
                    new_tower->fire_rate = TOWER_FIRE_RATE;
                    new_tower->fire_rate_cooldown = 0;
                    new_tower->level = 1;
                    new_tower->tower_texture = (*self).assets.tower_texture; // Sdílená textura

                    // Přidání na začátek seznamu
                    new_tower->next = (struct Tower_*)((*game).tower_list);
                    (*game).tower_list = new_tower;
                    
                    printf("Tower Built at %d, %d\n", tx, ty);
                } else {
                    printf("Not enough coins to build! Need %d\n", TOWER_COST_BUILD);
                }
            } 
            // 3. Logika UPGRADE (pokud věž existuje)
            else {
                if ((*game).player_coins >= TOWER_COST_UPGRADE) {
                    (*game).player_coins -= TOWER_COST_UPGRADE;
                    
                    found_tower->level++;
                    found_tower->damage += 2;      // Zvýšíme damage
                    found_tower->range += 20;      // Zvýšíme dosah
                    found_tower->fire_rate *= 0.9; // Zrychlíme střelbu o 10%
                    
                    printf("Tower Upgraded to Level %d\n", found_tower->level);
                } else {
                    printf("Not enough coins to upgrade! Need %d\n", TOWER_COST_UPGRADE);
                }
            }
            return; // Kliknuto, končíme prohledávání
        }
    }
}


void RenderEnemies(App *self) {
    Enemy *enemy = (*self).game_state.enemy_list;
    
    while(enemy != NULL) {
        SDL_Rect enemy_rect;
        // Velikost
        enemy_rect.w = 50;  
        enemy_rect.h = 50;
        // Umístění (střed)
        enemy_rect.x = (int)enemy->x - (enemy_rect.w / 2); 
        enemy_rect.y = (int)enemy->y - (enemy_rect.h / 2);

        // Vykreslení textury (rotace)
        SDL_RenderCopyEx(
            (*self).renderer, 
            enemy->enemy_textures, 
            NULL, 
            &enemy_rect, 
            enemy->angle, 
            NULL, 
            SDL_FLIP_NONE
        );

        // HealthBar
        if (enemy->actual_hp < enemy->max_hp) {
            float hp_percent = (float)enemy->actual_hp / (float)enemy->max_hp;
            if (hp_percent < 0) hp_percent = 0;

            // Pozadí (Červená)
            SDL_Rect hp_bg_rect;
            hp_bg_rect.x = enemy_rect.x;
            hp_bg_rect.y = enemy_rect.y - 8; 
            hp_bg_rect.w = 50;
            hp_bg_rect.h = 4;
            
            SDL_SetRenderDrawColor((*self).renderer, 255, 0, 0, 255);
            SDL_RenderFillRect((*self).renderer, &hp_bg_rect);

            // Popředí (Zelená)
            SDL_Rect hp_fg_rect = hp_bg_rect;
            hp_fg_rect.w = (int)(50 * hp_percent);
            
            SDL_SetRenderDrawColor((*self).renderer, 0, 255, 0, 255);
            SDL_RenderFillRect((*self).renderer, &hp_fg_rect);
        }
        
        enemy = enemy->next;
    }
}


void RenderTowers(App *self) {
    Tower *t_render = (*self).game_state.tower_list;
    
    while(t_render != NULL) {
        SDL_Rect tower_rect;
        tower_rect.x = t_render->x;
        tower_rect.y = t_render->y;
        tower_rect.w = TOWER_SIZE;
        tower_rect.h = TOWER_SIZE;

        SDL_RenderCopy((*self).renderer, t_render->tower_texture, NULL, &tower_rect);

        // Vykreslení laseru (žlutá příprava)
        if (t_render->fire_rate_cooldown > (t_render->fire_rate - 5)) {
            SDL_SetRenderDrawColor((*self).renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect((*self).renderer, &tower_rect);
        }
        // Vykreslení výstřelu (modrá čára)
        if (t_render->fire_rate_cooldown > (t_render->fire_rate - 10)){ 
            SDL_SetRenderDrawColor((*self).renderer, 85, 85, 255, 255);

            // Střed věže
            int start_x = t_render->x + (TOWER_SIZE / 2);
            int start_y = t_render->y + (TOWER_SIZE / 2);

            // Kreslení čáry k uloženému cíli
            SDL_RenderDrawLine((*self).renderer, start_x, start_y, t_render->last_target_x, t_render->last_target_y);
            // Tlustší čára
            SDL_RenderDrawLine((*self).renderer, start_x+1, start_y+1, t_render->last_target_x+1, t_render->last_target_y+1);
        }

        t_render = (Tower*)t_render->next;
    }
}




void ResetGame(App *self, int state){
    // Reset Hodnot
    (*self).game_state.player_health = PLAYER_START_HEALTH;
    (*self).game_state.player_coins = PLAYER_START_COINS;
    (*self).game_state.current_wave = 0;
    (*self).game_state.enemies_to_spawn = 0;
    (*self).game_state.spawn_interval = 1000;
    (*self).game_state.last_spawn_time = 0;
    (*self).game_state.is_speed_up_active = false;


    if(state == 2){// Pokud hra skončila
        // Smazání všech nepřátel z mapy
        Enemy *current = (*self).game_state.enemy_list;
        while (current != NULL) {
            Enemy *next = current->next;
            free(current);
            current = next;
        }
        (*self).game_state.enemy_list = NULL;

        // Smazání všech věží z mapy
        Tower *curr_t = (*self).game_state.tower_list;
        while (curr_t != NULL) {
            Tower *next_t = (Tower*)curr_t->next;
            free(curr_t);
            curr_t = next_t;
        }
        (*self).game_state.tower_list = NULL;
        
    }
}


void RenderGame(App *self){//TowerDefence
    // Vykreslí mapu hry
    SDL_RenderCopy((*self).renderer, (*self).assets.game_map_texture, NULL, NULL);


    //Nastavení barvy textu
    SDL_Color white = {255, 255, 255, 255};


    // Vykreslení počtu životů
    char life[50];
    sprintf(life,"%d",(*self).game_state.player_health); //Vytvoření textu
    SDL_Surface *set_text = TTF_RenderText_Solid((*self).assets.main_font, life, white);//Nastavení fontu a barvy textu
    SDL_Texture *texture_text = SDL_CreateTextureFromSurface((*self).renderer, set_text);//Vytvoření textu do obrazce
        //Umístění
    SDL_Rect text_rect;
        //Velikost
    text_rect.w = (*set_text).w;
    text_rect.h = (*set_text).h;
        //Posun
    text_rect.x = 80;
    text_rect.y = 30;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, texture_text, NULL, &text_rect);
        //Uvolnění textu z paměti - u while cyklu se se neustále vytvářel ale neuvolnil by se a tak by dříve nepo později došla pamět
    SDL_FreeSurface(set_text);
    SDL_DestroyTexture(texture_text);


     // Vykreslení wave
    char wave[50];
    sprintf(wave,"%d",(*self).game_state.current_wave); //Vytvoření textu
    SDL_Surface *set_text1 = TTF_RenderText_Solid((*self).assets.main_font, wave, white);//Nastavení fontu a barvy textu
    SDL_Texture *texture_text1 = SDL_CreateTextureFromSurface((*self).renderer, set_text1);//Vytvoření textu do obrazce
        //Umístění
    SDL_Rect text_rect1;
        //Velikost
    text_rect1.w = (*set_text1).w;
    text_rect1.h = (*set_text1).h;
        //Posun
    text_rect1.x = 407;
    text_rect1.y = 30;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, texture_text1, NULL, &text_rect1);
        //Uvolnění textu z paměti - u while cyklu se se neustále vytvářel ale neuvolnil by se a tak by dříve nepo později došla pamět
    SDL_FreeSurface(set_text1);
    SDL_DestroyTexture(texture_text1);


    // Vykreslení wave
    char coins[50];
    sprintf(coins,"%d",(*self).game_state.player_coins); //Vytvoření textu
    SDL_Surface *set_text2 = TTF_RenderText_Solid((*self).assets.main_font, coins, white);//Nastavení fontu a barvy textu
    SDL_Texture *texture_text2 = SDL_CreateTextureFromSurface((*self).renderer, set_text2);//Vytvoření textu do obrazce
        //Umístění
    SDL_Rect text_rect2;
        //Velikost
    text_rect2.w = (*set_text2).w;
    text_rect2.h = (*set_text2).h;
        //Posun
    text_rect2.x = 870;
    text_rect2.y = 30;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, texture_text2, NULL, &text_rect2);
        //Uvolnění textu z paměti - u while cyklu se se neustále vytvářel ale neuvolnil by se a tak by dříve nepo později došla pamět
    SDL_FreeSurface(set_text2);
    SDL_DestroyTexture(texture_text2);


    //Vykreslení SpeedScale button
    SDL_Texture *speed_texture;
        // Změna textury pokud je tlačítko aktivní
    if ((*self).game_state.is_speed_up_active) {
        speed_texture = (*self).assets.fast_time_actvated; 
    } else {
        speed_texture = (*self).assets.fast_time;
    }
    SDL_Rect speed_rect;
        // Velikost
    speed_rect.w = 90;
    speed_rect.h = 90;
        // Pozice
    speed_rect.x = 1170; 
    speed_rect.y = 610;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, speed_texture, NULL, &speed_rect);


    //Vykreslení cena věží
    char prices_text[100];
    sprintf(prices_text, "Build: %d | Upgr: %d", TOWER_COST_BUILD, TOWER_COST_UPGRADE);
    SDL_Surface *surf_prices = TTF_RenderText_Solid((*self).assets.main_font, prices_text, white);
    SDL_Texture *tex_prices = SDL_CreateTextureFromSurface((*self).renderer, surf_prices);
        // Velikost a umístění
    SDL_Rect rect_prices;
    rect_prices.w = surf_prices->w;
    rect_prices.h = surf_prices->h;
    rect_prices.x = 20; 
    rect_prices.y = SCREEN_HEIGHT - rect_prices.h - 20;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, tex_prices, NULL, &rect_prices);
        //Uvolnění textu z paměti - u while cyklu se se neustále vytvářel ale neuvolnil by se a tak by dříve nepo později došla pamět
    SDL_FreeSurface(surf_prices);
    SDL_DestroyTexture(tex_prices);


    //Vykreslení nepřátel
    RenderEnemies(self);


    //Vykreslení Towerek
    RenderTowers(self);
}


void RenderMainMenu(App *self, int best_wave){//MainMenu
    // Vykreslí pozadí
    SDL_RenderCopy((*self).renderer, (*self).assets.menu_background_texture, NULL, NULL);


    //Umístění Loga
        //Umístění
    SDL_Rect logo_rect;
        //Velikost
    logo_rect.w = 735;
    logo_rect.h = 185;
        //Posun
    logo_rect.x = 272;
    logo_rect.y = 50;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, (*self).assets.game_logo, NULL, &logo_rect);


    //Umístění textu top score
    char best_wave_text[50];
    sprintf(best_wave_text,"Best wave: %d",best_wave);//Vytvoření textu
    SDL_Color white = {255, 255, 255, 255}; //Vytvoření barvy pro text
    SDL_Surface *set_text = TTF_RenderText_Solid((*self).assets.main_font, best_wave_text, white);//Nastavení fontu a barvy textu
    SDL_Texture *texture_text = SDL_CreateTextureFromSurface((*self).renderer, set_text);//Vytvoření textu do obrazce
        //Umístění
    SDL_Rect text_rect;
        //Velikost
    text_rect.w = (*set_text).w;
    text_rect.h = (*set_text).h;
        //Posun
    text_rect.x = (SCREEN_WIDTH - (*set_text).w) / 2; //Vycentruje text
    text_rect.y = 250;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, texture_text, NULL, &text_rect);
        //Uvolnění textu z paměti - u while cyklu se se neustále vytvářel ale neuvolnil by se a tak by dříve nepo později došla pamět
    SDL_FreeSurface(set_text);
    SDL_DestroyTexture(texture_text);


    //Start
        //Umístění
    SDL_Rect but_start_rect;
        //Velikost
    but_start_rect.w = 300;
    but_start_rect.h = 120;
        //Posun
    but_start_rect.x = 490; 
    but_start_rect.y = 383;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, (*self).assets.start_button_texture, NULL, &but_start_rect);


    //Quit
        //Umístění
    SDL_Rect but_quit_rect;
        //Velikost
    but_quit_rect.w = 300;
    but_quit_rect.h = 120;
        //Posun
    but_quit_rect.x = 490;
    but_quit_rect.y = 480;
        //Vykreslení
    SDL_RenderCopy((*self).renderer, (*self).assets.quit_button_texture, NULL, &but_quit_rect);
}


void RenderEndGame(App *self){
    // Pozadí 
    SDL_RenderCopy((*self).renderer, (*self).assets.menu_background_texture, NULL, NULL);


    // Nastavení barev
    SDL_Color red = {255, 50, 50, 255};
    SDL_Color white = {255, 255, 255, 255};


    // Nápis: GAME OVER
    SDL_Surface *surf_over = TTF_RenderText_Solid((*self).assets.main_font, "GAME OVER", red);
    SDL_Texture *tex_over = SDL_CreateTextureFromSurface((*self).renderer, surf_over);
      // Velikost, umístění
    SDL_Rect rect_over;
    rect_over.w = (*surf_over).w * 2; // Zvětšíme text 2x
    rect_over.h = (*surf_over).h * 2;
    rect_over.x = (SCREEN_WIDTH - rect_over.w) / 2;
    rect_over.y = 150;
      // Uvolnění paměti
    SDL_RenderCopy((*self).renderer, tex_over, NULL, &rect_over);
    SDL_FreeSurface(surf_over);
    SDL_DestroyTexture(tex_over);


    // Nápis: Your wave is X
      //Text
    char score_text[50];
    sprintf(score_text, "Your wave: %d", (*self).game_state.current_wave-1);
    
    SDL_Surface *surf_score = TTF_RenderText_Solid((*self).assets.main_font, score_text, white);
    SDL_Texture *tex_score = SDL_CreateTextureFromSurface((*self).renderer, surf_score);
      // Velikost, umístění
    SDL_Rect rect_score;
    rect_score.w = surf_score->w;
    rect_score.h = surf_score->h;
    rect_score.x = (SCREEN_WIDTH - rect_score.w) / 2;
    rect_score.y = 250;
      // Uvolnění paměti
    SDL_RenderCopy((*self).renderer, tex_score, NULL, &rect_score);
    SDL_FreeSurface(surf_score);
    SDL_DestroyTexture(tex_score);


    //Start button
    SDL_Rect but_start_rect = {490, 383, 300, 120};
    SDL_RenderCopy((*self).renderer, (*self).assets.start_button_texture, NULL, &but_start_rect);


    // Quit button
    SDL_Rect but_quit_rect = {490, 480, 300, 120};
    SDL_RenderCopy((*self).renderer, (*self).assets.quit_button_texture, NULL, &but_quit_rect);
}



void RenderWin(App *self){
    // Pozadí 
    SDL_RenderCopy((*self).renderer, (*self).assets.menu_background_texture, NULL, NULL);


    // Nastavení barev
    SDL_Color red = {255, 50, 50, 255};
    SDL_Color white = {255, 255, 255, 255};


    // Nápis: VICTORY
    SDL_Surface *surf_over = TTF_RenderText_Solid((*self).assets.main_font, "VICTORY", red);
    SDL_Texture *tex_over = SDL_CreateTextureFromSurface((*self).renderer, surf_over);
      // Velikost, umístění
    SDL_Rect rect_over;
    rect_over.w = (*surf_over).w * 2; // Zvětšíme text 2x
    rect_over.h = (*surf_over).h * 2;
    rect_over.x = (SCREEN_WIDTH - rect_over.w) / 2;
    rect_over.y = 150;
      // Uvolnění paměti
    SDL_RenderCopy((*self).renderer, tex_over, NULL, &rect_over);
    SDL_FreeSurface(surf_over);
    SDL_DestroyTexture(tex_over);


    // Nápis: Your wave is X
      //Text
    char score_text[50];
    sprintf(score_text, "Your wave: %d", (*self).game_state.current_wave-1);
    
    SDL_Surface *surf_score = TTF_RenderText_Solid((*self).assets.main_font, score_text, white);
    SDL_Texture *tex_score = SDL_CreateTextureFromSurface((*self).renderer, surf_score);
      // Velikost, umístění
    SDL_Rect rect_score;
    rect_score.w = surf_score->w;
    rect_score.h = surf_score->h;
    rect_score.x = (SCREEN_WIDTH - rect_score.w) / 2;
    rect_score.y = 250;
      // Uvolnění paměti
    SDL_RenderCopy((*self).renderer, tex_score, NULL, &rect_score);
    SDL_FreeSurface(surf_score);
    SDL_DestroyTexture(tex_score);


    //Start button
    SDL_Rect but_start_rect = {490, 383, 300, 120};
    SDL_RenderCopy((*self).renderer, (*self).assets.start_button_texture, NULL, &but_start_rect);


    // Quit button
    SDL_Rect but_quit_rect = {490, 480, 300, 120};
    SDL_RenderCopy((*self).renderer, (*self).assets.quit_button_texture, NULL, &but_quit_rect);
}



void AppStartEndFunc(App *self){//Zapíná | vypíná aplikaci
    bool no_shutdown=(*self).is_running;
    if(no_shutdown==true){//Kontrola zda se má okno začít spouštět či se má začít vypínat
        (*self).window=SDL_CreateWindow(//Vytvoří okno s názvem "Tower Defense", které se bude nacházet uprostřed s předem definovanými rozměry
            "Tower Defense",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN //Nastaví aby okno bylo viditelné
        );
        (*self).renderer = SDL_CreateRenderer((*self).window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        // Vytvoří možnost vytvoření okna
        // Vsync omezí hru na 60 FPS
    }else{//Potřebné věci pro ukončení programu
        CleanupAssets(self);
        Mix_CloseAudio();
        Mix_Quit();

        SDL_DestroyRenderer((*self).renderer);
        SDL_DestroyWindow((*self).window);

        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
}


#endif