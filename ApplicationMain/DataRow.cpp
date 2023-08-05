//
// Created by antho on 8/3/2023.
//

#include <stdexcept>

#include "DataRow.h"

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

DataRow::DataRow(const std::string& row)
{
    this->data = split(row, ",");

    for (int i = 0; i < this->data.size(); ++i)
    {
        if (this->data[i].front() == '"' and this->data[i].back() == '"')
            this->data[i] = this->data[i].substr(1, this->data[i].length() - 2);
    }
}

DataColumn DataRow::get_column(unsigned int col) const
{
    if (col < this->data.size())
        return DataColumn(this->data[col]);

    throw std::out_of_range(
            "The row only has " +
            std::to_string(this->data.size()) +
            " rows but get column was called with col=" +
            std::to_string(col));
}


std::size_t DataRow::size() const
{
    return this->data.size();
}


DataColumn DataRow::operator [] (unsigned int col) const
{
    return this->get_column(col);
}


