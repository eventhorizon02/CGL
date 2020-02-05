CXX = gcc

CXXFLAGS = -Wall -std=c99
LDFLAGS = -framework SDL2 -framework SDL2_ttf
OBJECTS = main.o utils.o stateMachine.o introScene.o inputCtrl.o gameScene.o 

all:CGL

CGL:$(OBJECTS) def.h
	$(CXX) -g $(OBJECTS) -o CGL $(LDFLAGS)

utils.o:utils.c utils.h
	$(CXX) $(CXXFLAGS) -c utils.c -o utils.o

main.o:main.c
	$(CXX) $(CXXFLAGS) -c main.c -o main.o

stateMachine.o:stateMachine.c stateMachine.h
	$(CXX) $(CXXFLAGS) -c stateMachine.c -o stateMachine.o

introScene.o:introScene.c introScene.h
	$(CXX) $(CXXFLAGS) -c introScene.c -o introScene.o

inputCtrl.o:inputCtrl.c inputCtrl.h
	$(CXX) $(CXXFLAGS) -c inputCtrl.c -o inputCtrl.o

gameScene.o:gameScene.c gameScene.h
	$(CXX) $(CXXFLAGS) -c gameScene.c -o gameScene.o

clean:
	rm *.o







