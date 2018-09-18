OBJS = main.o menu.o startGame.o engine.o design.o io.o anim.o
CC = g++
DEBUG = -D DEB
ALLEGRO = -lallegro_dialog -lallegro_color -lallegro_audio -lallegro_image -lallegro_physfs -lallegro -lallegro_font -lallegro_acodec -lallegro_main \
-lallegro_memfile -lallegro_primitives -lallegro_ttf `pkg-config --libs allegro-5.0`
CFLAGS = -Wall -c
LFLAGS = -Wall

run.out : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(ALLEGRO) -o run.out

main.o : main.cc headers/objects.h headers/io.h
	$(CC) $(CFLAGS) $(ALLEGRO) main.cc

menu.o : menu.cc headers/objects.h headers/engine.h headers/io.h headers/design.h
	$(CC) $(CFLAGS) $(ALLEGRO) menu.cc

startGame.o : startGame.cc headers/objects.h headers/engine.h headers/io.h headers/design.h
	$(CC) $(CFLAGS) $(ALLEGRO) startGame.cc

anim.o : anim.cc headers/objects.h headers/engine.h
	$(CC) $(CFLAGS) $(ALLEGRO) anim.cc

design.o : headers/design.h design.cc headers/objects.h
	$(CC) $(CFLAGS) $(ALLEGRO) design.cc

engine.o : headers/engine.h engine.cc headers/objects.h headers/design.h
	$(CC) $(CFLAGS) $(ALLEGRO) engine.cc

io.o : headers/io.h io.cc
	$(CC) $(CFLAGS) $(ALLEGRO) io.cc

.PHONY: clean

clean:
	rm -rf *.o
