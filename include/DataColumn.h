//
// Created by antho on 8/3/2023.
//

#ifndef DSA_P3_DATACOLUMN_H
#define DSA_P3_DATACOLUMN_H

#include <string>


class DataColumn
{
std::string data;
public:
    explicit DataColumn(std::string  d);

    [[nodiscard]] double as_double() const;
    [[nodiscard]] long long as_int() const;
    [[nodiscard]] const std::string& as_string() const;
};


#endif //DSA_P3_DATACOLUMN_H
