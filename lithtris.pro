TEMPLATE=app
HEADERS+=include/Game.h \
         include/Block.h \
         include/Square.h \
         include/defines.h \
         include/enums.h 

SOURCES+=src/block.cpp \
         src/game_collisions.cpp \
         src/game.cpp \
         src/game_inputs.cpp \
         src/game_states.cpp \
         src/square.cpp \
         src/main.cpp 

INCLUDEPATH += include 
LIBS += `sdl-config --libs` -lSDL_ttf
QMAKE_CXXFLAGS_DEBUG =`sdl-config --cflags`
QMAKE_CXXFLAGS_RELEASE =`sdl-config --cflags`
CONFIG=debug
TARGET=lithtris
