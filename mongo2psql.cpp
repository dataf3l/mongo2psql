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
#include<iostream>
using namespace std;
using namespace pqxx;

int main(){
    string sql;
    cout << "it works\n";
    try {
        connection C("dbname = ats user = postgres password = postgres hostaddr = 127.0.0.1 port = 5432");
        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }

        try {

            /* Create SQL statement */
            sql = "INSERT INTO messages2 (email) VALUES('10') ";

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
