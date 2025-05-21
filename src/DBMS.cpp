#include "DBMS.hpp"
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <fstream>

void DBMS::runREPL() {
    std::string input;
    std::cout << "MiniDBMS v0.1\nType 'exit;' to quit\n\n";
    loadTables();
    while (true) {
        std::cout << ">> ";
        std::getline(std::cin, input);
        if (input == "exit;") break;
        executeQuery(input);
    }
}

void DBMS::loadTables() {
    if (tables.empty()) {
const std::string dbPath = "data/";
    for (const auto& entry : std::filesystem::directory_iterator(dbPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::string tableName = entry.path().stem().string();

            std::ifstream file(entry.path());
            if (!file.is_open()) continue;

            std::string headerLine;
            std::getline(file, headerLine);

            Table table;
            tables[filename] = Table({}, filename);
        }
    }
}
}
bool DBMS::deleteTableFile(const std::string& tableName) {
    std::string filePath = "data/" + tableName + ".db";
    return std::filesystem::remove(filePath); // true if file deleted
}

void DBMS::executeQuery(const std::string& query) {
    ParsedQuery pq = Parser::parse(query);

    if (pq.hasError()) {
        std::cout << "Syntax error: " << pq.error << "\n";
        return;
    }

    if (pq.type == QueryType::CREATE) {
        tables[pq.tableName] = Table(pq.columns, pq.tableName);
        std::cout << "Table " << pq.tableName << " created.\n";
    }
    else if (pq.type == QueryType::INSERT) {
        if (tables.find(pq.tableName) == tables.end())
            tables[pq.tableName] = Table({}, pq.tableName);

        tables[pq.tableName].insert(pq.values);
        std::cout << "1 row inserted.\n";
    }
    else if (pq.type == QueryType::SELECT) {
        if (tables.find(pq.tableName) == tables.end())
            tables[pq.tableName] = Table({}, pq.tableName);

        tables[pq.tableName].select();
    } else if (pq.type == QueryType::DROP) {
        auto it = tables.find(pq.tableName);
        if (it != tables.end()) {
            tables.erase(it);
            if (deleteTableFile(pq.tableName))
                std::cout << "Table dropped and file deleted.\n";
            else
                std::cout << "Table dropped, but file not deleted.\n";
        } else {
            std::cout << "Table not found.\n";
        }
    } else if (pq.type == QueryType::UPDATE) {
        if (tables.find(pq.tableName) != tables.end()) {
            tables[pq.tableName].update(pq.whereColumn, pq.whereValue,
                                        pq.updateColumn, pq.updateValue);
            std::cout << "Table updated.\n";
        } else {
            std::cout << "Table not found.\n";
        }
    } else if (pq.type == QueryType::UPDATE) {
        if (tables.find(pq.tableName) != tables.end()) {
            tables[pq.tableName].update(pq.whereColumn, pq.whereValue,
                                        pq.updateColumn, pq.updateValue);
            std::cout << "Table updated.\n";
        } else {
            std::cout << "Table not found.\n";
        }
    } else if (pq.type == QueryType::DELETE) {
        if (tables.find(pq.tableName) != tables.end()) {
            tables[pq.tableName].remove(pq.whereColumn, pq.whereValue);
            std::cout << "Rows deleted.\n";
        } else {
            std::cout << "Table not found.\n";
        }
    } else if (pq.type == QueryType::SELECT) {
        if (tables.find(pq.tableName) != tables.end()) {
            tables[pq.tableName].select(pq.whereColumn, pq.whereValue);
        } else {
            std::cout << "Table not found.\n";
        }
    } else if (pq.type ==  QueryType::SHOW_TABLES) {
        if (tables.empty()) {
            std::cout << "No tables found.\n";
        } else {
            std::cout << "+----------------+\n";
            std::cout << "| Tables         |\n";
            std::cout << "+----------------+\n";
            for (const auto& [name, _] : tables) {
                std::cout << "| " << std::setw(15) << std::left << name << "|\n";
            }
            std::cout << "+----------------+\n";
        }
    }
}


