/**
 * This program copies data from mongodb to postgresql
 * 1. read environment variables for db connection
 * 2. read environment for mongo url connection
 * 3. read environment for configuration (table)
 * 4. connect to mongodb
 * 5. connect to psql
 * 6. read from mongo
 * 7. write to postgres
 * 8. disconnect
 * 9. disconnect
 * 10. write report to disk
 * */

#include <pqxx/pqxx>
#include <iostream>
#include <cstdlib>
#include "getenv2.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;
using namespace pqxx;

/** v["a"] */
string mongo_get_field(bsoncxx::v_noabi::document::view view, const char* field_name){
    try {
        int x = view[field_name].length();// == bsoncxx::type::k_utf8);
        if(x == 0){
            // Element does not exist.
            return string("");
        }
        //cout << x<<"\n";
        return view[field_name].get_utf8().value.to_string();
    } catch (const std::exception &e) {
        cerr << "MONGO FIELD MISSING ERROR:"<<e.what() << " MISSING FIELD:"<<field_name << std::endl;
        return string("");
    }
    
}
/* v["a"][0] */
string mongo_get_array_item(bsoncxx::v_noabi::document::view view, const char* field_name, int item_id){
    try {
        return view[field_name][item_id].get_utf8().value.to_string();
    } catch (const std::exception &e) {
        cerr << "MONGO ARRAY ELEMENT MISSING ERROR:"<<e.what() << " MISSING FIELD:"<<field_name << std::endl;
        return string("");
    }
}

int main(){
    string sql;
    char const* pgpass = getenv("PGPASS");
    if (pgpass == NULL){
        char const* default_pass = "postgres";
        pgpass=default_pass;
    }
    string PGPASS  = getenv2("PGPASS", "postgres");
    string PGUSER  = getenv2("PGUSER", "postgres");
    string PGBASE  = getenv2("PGBASE", "ats");
    string PGTABLE = getenv2("PGTABLE","lead_source");
    string PGHOST  = getenv2("PGHOST", "127.0.0.1");
    string PGPORT  = getenv2("PGPORT", "5432");

    try {
        connection C("dbname = ats user = postgres password = postgres hostaddr = 127.0.0.1 port = 5432");
        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }

        try {

            // MongoDB Connection and Query:
            mongocxx::instance inst{};
            mongocxx::client conn{mongocxx::uri{}};

            bsoncxx::builder::stream::document document{};

            auto collection = conn["messages2"]["messages2"];
            //document << "hello" << "world";

            //collection.insert_one(document.view());
            auto cursor = collection.find({});
            string sql("");
            for (auto&& doc : cursor) {
                //std::cout << bsoncxx::to_json(doc) << std::endl;
                //string _id = mongo_get_field(doc,"_id");
                string _id("");
                try {
                    string first_name = mongo_get_field(doc,"first_name");
                    string last_name = mongo_get_field(doc,"last_name");
                    string source = mongo_get_field(doc,"source");
                    string apply_date = mongo_get_field(doc,"apply_date");
                    string submitted_to_vipkid = mongo_get_field(doc,"submitted_to_vipkid");
                    string email = mongo_get_array_item(doc,"emails",0);
                    string phone_number = mongo_get_array_item(doc,"phone_numbers",0);
                    string message_id = mongo_get_field(doc,"message_id");
                    //string replace = std::replace( first_name.begin(), first_name.end(), 's', 's');
                    using std::string;
                    first_name.erase(replace(first_name.begin(), first_name.end(), '\'', ''), first_name.end());
                    cout << first_name;
                    //cout << first_name <<"\t\t"<<last_name<<"\t\t"<<email<<"\t\t"<<phone_number<<"\t\t"<<message_id<< std::endl;
                    string sql("INSERT INTO lead_source (_id,first_name,last_name,email,phone_number,source,apply_date,message_id,submitted_to_vipkid) ");
                    sql += "VALUES('"+_id+"','"+first_name + "','"  + last_name+"','"+email+"','"+phone_number+"','"+source+"','"+apply_date+"','"+message_id+"','"+submitted_to_vipkid+"')";
                    cout << sql<<endl;

                } catch (const std::exception &e) {
                    cerr << "RECORD ERROR:"<<e.what() << std::endl;
                    return 1;
                }
                
            }


            /* Create SQL statement */
            sql = "INSERT INTO lead_source (_id,first_name,last_name,email,phone_number,source,apply_date,message_id,submitted_to_vipkid) VALUES('1','1','1','1','1','1','2018-01-01','1','0') ";

            /* Create a transactional object. */
            work W(C);

            /* Execute SQL query */
            W.exec( sql );
            W.commit();
            cout << "Records created successfully" << endl;
        } catch (const std::exception &e) {
            cerr << "PSQLERROR:"<<e.what() << std::endl;
            return 1;
        }


        C.disconnect ();
    } catch (const std::exception &e) {
        cerr << "PSQLCONNERROR:"<<e.what() << std::endl;
        return 1;
    }

}
