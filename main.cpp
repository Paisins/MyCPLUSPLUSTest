#include <iostream>
#include <json/json.h>

int main()
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