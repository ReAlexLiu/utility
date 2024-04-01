/*
 * @Descripttion:
 * @Author: lucky
 * @Date: 2023/4/7 10:07
 * @LastEditors: lucky
 * @LastEditTime: 2023/4/7 10:07
 */

#ifndef CONFIGS_H
#define CONFIGS_H

#include "utility.hpp"

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>

namespace utility
{
class config
{
    SINGLE_TPL(config);
    void create()
    {
        filename_ = utility::basepath() + "/../config/logs.conf";
        read(filename_);
    };
    void destory(){};

public:
    void read(const std::string& filename)
    {
        filename_ = filename;
        try
        {
            boost::property_tree::info_parser::read_info(filename_, ptree_);
        }
        catch (...)
        {
            std::cerr << "rade config failed! path: " << filename_ << std::endl;
        }
    }

    void write(const std::string& filename = "")
    {
        try
        {
            filename_ = filename.empty() ? filename_ : filename;

            boost::property_tree::info_parser::write_info(filename_, ptree_);
        }
        catch (...)
        {
            std::cerr << "write config failed! path: " << filename_ << std::endl;
        }
    }

    template <class Type>
    Type get(const std::string& path, const Type& default_value)
    {
        return ptree_.get<Type>(path, default_value);
    }

    template <class Type>
    void add(const std::string& path, const Type& value)
    {
        ptree_.add(path, value);
    }

    template <class Type>
    void put(const std::string& path, const Type& value)
    {
        ptree_.put(path, value);
    }

private:
    boost::property_tree::ptree ptree_;
    std::string                 filename_;
};
}

#endif  // CONFIGS_H
