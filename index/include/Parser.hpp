// -*- C++ -*-

// $Id: Parser.hpp,v 1.3 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef __parser_hpp__
#define __parser_hpp__

#include <fstream>
#include <string>
#include <envparm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ParseTree.hpp>

#define FEATURES "appo att be cn genetive neg passive pn prog tense sem"
#define FILENAME "temp_"
#define BUF_SIZE 4096

class Parser {
  public:
    Parser();
    ~Parser();

    // parse a new sentence and return a parse tree
    ParseTree* parse_sentence(const char* sent);

    // check if the parser is ready
    bool is_ready() const;

  private:
    // helper functions:

    // parse one line of parse
    void parse_line(ParseTree* tree, const char* line);

     // locate the roots of the sentence and put them in container
    void normalize_tree(ParseTree* tree);

    // test if a string is made of digits
    bool is_number(char* str);

    // parse property string
    void parse_props(PropVect& vect, char* str);

    // parse semantic properties
    PropVect* parse_sem(char* str);

  private:
    // stores the command to execute when we do parsing
    std::string _command;
    // buffer to store one line of parse
    char _buf[BUF_SIZE];
    // shows if parser is ready
    bool _ready;
    // file stream with parsed data
    std::ifstream _pdata;
};

#endif // __parser_hpp__
