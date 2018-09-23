/* -*- C++ -*- */

// $Id: ParseTree.cpp,v 1.2 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <ParseTree.hpp>

ParseTree::ParseTree()
{
  _words_cnt = 0;
}

ParseTree::~ParseTree()
{
  clear_nodes();
}

Node* ParseTree::add_word(const char* word, unsigned int num)
{
  Node* node = new Node();
  node->word = word;
  node->n = num;
  _words[num] = node;
  return node;
}

Node* ParseTree::add_word(const char* word, const char* stem, unsigned int num)
{
  Node* node = new Node();
  node->word = word;
  node->stem = stem;
  node->n = num;
  _words[num] = node;
  return node;
}

Node* ParseTree::add_stem(const char* stem, unsigned int num)
{
  Node* node = new Node();
  node->stem = stem;
  node->n = num;
  _words[num] = node;
  return node;
}

void ParseTree::add_root(Node* node)
{
  _roots.insert(node);
}

void ParseTree::clear_nodes()
{
  for(WordsBagIt it = _words.begin(); it != _words.end(); it++)
    delete it->second;
}

void ParseTree::clear_tree()
{
  for(cWordsBagIt it = _words.begin(); it != _words.end(); it++)
    delete it->second;
  _words.clear();
  _roots.clear();
  _words_cnt = 0;
}

void ParseTree::print_node(Node* node, unsigned int level)
{
  unsigned int i;
  for(i = 0; i < level; i++)
    printf("  ");
  printf("%s: %s (", node->stem.c_str(), node->role.c_str());
  for(PropsIt it = node->props.begin(); it != node->props.end(); it++)
    printf(" %s", it->c_str());
  printf(" )\n");
}

void ParseTree::print()
{
  ParseTree::RootIterator it(*this);

  if(_words.size() == 0)
    return;

  _ord.clear();
  _ord.assign(get_node_count(), -1);
  _cnt = 0;
  _level = 0;

  for(it.begin(); !it.end(); it.next()) {
    print_r(it.node());
  }
}

void ParseTree::print_r(Node* node)
{
  _ord[node->n] = _cnt++;
  print_node(node, _level);
  _level++;
  ParseTree::ChildIterator it(*this, node);
  for(it.begin(); !it.end(); it.next()) {
    if(_ord[it.node()->n] == -1)
      print_r(it.node());
  }
  _level--;
}

void ParseTree::add_child_node(unsigned int node_id, unsigned int child_id)
{
  // node to which we add a child
  Node* node;
  // checking if the parent node with given id exists
  WordsBagIt it = _words.find(node_id);
  if(it == _words.end()) {
    node = new Node();
    _words[node_id] = node;
  } else
    node = it->second;
  node->children.push_back(child_id);
}

void ParseTree::add_node(Node* node, unsigned int num)
{
  _words[num] = node;
}

unsigned int ParseTree::get_node_count()
{
  return _words.size();
}

void ParseTree::inc_words_cnt()
{
  _words_cnt++;
}

unsigned long ParseTree::get_words_cnt() const
{
  return _words_cnt;
}

Node* ParseTree::get_node(unsigned int id)
{
  if(node_exists(id))
    return _words[id];
  else
    return NULL;
}

bool ParseTree::node_exists(unsigned int id)
{
  WordsBagIt it = _words.find(id);
  return it != _words.end();
}

void ParseTree::normalize()
{
  TreeIterator it(this);

  for(it.begin(); !it.end(); it.next())
    if(it.node()->parent == (unsigned int) ~0)
      this->add_root(it.node());
}

/* StemIterator */

void ParseTree::StemIterator::begin()
{
  _it = _tree._words.begin();
  if(_it->second->stem == _stem)
    return;
  else
    next();
}

void ParseTree::StemIterator::next()
{
  while(++_it != _tree._words.end())
    if(_it->second->stem == _stem)
      return;
}

bool ParseTree::StemIterator::end()
{
  return _it == _tree._words.end();
}

