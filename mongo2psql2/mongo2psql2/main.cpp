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
//#include "getenv2.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;
using namespace pqxx;


std::string getenv2(string envkey, string envdefault){
    char const* the_value= getenv(envkey.c_str());
    if (the_value == NULL){
        cout << "ENV:"<<envkey<<" MISSING \n";
        return envdefault;
    }
    string s_the_value(the_value);
    cout << "DEBUG:ENV:"<<envkey<<" = "<< the_value <<"\n";
    return the_value;
}



string mongo_get_field_with_default(bsoncxx::v_noabi::document::view view, const char* field_name, string default_value){
    try {
        if (!view[field_name]){
            return default_value;
        }
        //cout << x<<"\n";
        return view[field_name].get_utf8().value.to_string();
    } catch (const std::exception &e) {
        cerr << "MONGO FIELD MISSING ERROR:"<<e.what() << " MISSING FIELD:"<<field_name << std::endl;
        return string("");
    }
    
}
/** v["a"] */

string mongo_get_field(bsoncxx::v_noabi::document::view view, const char* field_name){
    try {
        if (!view[field_name]){
            cerr << "MONGO FIELD MISSING MISSING FIELD:"<<field_name << std::endl;
            return string("");
        }
        //cout << x<<"\n";
        return view[field_name].get_utf8().value.to_string();
    } catch (const std::exception &e) {
        cerr << "MONGO FIELD MISSING ERROR:"<<e.what() << " MISSING FIELD:"<<field_name << std::endl;
        return string("");
    }
    
}
string mongo_get_date_field(bsoncxx::v_noabi::document::view view, const char* field_name){
    try {
        if (!view[field_name]){
            cerr << "MONGO FIELD MISSING MISSING FIELD:"<<field_name << std::endl;
            return string("");
        }
        std::chrono::duration<long int, std::ratio<1l, 1000l>> duration = view[field_name].get_date().value;
        
        auto time1 =  std::chrono::duration_cast<std::chrono::seconds>(duration);
        struct tm * timeinfo;
        
        int64_t duration_count = time1.count();
        std::time_t tt = duration_count;
        timeinfo = localtime (&tt);
        std::array<char, 64> timebuf;
        const char* timeFormat = "%F %T";
        std::strftime(timebuf.data(), timebuf.size(), timeFormat, timeinfo);
        std::string formatted_string(std::begin(timebuf), std::end(timebuf));
        return formatted_string;
    } catch (const std::exception &e) {
        cerr << "MONGO DATE FIELD MISSING ERROR:"<<e.what() << " MISSING FIELD:"<<field_name << std::endl;
        try {
            // warning
            return mongo_get_field(view, field_name);
        } catch (const std::exception &e) {
            cerr << "MONGO DATE FIELD WAS NOT A STRING OR A DATE ERROR:"<<e.what() << " MISSING FIELD:"<<field_name << std::endl;
            return string("");
        }
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
        connection C("dbname = "+PGBASE+" user = "+PGUSER+" password = "+PGPASS+" hostaddr = "+PGHOST+" port = "+PGPORT);
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
            
            for (auto&& doc : cursor) {
                //std::cout << bsoncxx::to_json(doc) << std::endl;
                //string _id = mongo_get_field(doc,"_id");
                try {
                    /* Create a transactional object. */
                    work W(C);
                    bsoncxx::v_noabi::document::element id = doc["_id"];
                    bsoncxx::v_noabi::oid oid = id.get_oid().value;
                    //string _id = obj.OID().toString();
                    string _id = oid.to_string();
                    string first_name = mongo_get_field(doc,"first_name");
                    string last_name = mongo_get_field(doc,"last_name");
                    string source = mongo_get_field(doc,"source");
                    string apply_date = mongo_get_date_field(doc,"apply_date");
                    string submitted_to_vipkid = mongo_get_field_with_default(doc,"submitted_to_vipkid","0");
                    string email = mongo_get_array_item(doc,"emails",0);
                    string phone_number = mongo_get_array_item(doc,"phone_numbers",0);
                    string message_id = mongo_get_field(doc,"message_id");
                    
                    if(apply_date == ""){
                        apply_date = "null";
                    }else {
                        apply_date = "'" + W.esc(apply_date) + "'";
                    }
                    
                    if(submitted_to_vipkid == ""){
                        submitted_to_vipkid = "0";
                    }
                    
                    //auto s = W.esc(first_name);
                    //cout << s << " \t " << first_name << "\n" << W.esc(last_name) << "\t" << last_name << "\n";
                    //cout << first_name << "\n";
                    string sql("INSERT INTO lead_source (_id,first_name,last_name,email,phone_number,source,apply_date,message_id,submitted_to_vipkid) ");
                    sql += "VALUES('"+W.esc(_id)+"','"+W.esc(first_name) + "','"  + W.esc(last_name)+"','"+W.esc(email)+"','"+W.esc(phone_number)+"','"+W.esc(source)+"',"+apply_date+",'"+message_id+"','"+submitted_to_vipkid+"') ON CONFLICT (message_id) DO UPDATE SET  "
                    "_id=EXCLUDED._id,"
                    "first_name=EXCLUDED.first_name,"
                    "last_name=EXCLUDED.last_name,"
                    "email=EXCLUDED.email,"
                    "phone_number=EXCLUDED.phone_number,"
                    "source=EXCLUDED.source,"
                    "apply_date=EXCLUDED.apply_date,"
                    "submitted_to_vipkid=EXCLUDED.submitted_to_vipkid ";

                    
                    
                    //cout << sql <<endl;
                    /* Execute SQL query */
                    W.exec( sql );
                    W.commit();
                } catch (const std::exception &e) {
                    cerr << "RECORD ERROR:"<<e.what() << std::endl;
                    return 1;
                }
                
            }
            
            
            /* Create SQL statement */
            //sql = "INSERT INTO lead_source (_id,first_name,last_name,email,phone_number,source,apply_date,message_id,submitted_to_vipkid) VALUES('1','1','1','1','1','1','2018-01-01','1','0') ";
            
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

//  https://stackoverflow.com/questions/48307724/mongocxx-static-driver-example-fails


