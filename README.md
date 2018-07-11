
# Installation:

Clone this repo: https://github.com/jtv/libpqxx
into the ./lib/libpqxx folder

then proceed to run these commands on that folder:

    ./configure --disable-documentation
    make
    make install

# Compilation:

run buildit.sh

# Install mongo 
## Enter this folder
    cd lib/
        git clone https://github.com/mongodb/mongo-cxx-driver.git     --branch releases/stable --depth 1
        cd mongo-cxx-driver/build
            cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
    If fail run clone this repo in lib/
        wget https://github.com/mongodb/libbson/releases/download/1.3.5/libbson-1.3.5.tar.gz
        tar -xzf libbson-1.3.5.tar.gz
        cd libbson-1.3.5/
        ./configure --prefix=/usr --libdir=/usr/lib64
        make
        sudo make install
    cd mongo-cxx-driver/build
        make EP_mnmlstc_core
        make
        sudo   make install
    c++ --std=c++11 mongo.cpp -o test $(pkg-config --cflags --libs libmongocxx) -Wl,-rpath,/usr/local/lib
    ./test 

