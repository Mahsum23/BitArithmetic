#pragma once

#include <iostream>
#include <string>
#include <set>
#include <regex>
#include <filesystem>
#include <functional>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <optional>
#include <cmath>

namespace fs = std::filesystem;
using namespace std::literals;

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
    for (int i = 0; i < v.size(); ++i)
    {
        out << v[i] << " ";
    }
    return out;
}

template <typename F, int ARGNUM>
class Tester
{
public:

    Tester(const std::string& folder, const std::string& function_name, 
           std::function<F> function, std::ostream& out_info = std::cout)
        : folder_(folder), function_name_(function_name), function_(function), out_info_(out_info)
    {

    }

    void Run()
    {
        std::cout << "=======================================================" << std::endl;
        std::cout << function_name_ << std::endl;

        std::set<fs::path, AlphaNumSort> files;
        for (const auto& file : fs::directory_iterator(fs::path(folder_)))
        {
            std::string in_filename = file.path().string();
            if (file.path().filename().string().find(in_ext_) != std::string::npos)
            {
                files.insert(file);
            }
            
        }
        for (const auto& file : files)
        {
            {
                std::ifstream in_file(file.string());
                std::vector<std::string> args;
                std::string in_line;
                while (getline(in_file, in_line))
                {
                    args.push_back(in_line);
                }

                std::string out_filename = folder_ + "\\" + file.stem().string() + out_ext_;
                std::ifstream out_file(out_filename);                
                std::stringstream outss; outss << out_file.rdbuf();
                std::string output(outss.str()); 
                if (output.back() == '\n')
                    output.pop_back();
                std::string result;
                {
                    std::stringstream ss; ss << args;
                    if constexpr (ARGNUM == 0)
                        result = function_();
                    else if constexpr (ARGNUM == 1)
                        result = function_(args[0]);
                    else if constexpr (ARGNUM == 2)
                        result = function_(args[0], args[1]);
                    else
                    {
                        std::cerr << "Unsupported number of arguments" << std::endl;
                    }
                }
                if (result == output)
                {
                    std::cout << file.filename() << ": " << "PASSED" << std::endl;
                    //std::cout << "\tExpected: " << output << "\n\tResult: " << result << std::endl << std::endl;
                }
                else
                {
                    std::cout << file.filename() << ": " << "FAILED" << std::endl;
                    std::cout << "\tINPUT: " << in_line << std::endl;
                    std::cout << '\t' << "SHOULD BE: " << std::endl <<
                        output << "\n\tRESULT: " << std::endl <<
                        result << std::endl << std::endl;
                }
                
            }
        }
        std::cout << "=======================================================\n\n";
    }



private:

    struct AlphaNumSort
    {
        std::optional<int> GetFirstNumber(const std::string& s) const
        {
            auto start = std::find_if(s.begin(), s.end(), [](char c) { return std::isdigit(c); });
            if (start == s.end())
                return std::nullopt;
            auto end = std::find_if(start + 1, s.end(), [](char c) { return std::isdigit(c); });
            return std::stoi(s.substr(start - s.begin(), (end - s.begin()) - (start - s.begin()) + 1));
        }

        bool operator()(const fs::path& p1, const fs::path& p2) const
        {
            std::string s1 = p1.string();
            std::string s2 = p2.string();
            auto num1 = GetFirstNumber(p1.string());
            auto num2 = GetFirstNumber(p2.string());
            if (!num1 || !num2)
                return s1 < s2;
            return num1 < num2;
        }
    };


    std::string in_ext_ = ".in";
    std::string out_ext_ = ".out";
    std::string folder_;
    std::function<F> function_;
    std::string function_name_;
    
    std::ostream& out_info_;
};