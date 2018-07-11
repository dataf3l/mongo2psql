#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

int main(int, char**) {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{}};

    //bsoncxx::builder::stream::document document{};

    auto collection = conn["messages2"]["messages2"];
    //document << "hello" << "world";

    //collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
        //std::cout << bsoncxx::to_json(doc) << std::endl;
        //bsoncxx::document::view view = doc.view();
        //auto firstName = view["first_name"];
        /*if(firstName.type() != bsoncxx::type::k_utf8) {
          // Error
            std::cout << "First name was not a string";
            continue;
        }*/
        //std::string name = firstName.get_utf8().value.to_string();
        //std::cout << name;
        auto json = bsoncxx::to_json(doc);
        //std::cout << "\n" << json.key("first_name") << "\n";
        //if (first_name) {
            // this block will only execute if "store" was found in the document
           // std::cout << "Examining inventory at " << to_json(first_name.get_value()) << std::endl;
        //}
        //std::cout << json.find("first_name") << "\n";
        //std::cout << json.empty();
        std::cout << json.type();
    }
}
