//
// Created by antho on 8/3/2023.
//

#include <stdexcept>
#include <limits>
#include <utility>
#include "DataColumn.h"

// ctor
DataColumn::DataColumn(std::string  d) : data(std::move(d)) {}

double DataColumn::as_double() const
{
    try
    {
        return std::stod(this->data);
    }
    catch (const std::invalid_argument& e)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

long long DataColumn::as_int() const
{
    try
    {
        return std::stoi(this->data);
    }
    catch (const std::invalid_argument& e)
    {
        return LONG_LONG_MAX;
    }
}

const std::string& DataColumn::as_string() const
{
    return this->data;
}

DataColumn::operator double() const
{
    return this->as_double();
}

DataColumn::operator long long() const
{
    return this->as_int();
}

DataColumn::operator std::string() const
{
    return this->as_string();
}
