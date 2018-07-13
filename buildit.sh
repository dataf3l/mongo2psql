# -static-libgcc -static-libstdc++
# -static -static-libgcc -static-libstdc++

# This program compiles our humble mongo2psql.cpp file,
# Due to mongo using c++11, we need to provide the -std=c++11 
# Flag, the -I./lib/libpqxx/include is used for PostgreSQL Support,
# just as are required the -lpqxx -lpq which are linker flags
# which tell the program we need pq and pqxx in /usr/local/lib
# the program pkg-config is executed and returns a set of flags,
# on OSX These flags are as follows:
# -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi -L/usr/local/lib -lmongocxx -lbsoncxx
# however these flags may be different on your system, you may need
# to import said flags, and the include directories on your machine.

#g++ -std=c++11 -I. -I./lib/libpqxx/include  -o ./mongo2psql  mongo2psql.cpp  -lpqxx -lpq $(pkg-config --cflags --libs libmongocxx) -Wl,-rpath,/usr/local/lib -v
g++ -std=c++11 -I. -I./lib/libpqxx/include  -o ./mongo2psql-static  mongo2psql.cpp  -lpqxx -lpq $(pkg-config --cflags --libs libmongocxx-static) -Wl,-rpath,/usr/local/lib 
upx ./mongo2psql-static




#libpqxx.a
#libmongoc-static-1.0.a
#libbson-static-1.0.a
#libpqxx.a
#libpq.a

#libbsoncxx-static.a
#
# cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local ..


