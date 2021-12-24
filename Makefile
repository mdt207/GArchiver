
#CC = g++
CC = /usr/bin/g++
garchiver: main.cpp
	$(CC) -std=c++17 `pkg-config gtk+-3.0 --cflags` main.cpp ctasnif.cpp utils.cpp -O2 -o garchiver `pkg-config gtk+-3.0 --libs` `pkg-config libpqxx --libs` `pkg-config libpq --libs` `pkg-config jsoncpp --libs` `pkg-config epoxy --libs` -lpthread

clean: 
	rm -f *.o garchiver
