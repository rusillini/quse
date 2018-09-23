/* -*- C++ -*- */

// $Id: Parser.cpp,v 1.2 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <Parser.hpp>

Parser::Parser()
{
    _ready = false;
    // forming the command line to call minipar with
    std::string minipar, minipar_data;
    if(getenv("ENTL_HOME") == NULL) {
        printf("Error: ENTL_HOME environment variable not found!\n");
        exit(1);
    }

    minipar = cmlib::makeFullString("%ENTL_HOME%/minipar/bin/pdemo");
    minipar_data = cmlib::makeFullString("%ENTL_HOME%/minipar/data");

    _command = minipar + " -p " + minipar_data + " -i -l -f \"" + std::string(FEATURES) + "\" > " + std::string(FILENAME);
    _ready = true;
}

Parser::~Parser()
{ }

void Parser::parse_line(ParseTree* tree, const char* line)
{
    unsigned int i, num_chars;
    char *token, *space_pos;
    char stem[256];
    Node* node = NULL;
    // to know that we need to add a new node
    bool new_node = false;
    //printf("%s\n", line);
    unsigned int num = ~0;

    token = strtok((char* ) line, "\t");

    for(i = 1; token != NULL; i++) {
        if(i == 1) {
            if(!is_number(token))
                return;
            else {
                num = (unsigned int) atoi(token);
                node = tree->get_node(num);
                if(node == NULL) {
                    // we're creating a new node here
                    node = new Node();
                    node->n = num;
                    new_node = true;
                } else
                    node->n = num;
            }
        }
        else if(i == 2) {
            // parsing word
            node->word = token + 1;
        }
        else if(i == 3) {
            space_pos = strrchr(token, ' ');
            num_chars = space_pos-token;
            strncpy(stem, token, num_chars);
            stem[num_chars] = '\0';

            if(stem[0] == '~')
                node->stem = node->word;
            else
                node->stem = stem;
        }
        else if(i == 4) {
            unsigned int parent;
            if(is_number(token)) {
                parent = (unsigned int) atoi(token);
                tree->add_child_node(parent, num);
                node->parent = parent;
            }
        }
        else if(i == 5) {
            node->role = token;
        }
        else if(i == 7) {
            // parsing syntactic and semantic properties
            parse_props(node->props, token);
        }
        token = strtok(NULL, "\t");
    }

    if(new_node)
        tree->add_node(node, num);
}

void Parser::parse_props(PropVect& vect, char* str)
{
    char* token;
    char buf[512];
    char* sign_loc;
    unsigned int num_chars;
    std::string prop;
    int state = 0; // state of the parser
    // 1. syntactic
    // 2. semantic
    token = strtok(str, "()");
    do {
        if(!((strlen(token) == 0 || strlen(token) == 1) && token[0] == ' ')) {
            if(strstr(token, "atts") != NULL)
                state = 1;
            else if(strstr(token, "sem") != NULL)
                state = 2;
            else {
                if(state == 1) {
                    sign_loc = strchr(token, '+');
                    if(sign_loc != NULL) {
                        num_chars = sign_loc-token-1;
                        strncpy(buf, token, num_chars);
                        buf[num_chars] = '\0';
                        prop = std::string(buf);
                        vect.push_back(prop);
                    }
                } else if(state == 2) {
                    sign_loc = strchr(token, '+');
                    if(sign_loc != NULL) {
                        num_chars = sign_loc-token;
                        strcpy(buf, token+num_chars);
                        PropVect* sem_props = parse_sem(buf);
                        for(unsigned int i = 0; i < sem_props->size(); i++)
                            vect.push_back(sem_props->at(i));
                        delete sem_props;
                    }
                }
            }
        }
        token = strtok(NULL, "()");
    } while(token != NULL);
}

void Parser::normalize_tree(ParseTree* tree)
{
    ParseTree::TreeIterator it(tree);

    for(it.begin(); !it.end(); it.next()) {
        if(it.node()->parent == (unsigned int) ~0)
            tree->add_root(it.node());
    }
}

ParseTree* Parser::parse_sentence(const char* sent)
{
    std::string command;
    ParseTree* tree = new ParseTree();

    command = "echo \"" + std::string(sent) + "\" | " + _command;
    system(command.c_str());

    _pdata.open(FILENAME);
    if(!_pdata.is_open()) {
        printf("Error: can't open file %s!\n", FILENAME);
        exit(1);
    }

    while(!_pdata.eof()) {
       _pdata.getline(_buf, BUF_SIZE);
       parse_line(tree, _buf);
    }

    normalize_tree(tree);

    _pdata.close();
    return tree;
}

PropVect* Parser::parse_sem(char* str)
{
    char* token;
    PropVect* vect = new PropVect();
    token = strtok(str, "+ ");
    do {
        vect->push_back(std::string(token));
        token = strtok(NULL, "+ ");
    } while(token != NULL);

    return vect;
}

bool Parser::is_ready() const
{  return _ready; }

bool Parser::is_number(char* str)
{
    while(*str) {
        if(!isdigit(*str))
            return false;
        else
            str++;
    }

    return true;
}

