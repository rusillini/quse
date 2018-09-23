// -*- C++ -*-

// $Id: ParseTree.hpp,v 1.2 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef __parse_tree_hpp__
#define __parse_tree_hpp__

#include <map>
#include <vector>
#include <stack>
#include <set>
#include <string>
#include <string.h>
#include <stdio.h>

typedef std::set<std::string> Props;
typedef Props::const_iterator PropsIt;

typedef struct __node {
  __node() : n(~0), parent(~0)
  { }
  // the ordinal number of a word in the sentence
  unsigned int n;
  // a word in the sentence that this node represents
  std::string word;
  // the stem form of a word
  std::string stem;
  // syntactic role of a word
  std::string role;
  // the ordinal number of the parent of a node in the bag of words
  unsigned int parent;
  // children of a node
  std::vector<unsigned int> children;
  // syntactic and semantic properties of the node
  Props props;
} Node;

// parse tree corresponds to a sentence
class ParseTree {
public:
  ParseTree();
  ~ParseTree();

  /* add word to the bag of words */
  Node* add_word(const char* word, unsigned int num);

  /* add word and stem to the bag of words */
  Node* add_word(const char* word, const char* stem, unsigned int num);

  /* add stem to the bag */
  Node* add_stem(const char* stem, unsigned int num);

  /* add node to the bag of words */
  void add_node(Node* node, unsigned int num);

  /* get node by number */
  Node* get_node(unsigned int id);

  /* add child node id to node */
  void add_child_node(unsigned int node_id, unsigned int child_id);

  /* get the number of nodes in the tree */
  unsigned int get_node_count();

  /* increment the number of meaningfull words in the tree */
  void inc_words_cnt();

  /* get the number of meaningfull words in the tree */
  unsigned long get_words_cnt() const;

  /* check if node with this id exists */
  bool node_exists(unsigned int id);

  /* add root of the tree */
  void add_root(Node* node);

  /* locate the roots of the tree and add them to separate container */
  void normalize();

  // print parse tree
  void print();

  // clear tree
  void clear_tree();

public:
  class TreeIterator;
  friend class TreeIterator;

  class ConstTreeIterator;
  friend class ConstTreeIterator;

  class StemIterator;
  friend class StemIterator;

  class RootIterator;
  friend class RootIterator;

  class ChildIterator;
  friend class NodeIterator;

  class RevChildIterator;
  friend class RevNodeIterator;

  class CondNodeIterator;
  friend class CondNodeIterator;

  class CondChildIterator;
  friend class CondChildIterator;

private:
  void clear_nodes();
  void print_node(Node* , unsigned int);
  void print_r(Node* );

private:
  // words stored in the sentence, indexed by the order, in which they occur in the sentence
  typedef std::map<unsigned int, Node*> WordsBagMap;
  typedef WordsBagMap::iterator WordsBagIt;
  typedef WordsBagMap::const_iterator cWordsBagIt;

  typedef std::set<Node*> Roots;
  typedef Roots::iterator RootsIt;

  /* used for depth-first search */
  std::vector<int> _ord;
  unsigned int _cnt;
  unsigned int _level;

  /* number of meaningful words in the sentence */
  unsigned long _words_cnt;

  WordsBagMap _words;
  Roots _roots;
};

/* iterator to browse the nodes in the tree */

class ParseTree::TreeIterator {
public:
  TreeIterator(ParseTree* tree) :
    _tree(tree)
  { }
  // initialize iterator
  void begin();
  // move to the next element
  void next();
  // check if the last node is reached
  bool end();
  // get the node
  Node* node();

private:
  ParseTree *_tree;
  ParseTree::WordsBagIt _it;
};

/* constant iterator to browse the nodes in the tree */
class ParseTree::ConstTreeIterator {
public:
  ConstTreeIterator(const ParseTree *tree) :
    _tree(tree)
  { }
  // initialize iterator
  void begin();
  // move to the next element
  void next();
  // check if the last node is reached
  bool end();
  // get the node
  const Node* node();

private:
  const ParseTree *_tree;
  ParseTree::cWordsBagIt _it;
};


/*
 * iterator to find the occurences of all words with
 * the given stem in the sentence
 */

class ParseTree::StemIterator {
public:
  StemIterator(ParseTree& tree, std::string& stem) :
    _tree(tree), _stem(stem)
  { }

  // initalize iterator
  void begin();
  // move to the next element
  void next();
  // check if the last node is reached
  bool end();
  // get the node
  Node* node();

private:
  ParseTree& _tree;
  std::string _stem;
  ParseTree::WordsBagIt _it;
};

/*
 * iterator to go through all the root nodes (subjects) of
 * the given parse tree of a sentence
 */

class ParseTree::RootIterator {
public:
  RootIterator(ParseTree& tree) : _tree(tree)
  { }

  // initialize iterator
  void begin();
  // move to the next element
  void next();
  // check if the last root node is reached
  bool end();
  // get the root
  Node* node();

private:
  ParseTree& _tree;
  ParseTree::RootsIt _it;
};

/*
 * iterator to traverse through the children
 * of a node
 */

class ParseTree::ChildIterator {
public:
  ChildIterator(ParseTree& tree, Node* node) : _tree(tree),
                                              _node(node)
  { }

  // initialze iterator
  void begin();
  // move to the next child node
  void next();
  // check if the last child node is reached
  bool end();
  // get the node
  Node* node();

private:
  ParseTree& _tree;
  Node* _node;
  unsigned int _i;
};

/*
 * iterator to traverse through the children of
 * a node that satisfy certain condition
 */

class ParseTree::CondNodeIterator {
public:
  CondNodeIterator(ParseTree *tree,
                   std::string& role,
                   std::string& attr) : _tree(tree),
                                        _role(role),
                                        _attr(attr)
  {}

  // initialize iterator
  void begin();
  // move to the next child node
  void next();
  // check if the last child node is reached
  bool end();
  // get the node
  Node* node();

private:
  ParseTree *_tree;
  std::string _role;
  std::string _attr;
  ParseTree::cWordsBagIt _it;
};



/*
 * iterator to traverse through the child nodes
 * of a node in reverse order
 */

class ParseTree::RevChildIterator {
public:
  RevChildIterator(ParseTree& tree, Node* node) : _tree(tree),
                                                 _node(node)
  { }

  // initialize iterator
  void begin();
  // move to the next child node
  void next();
  // check if the last child node is reached
  bool end();
  // get the node
  Node* node();

private:
  ParseTree& _tree;
  Node* _node;
  unsigned int _i;
};


/*
 * iterator to traverse through the child nodes
 * of a node satisfying conditions
 */

class ParseTree::CondChildIterator {
public:
  CondChildIterator(ParseTree *tree, Node *node,
                    std::string& role, std::string& attr) : _tree(tree),
                                                            _node(node),
                                                            _role(role),
                                                            _attr(attr)
  { }

  // initialize iterator
  void begin();
  // move to the next child node
  void next();
  // check if the last child node is reached
  bool end();
  // get the node
  Node* node();

private:
  ParseTree *_tree;
  Node *_node;
  std::string _role;
  std::string _attr;
  unsigned int _i;
};

#endif // __parse_tree_hpp__
