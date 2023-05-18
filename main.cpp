#include <iostream>
#include <sqlite3.h>
#include <json/json.h>

int use_jsoncpp()
{
    std::string input_str = R"({"name": "John", "age": 30, "city": "New York"})";

    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();
    Json::Value root;
    std::string errors;

    bool parsingSuccessful = reader->parse(input_str.c_str(), input_str.c_str() + input_str.size(), &root, &errors);
    delete reader;

    if (!parsingSuccessful)
    {
        std::cout << "Failed to parse the input string: " << errors << std::endl;
        return 1;
    }

    std::cout << "Parsed JSON: " << root << std::endl;

    return 0;
}

int use_sqlite3()
{

    sqlite3 *db;
    int rc = sqlite3_open(":memory:", &db);

    if (rc != SQLITE_OK)
    {
        std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    const char *createTableQuery = "CREATE TABLE Test (ID INT, NAME TEXT);";
    rc = sqlite3_exec(db, createTableQuery, nullptr, nullptr, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "Cannot create table: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    const char *insertDataQuery = "INSERT INTO Test (ID, NAME) VALUES (1, 'John');";
    rc = sqlite3_exec(db, insertDataQuery, nullptr, nullptr, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "Cannot insert data: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    const char *selectDataQuery = "SELECT * FROM Test;";
    rc = sqlite3_exec(
        db, selectDataQuery, [](void *data, int argc, char **argv, char ** /*colNames*/) -> int
        {
        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << " ";
        }
        std::cout << std::endl;
        return 0; },
        nullptr, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cout << "Cannot select data: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    sqlite3_close(db);
    return 0;
}

int main()
{
    use_jsoncpp();
    use_sqlite3();
    return 0;
}