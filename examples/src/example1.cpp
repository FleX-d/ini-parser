/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Adrian Peniak
 *
 * Created on September 30, 2017, 21:03 PM
 */

#include "INIParser.h"
#include <iostream>
#include <cassert>

int main(int argc, char** argv)
{
    std::cout << "start\n";
    bool tmpBool = false;
    int tmpInt = 500;
    size_t tmpSize_t = 0;
    std::string tmpString{};
    
    // bad ini files
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_comment.ini") == false);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_comment2.ini") == false);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_comment3.ini") == false);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_line.ini") == false);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_line2.ini") == false);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_line3.ini") == false);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_line4.ini") == false);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_bad_section.ini") == false);
    
    // test overwrite
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_overwrite1.ini") == true);
    assert(ini::INIParser::getInstance().get("overwrite:a", tmpInt) == 10);
    assert(ini::INIParser::getInstance().parseFiles("../resources/test_overwrite2.ini") == true);
    assert(ini::INIParser::getInstance().get("overwrite:a", (int)0) == 5);
    
    // test bool
    assert(ini::INIParser::getInstance().parseFiles("../resources/test.ini") == true); // parse good file
    tmpBool = ini::INIParser::getInstance().get("test:e", false); // get bool from string
    assert(tmpBool == true);
    assert(ini::INIParser::getInstance().get("test:c", tmpBool) == false); // get bool from int
    assert(ini::INIParser::getInstance().get("test:e", tmpBool) == true); // get bool with first uppercase

    // test getValue
    assert(ini::INIParser::getInstance().getValue("test:a", tmpSize_t) == true); // will be found
    assert(tmpSize_t == 5);
    tmpString = "some dummy text";
    assert(ini::INIParser::getInstance().getValue("test:x", tmpString) == false); // will be not found
    assert(tmpString == "some dummy text");
    assert(ini::INIParser::getInstance().getValue("test:d", tmpString) == true); // will be found
    assert(tmpString == "aaa");
    
    // test special cases
    assert(ini::INIParser::getInstance().get("test:h", std::string("")) == "h"); // get single char
    assert(ini::INIParser::getInstance().get("test1:multi", std::string("")) == "lalala\nblabla"); // get multi-line
    assert(ini::INIParser::getInstance().get("test1:reserved-chars", std::string("")) == "a#b;c=d:e"); // get reserved chars value
    assert(ini::INIParser::getInstance().get("test\\=2:reserved\\:char", std::string("")) == "in section and key"); // use reserved char in section and key 
    assert(ini::INIParser::getInstance().get("test1:NULL", std::string("")) == ""); // test NULL
    assert(ini::INIParser::getInstance().get("test1:NOT-NULL", std::string("")) == "\"\""); // test NOT-NULL ""
    
    ini::INIParser::getInstance().dump();
    std::cout << "end\n";
    return 0;
}

