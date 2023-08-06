//
// Created by antho on 8/3/2023.
//

#include <iostream>

#include "CSV_File.h"


CSV_File::CSV_File(std::ifstream f)
{
    std::string line;
    std::getline(f, line);

    auto header_row = DataRow(line);

    // indexes the headers with a number
    for (int i = 0; i < header_row.size(); ++i)
    {
        this->headers[header_row.get_column(i).as_string()] = i;
    }

    while (std::getline(f, line))
        this->data.emplace_back(line);

    f.close();
}

/*
 * Does what it says
 */
std::vector<double> CSV_File::column_as_double(unsigned int col) const
{
    std::vector<double> ret;

    for (auto& row : this->data)
    {
        try
        {
            ret.emplace_back(row.get_column(col).as_double());
        }
        catch (const std::out_of_range& e)
        {
            std::cerr <<
                "CSV:File::column_as_double failed with col=" +
                std::to_string(col) << std::endl;
            return {};
        }
    }

    return ret;
}

/*
 * Does what it says
 */
std::vector<long long> CSV_File::column_as_int(unsigned int col) const
{
    std::vector<long long> ret;

    for (auto& row : this->data)
    {
        try
        {
            ret.emplace_back(row.get_column(col).as_int());
        }
        catch (const std::out_of_range& e)
        {
            std::cerr <<
                      "CSV:File::column_as_double failed with col=" +
                      std::to_string(col) << std::endl;
            return {};
        }
    }

    return ret;
}

/*
 * Does what it says
 */
std::vector<std::string> CSV_File::column_as_string(unsigned int col) const
{
    std::vector<std::string> ret;

    for (auto& row : this->data)
    {
        try
        {
            ret.emplace_back(row.get_column(col).as_string());
        }
        catch (const std::out_of_range& e)
        {
            std::cerr <<
                      "CSV:File::column_as_double failed with col=" +
                      std::to_string(col) << std::endl;
            return {};
        }
    }

    return ret;
}

/*
 * Does what it says
 */
std::vector<double> CSV_File::column_as_double(const std::string& col) const
{
    try
    {
        unsigned int col_index = this->headers.at(col);

        return this->column_as_double(col_index);
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "CSV_File::column_as_double failed because it does not have column " + col << std::endl;
        return {};
    }
}

/*
 * Does what it says
 */
std::vector<long long> CSV_File::column_as_int(const std::string& col) const
{
    try
    {
        unsigned int col_index = this->headers.at(col);

        return this->column_as_int(col_index);
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "CSV_File::column_as_double failed because it does not have column " + col << std::endl;
        return {};
    }
}

/*
 * Does what it says
 */
std::vector<std::string> CSV_File::column_as_string(const std::string& col) const
{
    try
    {
        unsigned int col_index = this->headers.at(col);

        return this->column_as_string(col_index);
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << "CSV_File::column_as_double failed because it does not have column " + col << std::endl;
        return {};
    }
}
