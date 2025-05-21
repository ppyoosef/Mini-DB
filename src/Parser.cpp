#include "Parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

// Utility to trim characters from both ends
std::string trim(const std::string& s) {
    size_t start = 0, end = s.length();
    while (start < end && (s[start] == '(' || s[start] == ',')) ++start;
    while (end > start && (s[end - 1] == ')' || s[end - 1] == ',' || s[end - 1] == ';')) --end;
    return s.substr(start, end - start);
}

ParsedQuery Parser::parse(const std::string& query) {
    ParsedQuery pq;

    std::string cleaned = query;
    while (!cleaned.empty() && std::isspace(cleaned.back())) cleaned.pop_back();

    if (cleaned.back() == ';') cleaned.pop_back(); // remove trailing ;
    std::istringstream ss(cleaned);
    std::string word;
    ss >> word;

     if (word == "CREATE") {
        std::string tmp;
        ss >> tmp; // TABLE
        ss >> pq.tableName;

        if (pq.tableName.empty()) {
            pq.error = "Missing table name in CREATE TABLE statement.";
            return pq;
        }

        std::string cols;
        std::getline(ss, cols, '(');
        std::getline(ss, cols, ')');

        if (cols.empty()) {
            pq.error = "Missing column list in CREATE TABLE statement.";
            return pq;
        }

        std::istringstream colStream(cols);
        std::string col;
        while (colStream >> col) {
            pq.columns.push_back(col);
        }
        pq.type = QueryType::CREATE;

    } else if (word == "INSERT") {
        std::string tmp;
        ss >> tmp; // INTO
        ss >> pq.tableName;

        if (pq.tableName.empty()) {
            pq.error = "Missing table name in INSERT INTO statement.";
            return pq;
        }

        std::string valuesKeyword;
        ss >> valuesKeyword;
        if (valuesKeyword != "VALUES") {
            pq.error = "Expected 'VALUES' keyword in INSERT INTO statement.";
            return pq;
        }

        std::string values;
        std::getline(ss, values, '(');
        std::getline(ss, values, ')');

        if (values.empty()) {
            pq.error = "Missing values in INSERT INTO statement.";
            return pq;
        }

        std::istringstream valStream(values);
        std::string val;
        while (valStream >> val) {
            pq.values.push_back(val);
        }
        pq.type = QueryType::INSERT;

    } else if (word == "SELECT") {
        std::string star;
        ss >> star;
        std::string from;
        ss >> from;
        ss >> pq.tableName;

        if (star != "*" || from != "FROM" || pq.tableName.empty()) {
            pq.error = "Invalid SELECT syntax. Use: SELECT * FROM <table_name>;";
            return pq;
        }

        pq.type = QueryType::SELECT;
    } else if (word == "DROP") {
    std::string tmp;
    ss >> tmp; // TABLE
    ss >> pq.tableName;
    if (pq.tableName.empty()) {
        pq.error = "Missing table name in DROP TABLE.";
        return pq;
    }
    pq.type = QueryType::DROP;
} else if (word == "UPDATE") {
    ss >> pq.tableName;
    std::string tmp;
    ss >> tmp; // SET
    if (tmp != "SET") {
        pq.error = "Expected SET keyword in UPDATE.";
        return pq;
    }

    std::string assignment;
    ss >> assignment;
    size_t eq = assignment.find('=');
    if (eq == std::string::npos) {
        pq.error = "Invalid SET syntax. Use: column=value";
        return pq;
    }

    pq.updateColumn = assignment.substr(0, eq);
    pq.updateValue = assignment.substr(eq + 1);

    ss >> tmp; // WHERE
    if (tmp != "WHERE") {
        pq.error = "Expected WHERE clause in UPDATE.";
        return pq;
    }

    std::string condition;
    ss >> condition;
    size_t eq2 = condition.find('=');
    if (eq2 == std::string::npos) {
        pq.error = "Invalid WHERE condition.";
        return pq;
    }

    pq.whereColumn = condition.substr(0, eq2);
    pq.whereValue = condition.substr(eq2 + 1);
    pq.type = QueryType::UPDATE;
} else if (word == "DELETE") {
    std::string tmp;
    ss >> tmp; // FROM
    ss >> pq.tableName;
    ss >> tmp; // WHERE
    if (tmp != "WHERE") {
        pq.error = "Expected WHERE clause in DELETE.";
        return pq;
    }

    std::string condition;
    ss >> condition;
    size_t eq = condition.find('=');
    if (eq == std::string::npos) {
        pq.error = "Invalid WHERE condition.";
        return pq;
    }

    pq.whereColumn = condition.substr(0, eq);
    pq.whereValue = condition.substr(eq + 1);
    pq.type = QueryType::DELETE;
} else if (word == "SELECT") {
    std::string star;
    ss >> star;
    std::string from;
    ss >> from;
    ss >> pq.tableName;

    if (star != "*" || from != "FROM" || pq.tableName.empty()) {
        pq.error = "Invalid SELECT syntax. Use: SELECT * FROM <table> [WHERE col=val];";
        return pq;
    }

    std::string where;
    ss >> where;
    if (where == "WHERE") {
        std::string cond;
        ss >> cond;
        size_t eq = cond.find('=');
        if (eq == std::string::npos) {
            pq.error = "Invalid WHERE condition in SELECT.";
            return pq;
        }
        pq.whereColumn = cond.substr(0, eq);
        pq.whereValue = cond.substr(eq + 1);
    }

    pq.type = QueryType::SELECT;
} else if (word == "SHOW") {
    std::string secondWord;
    ss >> secondWord;
    if (secondWord == "TABLES") {
        pq.type = QueryType::SHOW_TABLES;
        return pq;
    } else {
        pq.error = "Invalid SHOW command. Did you mean SHOW TABLES?";
        return pq;
    }
} else {
        pq.error = "Unrecognized command. Supported: CREATE, INSERT, SELECT.";
    }

    return pq;
}