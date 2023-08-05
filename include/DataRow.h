//
// Created by antho on 8/3/2023.
//

#ifndef DSA_P3_DATAROW_H
#define DSA_P3_DATAROW_H

#include <vector>
#include <string>

#include "DataColumn.h"

class DataRow
{
std::vector<std::string> data;
public:
    explicit DataRow(const std::string& row);
    DataRow() = default;

    [[nodiscard]] DataColumn get_column(unsigned int col) const;
    [[nodiscard]] DataColumn operator [] (unsigned int col) const;
    [[nodiscard]] std::size_t size() const;
};


#endif //DSA_P3_DATAROW_H
