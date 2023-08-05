//
// Created by antho on 8/3/2023.
//

#ifndef DSA_P3_CSV_FILE_H
#define DSA_P3_CSV_FILE_H

#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

#include "DataRow.h"


class CSV_File
{
std::vector<DataRow> data;
std::unordered_map<std::string, unsigned int> headers;

public:
    CSV_File(std::ifstream f);
    [[nodiscard]] std::vector<double> column_as_double(unsigned int col) const;
    [[nodiscard]] std::vector<long long> column_as_int(unsigned int col) const;
    [[nodiscard]] std::vector<std::string> column_as_string(unsigned int col) const;
    [[nodiscard]] std::vector<double> column_as_double(const std::string& col) const;
    [[nodiscard]] std::vector<long long> column_as_int(const std::string& col) const;
    [[nodiscard]] std::vector<std::string> column_as_string(const std::string& col) const;
};


#endif //DSA_P3_CSV_FILE_H
