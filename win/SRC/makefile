
OBJS = gameScene.c inputCtrl.c introScene.c main.c stateMachine.c utils.c
CC = gcc
INCLUDE_PATHS = -IC:\mingw_dev_lib\include\SDL2

LIBRARY_PATHS = -LC:\mingw_dev_lib\lib

COMPILER_FLAGS = -w -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

OBJ_NAME = CGL


all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)




