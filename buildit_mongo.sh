c++ --std=c++11 mongo.cpp -o test $(pkg-config --cflags --libs libmongocxx) -Wl,-rpath,/usr/local/lib
