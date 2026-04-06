# TOWER DEFENCE
Školní projekt do předmětu UPR

## FUNKCIONALITA

- Hra je dělaná na pevné rozlišení HD neboli 1280x720 a tudíž změna velikosti či maximalizace může rozbít grafiku hry, ačkoliv by tato funkce neměla být povolena tak - to není potřeba zkoušet.
- Hra má taktéž nějaké klávesové zkratky které je dobré znát:
  - Klávesa "ESC" - ihned ukončí hru
  - Klávesa "m" - minimalizuje okno hry
- Hra obsahuje 4 typy enemy, několik růzdných menu, speed-scale systém a update systém a taky win screen.

Jak daná hra funguje:

Celý projekt je strukturován do dvou klíčových souborů, kde main.c funguje jako spouštěcí bod a tower_lib.h jako obsáhlá knihovna definující veškerou logiku, datové struktury a konstanty. Program začíná ve funkci main, která nejprve vynuluje paměť pro hlavní strukturu App pomocí memset a zavolá funkci AppStartEndFunc. Tato funkce inicializuje grafické rozhraní SDL2, vytvoří okno uprostřed obrazovky o rozměrech 1280x720 a inicializuje renderovací jádro s akcelerací. Následuje volání funkce LoadSave, která ověří existenci souboru "game.save" pomocí pomocné funkce FileExist, a pokud neexistuje, vytvoří jej; následně z něj načte nejvyšší dosažené skóre (Wave). Klíčovým krokem je volání funkce LoadAssets, která zavádí do paměti externí soubory: inicializuje písma (TTF), audio (Mixer) a obrázky (IMG). Tato funkce postupně načítá textury pro menu, mapu, tlačítka a všechny typy nepřátel, přičemž každé načtení je jištěno kontrolou existence souboru. Pokud je vše úspěšné, spustí se BackgroundMusic pro přehrávání hudební smyčky.

Jádro aplikace běží v nekonečném while cyklu ve funkci main, který je řízen booleovskou proměnnou is_running. V každém průchodu cyklem se nejprve zavolá SDL_PollEvent pro zpracování vstupů. Zde se detekuje stisk klávesy ESC pro ukončení, 'm' pro minimalizaci, nebo kliknutí myši. Kliknutí jsou rozdělena podle aktuálního stavu hry (current_game_state): v menu ovládají tlačítka Start/Quit, ve hře volají funkci HandleTowerClick nebo přepínají rychlost hry, a na konci hry umožňují restart. Funkce HandleTowerClick kontroluje, zda souřadnice myši spadají do oblasti pro stavbu věže. Pokud ano a na místě věž není, zkontroluje dostatek peněz (player_coins) a alokuje novou paměť (malloc) pro strukturu Tower, kterou zařadí na začátek spojového seznamu věží. Pokud věž existuje, provede její upgrade (zvýšení damage, dosahu a rychlosti střelby).

Logika hry je aktualizována v reálném čase. Funkce HandleWaves hlídá čas pomocí SDL_GetTicks. Pokud uplyne interval pro spawn, zavolá SpawnEnemy. Tato funkce dynamicky alokuje paměť pro nového nepřítele typu Enemy, nastaví mu statistiky (životy, rychlost) podle typu a aktuální vlny, a vloží ho do spojového seznamu nepřátel. Pohyb zajišťuje funkce UpdateEnemies, která pro každého nepřítele vypočítá vektor k dalšímu bodu na mapě (map_path). Pomocí funkce atan2 určí úhel natočení textury a posune nepřítele o jeho rychlost, která může být modifikována zrychlením hry (time_scale). Pokud nepřítel dojde na konec cesty, ubere život hráči, a pokud zemře, je uvolněn z paměti a odstraněn ze seznamu. Funkce UpdateTowers iteruje přes všechny věže, odečítá jejich cooldown (opět ovlivněný zrychlením hry) a pomocí Pythagorovy věty hledá nejbližšího nepřítele v dosahu. Při výstřelu okamžitě sníží životy cíle a uloží souřadnice pro vykreslení laseru.

Vykreslování zajišťuje funkce RenderGame, která postupně volá SDL_RenderCopy pro pozadí mapy, vykresluje texty (životy, peníze, vlny) pomocí TTF_RenderText_Solid a následně volá specializované funkce RenderEnemies a RenderTowers. RenderEnemies kreslí textury nepřátel s rotací a nad nimi vypočítává a kreslí Health Bar (červený a zelený obdélník). RenderTowers kreslí věže a pokud došlo k výstřelu, vykreslí pomocí SDL_RenderDrawLine čáru (laser) k cíli. Stavy Menu a EndGame jsou vykreslovány separátními funkcemi RenderMainMenu a RenderEndGame.

Při ukončení hry, ať už restartem (ResetGame) nebo vypnutím, se volá funkce CleanupAssets. Ta je kritická pro správu paměti: postupně prochází spojové seznamy nepřátel a věží a pro každý prvek volá free, aby nedošlo k úniku paměti (Memory Leak). Následně pomocí SDL funkcí (SDL_DestroyTexture, Mix_FreeMusic) uvolní veškeré grafické a zvukové prostředky. Nakonec funkce SaveGame porovná aktuální vlnu s uloženým rekordem a v případě zlepšení soubor přepíše.

# INSTALACE

Pro kompilaci a spuštění hry je potřeba mít nainstalované několik knihoven a souborů

  1. GCC kompilátor pomocí, kterého se daná hra kompiluje
  2. MakeFile aby bylo možné spustit příkaz "make"
  3. Potřebné knihovny na SDL2 a to: SDL2/SDL.h ,SDL2/SDL_image.h, SDL2/SDL_ttf.h, SDL2/SDL_mixer.h

Příkazy pro Ubuntu:
  sudo apt update
  Instalace GCC: sudo apt install gcc-14 g++-14
  Instalace MakeFile: sudo apt install make
  Instalace SDL2 Knihoven: sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

Pokud jsou všechny potřebné knihovny nainstalovány tak by mělo stačit do terminálu napsat příkaz: make
Tento příkaz kompiluje hru do názvu: TowerDefenseGame

Spuštění hry

  Spuštění hry na WSL ve windows 11 je potřeba poté do terminálu nahrát tento příkaz: export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0.0
  Poté stačí hru spustit příkazem: ./TowerDefenseGame"
  Pokud spuštění bude mít problém s audiem tak bude pořeba hru spustit příkazem: SDL_AUDIODRIVER=pulseaudio ./TowerDefenseGame 2>&1 | grep "tower_lib.h"
