#pragma once
#include <vector>
#include <string>

class Table
{
public:
    Table() = default;
    Table(const std::vector<std::string> &columns, const std::string &name);
    void insert(const std::vector<std::string> &row);
    void select(const std::string &whereCol = "", const std::string &whereVal = "") const;
    void update(const std::string &whereCol, const std::string &whereVal, const std::string &updateCol, const std::string &updateVal);
    void remove(const std::string &whereCol, const std::string &whereVal);

    void saveToFile() const;
    void loadFromFile();

private:
    std::string name;
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> data;
};
