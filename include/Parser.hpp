#pragma once
#include <vector>
#include <string>

enum class QueryType {
    CREATE,
    INSERT,
    SELECT,
    DROP,
    UPDATE,
    DELETE,
    INVALID,
    SHOW_TABLES,
};

struct ParsedQuery {
    QueryType type;
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<std::string> values;
    std::string whereColumn;
    std::string whereValue;
    std::string updateColumn;
    std::string updateValue;
    std::string error;

    bool hasError() const { return !error.empty(); }
};

class Parser {
public:
    static ParsedQuery parse(const std::string& query);
};
