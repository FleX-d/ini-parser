/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   INIParser.h
 * Author: Adrian Peniak
 *
 * Created on September 30, 2017, 21:07 PM
 */

#ifndef INIPARSER_H
#define INIPARSER_H

#include <map>
#include <string>
#include <functional>

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace ini
{
    class INIParser 
    {
    public:
        /**
         * Singleton...
         * @return instance of INIParser.
         */
        static INIParser& getInstance();
        ~INIParser() = default;
        /**
         * Parse INI file.
         * @param head is file path.
         * @return false when some error was handled, true otherwise.
         */
        template<typename H>
        bool parseFiles(const H& head)
        {
            return parseFile(head);
        }
        /**
         * Parse list of INI files.
         * @param head is file path.
         * @param tail is list of file paths.
         * @return false when some error was handled, true otherwise.
         */
        template<typename H, typename... T>
        bool parseFiles(const H& head, const T&... tail)
        {
            bool ret = parseFile(head);
            if(!parseFiles(tail...)) ret = false;
            return ret;
        }
        /**
         * Get value type T according specific key.
         * Supported types are std::string, int, unsigend int, size_t, bool.
         * @param key is specific key with format SECTION:KEY
         * @param val type T is reference to required type of VALUE 
         *        where will result will be stored.
         * @return true if key was found and return VALUE was converted 
         *         successfully to required type, false otherwise.
         */
        template<class T>
        bool getValue(const std::string& key, T& val) const
        {
            return false;
        }
        /**
         * Get value type T according specific key.
         * Supported types are std::string, int, unsigend int, size_t, bool.
         * @param key is specific key with format SECTION:KEY
         * @param def type T is copy of default value.
         * @return VALUE if key was found and return VALUE was converted 
         *         successfully to required type. Otherwise default VALUE 
         *         will be returned.
         */
        template<class T>
        const T get(const std::string& key, T def) const
        {
            std::ignore = getValue(key, def);
            return def;
        }        
        /**
         * Print content of INIParset container to cout.
         */
        void dump() const;
        
        INIParser(const INIParser&) = delete;
        INIParser& operator=(const INIParser&) = delete;
        
    private:
        INIParser() = default;
        bool parseFile(const std::string& filePath);
        bool tryConverStingFromMap(const std::string& key, std::function<void(const std::string& str)> fcn) const;
        
    private:
        std::map<std::string,std::string> m_map;
    };
} // namespace ini

namespace ini
{
    template<>
    bool INIParser::getValue<bool>(const std::string& key, bool& val) const 
    {
        auto search = m_map.find(key);
        if(search != m_map.cend())
        {
            std::string tmp = search->second;
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
            if(tmp == "true")
            {
                val = true;
                return true;
            }
            else if(tmp == "false")
            {
                val = false;
                return true;
            }
            else
            {
                return tryConverStingFromMap(key, [&val](const std::string& str){ val = std::stoi(str); });
            }
        }
        return false;
    }
    
    template<>
    bool INIParser::getValue<int>(const std::string& key, int& val) const 
    {
        return tryConverStingFromMap(key, [&val](const std::string& str){ val = std::stoi(str); });
    }
    
    template<>
    bool INIParser::getValue<unsigned int>(const std::string& key, unsigned int& val) const 
    {
        return tryConverStingFromMap(key, [&val](const std::string& str){ val = std::stoul(str); });
    }
   
    template<>
    bool INIParser::getValue<std::string>(const std::string& key, std::string& val) const 
    {
        auto search = m_map.find(key);
        if(search != m_map.cend())
        {
            val = search->second;
            return true;
        }
        return false;
    }
} // namespace ini
#endif /* INIPARSER_H */

