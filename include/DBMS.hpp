#pragma once
#include <string>
#include <unordered_map>
#include "Table.hpp"
#include "Parser.hpp"

class DBMS
{
public:
    void runREPL();
    bool deleteTableFile(const std::string &tableName);

private:
    std::unordered_map<std::string, Table> tables;
    void executeQuery(const std::string &query);
    void loadTables();
};
