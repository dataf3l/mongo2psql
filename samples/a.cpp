#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;
int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    bsoncxx::builder::stream::document document{};

    auto collection = conn["messages2"]["messages2"];
    //document << "hello" << "world";

    //collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
        //std::cout << bsoncxx::to_json(doc) << std::endl;
        cout << doc["first_name"].get_utf8().value.to_string() << std::endl;
        cout << doc["last_name"].get_utf8().value.to_string() << std::endl;
        cout << doc["message_id"].get_utf8().value.to_string() << std::endl;
        cout << doc["emails"][0].get_utf8().value.to_string() << std::endl;
        cout << doc["phone_numbers"][0].get_utf8().value.to_string() << std::endl;
        cout << doc["first_name"].get_utf8().value.to_string() << std::endl;
        cout << "***************" << std::endl;
    }
}

