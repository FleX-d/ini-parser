/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   INIParser.cpp
 * Author: Adrian Peniak
 *
 * Created on September 30, 2017, 21:07 PM
 */

#include "INIParser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace
{
    template<class T>
    std::string::iterator findChar(std::string& str, const T& variations, const size_t offset = 0)
    {
        char tmp = '\0';
        auto fcn = [&tmp, &variations](const char ch)->bool
        {
            for(const auto& i : variations)
            {
                if((ch == i)&&(tmp != '\\')) return true;
            }
            tmp = ch;
            return false;
        };
        return std::find_if(str.begin()+offset, str.end(), fcn);
    }

    template<class T>
    void transformReservedChars(std::string& str, const T& reservedChars, const size_t offset = 0)
    {
        std::string tmp = str;
        std::string::iterator tmpIt = tmp.begin();
        auto fcn = [&tmpIt, &reservedChars](const char ch)->bool
        {
            ++tmpIt;
            for(const auto& i : reservedChars)
            {
                if((ch == '\\')&&(*(tmpIt) == i)) return true;
            }
            return false;
        };
        str.erase(std::remove_if(str.begin(), str.end(), fcn), str.end());
    }

    void trimStr(std::string& str)
    {
        auto begin = str.find_first_not_of(' ');
        if(begin != str.npos) str = str.substr(begin, str.find_last_not_of(' ')-begin+1);
        else str = "\0";
    }

    void replaceString(const std::string& src, std::string& des, size_t start, size_t end)
    {
        des.clear();
        des.assign(src, start, end);
        trimStr(des);
    }

    class INIFile
    {
    public:
        INIFile(const std::string& filePath)
        : m_line(),
          m_section(),
          m_key(),
          m_value(),
          m_filePath(filePath),
          m_comments(";#"),
          m_separators(":="),
          m_map()
        {}
        ~INIFile() = default;

        const std::map<std::string, std::string>& getMap()
        {
            return m_map;
        }

        bool parseFile()
        {
            bool ret = true;
            std::ifstream file;
            file.open(m_filePath.c_str());
            if(!file)
            {
                std::cerr << "[error]...File " << m_filePath << " couldn't be found!\n";
                ret = false;
            }

            size_t lineNo = 0;
            while (std::getline(file, m_line))
            {
                lineNo++;
                if(m_line.empty()) continue;
                if(onlyWhitespace()) continue;
                removeComment();
                if(!parseLine())
                {
                    ret = false;
                    std::cerr << "[error]...Parse line #" << lineNo << " \"" << m_line << "\""
                              << " in file: " << m_filePath << "\n";
                }
            }

            file.close();
            return ret;
        }

    private:
        bool onlyWhitespace() const
        {
            return (m_line.find_first_not_of(' ') == m_line.npos);
        }

        void removeComment()
        {
            auto it = findChar(m_line, m_comments);
            if( it != m_line.cend())
            {
                m_line.erase(it, m_line.end());
            }
        }

        bool parseLine()
        {
            if((m_line.size() >= 3)&&(findChar(m_line, m_comments) != m_line.cbegin()))
            {
                if(m_line.front() == '[') // Find section
                {
                    if(!findSection()) return false;
                }
                else
                {
                    if(!tryFindKeyValuePair()) // Find key=value pair in line
                    {
                        if(tryAppendValue()) // Continue multi-line
                        {
                            if(!m_section.empty() && !m_key.empty()) m_map[m_section + ":" + m_key] = m_value;
                            else return false;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if(!m_section.empty() && !m_key.empty())
                        {
                            std::string key = m_section + ":" + m_key;
                            auto search = m_map.find(key); // check for duplicit key in file
                            if(search != m_map.cend())
                            {
                                std::cerr << "[error]...Find duplicit key (" << key << ")\n";
                                return false;
                            }
                            m_map.insert(std::make_pair(key,m_value));
                        }
                    }
                }
                return true;
            }
            return false;
        }

        bool findSection()
        {
            auto end = m_line.find_last_of(']');
            if(end != m_line.npos)
            {
                if(m_line.find_first_not_of(' ', end+1) == m_line.npos)
                {
                    m_section.clear();
                    m_section.assign(m_line.c_str(), 1, end-1);
                    m_key.clear(); // reset key
                    return true;
                }
            }
            return false;
        }

        bool tryFindKeyValuePair()
        {
            if((m_line.front() != '=')||(m_line.front() != ':'))
            {
                auto it = findChar(m_line, m_separators);
                if(it != m_line.cend())
                {
                    size_t pos = std::distance(m_line.begin(), it);
                    if(!validLine(pos+2)) return false;
                    replaceString(m_line, m_key, 0, pos);
                    replaceString(m_line, m_value, pos+1, m_line.npos);
                    transformReservedChars(m_value, std::string(m_separators + m_comments));
                    return true;
                }
            }
            return false;
        }

        bool validLine(size_t offset = 0)
        {
            return (findChar(m_line, m_separators, offset) == m_line.cend());
        }

        bool tryAppendValue()
        {
            if(!validLine()) return false;
            if(onlyWhitespace()) return true;
            trimStr(m_line);
            transformReservedChars(m_line, std::string(m_separators + m_comments));
            m_value+= "\n" + m_line;
            return true;
        }

    private:
        std::string m_line;
        std::string m_section;
        std::string m_key;
        std::string m_value;
        const std::string m_filePath;
        const std::string m_comments;
        const std::string m_separators;
        std::map<std::string, std::string> m_map;
    };
}

namespace ini
{
    INIParser& INIParser::getInstance()
    {
        static INIParser instance;
        return instance;
    }

    bool INIParser::parseFile(const std::string& filePath)
    {
        INIFile ini(filePath);
        if(ini.parseFile())
        {
            for(const auto& it : ini.getMap())
            {
                m_map[it.first] = it.second;
            }
            return true;
        }
        else
        {
            std::cerr << "[error]...File: " << filePath << " was skipped please see previous errors\n";
        }
        return false;
    }

    void INIParser::dump() const
    {
        for(const auto& it : m_map)
        {
            std::cout << "[dump]..." << it.first << " = " << it.second << "\n";
        }
    }

} // namespace ini