Node* ParseTree::StemIterator::node()
{
  return _it->second;
}

/* TreeIterator */

void ParseTree::TreeIterator::begin()
{
  _it = _tree->_words.begin();
}

void ParseTree::TreeIterator::next()
{
  if(_it != _tree->_words.end())
    _it++;
}

bool ParseTree::TreeIterator::end()
{
  return _it == _tree->_words.end();
}

Node* ParseTree::TreeIterator::node()
{
  return _it->second;
}

/* ConstTreeIterator */

void ParseTree::ConstTreeIterator::begin()
{
  _it = _tree->_words.begin();
}

void ParseTree::ConstTreeIterator::next()
{
  if(_it != _tree->_words.end())
    _it++;
}

bool ParseTree::ConstTreeIterator::end()
{
  return _it == _tree->_words.end();
}

const Node* ParseTree::ConstTreeIterator::node()
{
  return (const Node* ) _it->second;
}

/* RootIterator */

void ParseTree::RootIterator::begin()
{
  _it = _tree._roots.begin();
}

void ParseTree::RootIterator::next()
{
  if(_it != _tree._roots.end())
    _it++;
}

bool ParseTree::RootIterator::end()
{
  return _it == _tree._roots.end();
}

Node* ParseTree::RootIterator::node()
{
  return (*_it);
}

/* ChildIterator */

void ParseTree::ChildIterator::begin()
{
  _i = 0;
}

void ParseTree::ChildIterator::next()
{
  if(_i < _node->children.size())
    _i++;
}

bool ParseTree::ChildIterator::end()
{
  return _i == _node->children.size();
}

Node* ParseTree::ChildIterator::node()
{
  return _tree._words[_node->children[_i]];
}

/* CondChildIterator */
void ParseTree::CondChildIterator::begin()
{
  Node *n;
  _i = 0;
  if(_node->children.size() == 0)
    return;
  n = _tree->_words[_node->children[_i]];
  if(n->role == _role &&
     (_attr.length() == 0 || (n->props.find(_attr) != n->props.end())))
    return;
  else
    next();
}

void ParseTree::CondChildIterator::next()
{
  Node *n;
  while(++_i < _node->children.size()) {
    n = _tree->_words[_node->children[_i]];
    if(n->role == _role &&
       (_attr.length() == 0 || (n->props.find(_attr) != n->props.end())))
      break;
  }
}

bool ParseTree::CondChildIterator::end()
{
  return _i == _node->children.size();
}

Node* ParseTree::CondChildIterator::node()
{
  return _tree->_words[_node->children[_i]];
}

/* Conditional Node Iterator */
void ParseTree::CondNodeIterator::begin()
{
  _it = _tree->_words.begin();
  if(_it == _tree->_words.end())
    return;
  if( _it->second->role == _role &&
     ( _attr.length() == 0 ||
      (_it->second->props.find(_attr) != _it->second->props.end())
     )
    )
    return;
  else
    next();
}

void ParseTree::CondNodeIterator::next()
{
  while(++_it != _tree->_words.end()) {
    if(_it->second->role == _role &&
       ( _attr.length() == 0 ||
        (_it->second->props.find(_attr) != _it->second->props.end())
       )
      )
      break;
  }
}

bool ParseTree::CondNodeIterator::end()
{
  return _it == _tree->_words.end();
}

Node* ParseTree::CondNodeIterator::node()
{
  return _it->second;
}

/* Reverse Child Iterator */

void ParseTree::RevChildIterator::begin()
{
  if(_node->children.size() == 0)
    _i = 0;
  else
    _i = _node->children.size() - 1;
}

void ParseTree::RevChildIterator::next()
{
  if(_i >= 0) _i--;
}

bool ParseTree::RevChildIterator::end()
{
  return _i == 0;
}

Node* ParseTree::RevChildIterator::node()
{
  return _tree._words[_node->children[_i]];
}

