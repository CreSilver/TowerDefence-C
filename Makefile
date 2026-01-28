CC = gcc

# Jméno výstupního souboru
TARGET = TowerDefenseGame

# Zdrojové soubory a hlavičky
SRC = $(wildcard *.c)
HEADERS = $(wildcard *.h)

# Knihovny (-l říká linkeru, jaké knihovny použít)
LDFLAGS = -L/usr/lib64 
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

CFLAGS = -g -Wall -Wextra -pedantic
ifneq (,$(filter asan,$(MAKECMDGOALS)))
	CFLAGS += -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=address -fsanitize=undefined
endif
$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

asan: clean $(TARGET)
	@echo "-------------------------------------------------------"
	@echo "Zkompilovano s AddressSanitizerem."
	@echo "POZOR: Nespoustej tuto verzi pod Valgrindem!"
	@echo "-------------------------------------------------------"

clean:
	rm -f $(TARGET)
	@echo "Clean."