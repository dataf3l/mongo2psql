# -static-libgcc -static-libstdc++
# -static -static-libgcc -static-libstdc++
g++ -std=c++11 -I. -I./lib/libpqxx/include  -o ./mongo2psql  mongo2psql.cpp -lpqxx -lpq  $(pkg-config --cflags --libs libmongocxx) -Wl,-rpath,/usr/local/lib -static
