#include "Table.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <algorithm>

Table::Table(const std::vector<std::string>& cols, const std::string& tableName)
    : columns(cols), name(tableName) {
    loadFromFile();
}

void Table::insert(const std::vector<std::string>& row) {
    if (row.size() == columns.size()) {
        data.push_back(row);
        saveToFile();
    }
}

void Table::select(const std::string& whereCol, const std::string& whereVal) const {
    std::vector<size_t> colWidths(columns.size());
    for (size_t i = 0; i < columns.size(); ++i)
        colWidths[i] = columns[i].size();

    std::vector<std::vector<std::string>> filtered;

    int whereIdx = -1;
    if (!whereCol.empty()) {
        for (size_t i = 0; i < columns.size(); ++i) {
            if (columns[i] == whereCol) {
                whereIdx = i;
                break;
            }
        }
    }

    for (const auto& row : data) {
        if (whereIdx == -1 || (whereIdx < row.size() && row[whereIdx] == whereVal)) {
            filtered.push_back(row);
            for (size_t i = 0; i < row.size(); ++i)
                colWidths[i] = std::max(colWidths[i], row[i].size());
        }
    }

    auto printSep = [&]() {
        std::cout << "+";
        for (auto w : colWidths) std::cout << std::string(w + 2, '-') << "+";
        std::cout << "\n";
    };

    auto printRow = [&](const std::vector<std::string>& row) {
        std::cout << "|";
        for (size_t i = 0; i < colWidths.size(); ++i) {
            std::cout << " " << std::setw(colWidths[i]) << std::left << (i < row.size() ? row[i] : "") << " |";
        }
        std::cout << "\n";
    };

    printSep();
    printRow(columns);
    printSep();
    for (const auto& row : filtered) printRow(row);
    printSep();
}

void Table::update(const std::string& whereCol, const std::string& whereVal,
                   const std::string& updateCol, const std::string& updateVal) {
    int whereIdx = -1, updateIdx = -1;
    for (size_t i = 0; i < columns.size(); ++i) {
        if (columns[i] == whereCol) whereIdx = i;
        if (columns[i] == updateCol) updateIdx = i;
    }

    if (whereIdx == -1 || updateIdx == -1) {
        std::cout << "Update failed: invalid column\n";
        return;
    }

    for (auto& row : data) {
        if (whereIdx < row.size() && row[whereIdx] == whereVal) {
            if (updateIdx >= row.size()) row.resize(updateIdx + 1);
            row[updateIdx] = updateVal;
        }
    }
}

void Table::remove(const std::string& whereCol, const std::string& whereVal) {
    int whereIdx = -1;
    for (size_t i = 0; i < columns.size(); ++i)
        if (columns[i] == whereCol) whereIdx = i;

    if (whereIdx == -1) {
        std::cout << "Delete failed: invalid column\n";
        return;
    }

    data.erase(std::remove_if(data.begin(), data.end(), [&](const auto& row) {
        return whereIdx < row.size() && row[whereIdx] == whereVal;
    }), data.end());
}


void Table::saveToFile() const {
    std::filesystem::create_directory("data");
    std::ofstream file("data/" + name + ".db");
    for (size_t i = 0; i < columns.size(); ++i) {
        file << columns[i];
        if (i != columns.size() - 1) file << ",";
    }
    file << "\n";

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i != row.size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
}

void Table::loadFromFile() {
    std::ifstream file("data/" + name + ".db");
    if (!file.is_open()) return;

    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        if (firstLine) {
            if (columns.empty()) columns = row;
            firstLine = false;
        } else {
            data.push_back(row);
        }
    }

    file.close();
}
