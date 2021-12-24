
#CC = g++
CC = /usr/bin/g++
inserter-0.0: main.cpp
	$(CC) -std=c++17 `pkg-config gtk+-3.0 --cflags` main.cpp ctasnif.cpp utils.cpp -O2 -o inserter3 `pkg-config gtk+-3.0 --libs` `pkg-config libpqxx --libs` `pkg-config libpq --libs` `pkg-config jsoncpp --libs` `pkg-config epoxy --libs` -lpthread

clean: 
	rm -f *.o inserter-0.0
