/* -*- C++ -*- */

// $Id: Pattern.hpp,v 1.4 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef __pattern_hpp__
#define __pattern_hpp__

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <except.h>
#include "ParseTree.hpp"

// One slot of a syntactic pattern
typedef struct _PSlot {
  _PSlot() : id(~0), parent(~0)
  { }

  _PSlot(unsigned long id_, std::string& role_,
          unsigned long parent_, std::string& attr_) : id(id_),
                                                       parent(parent_),
                                                       role(role_),
                                                       attr(attr_)
  { }

  // identifier of a slot in a pattern
  unsigned long id;
  // the parent of a slot in a pattern
  unsigned long parent;
  // the role of a slot in a pattern
  std::string role;
  // the attribute of a slot in a pattern
  std::string attr;
  // identifiers of children nodes
  std::vector<unsigned long> children;
} PSlot;

// One slot of an instance of a syntactic pattern
typedef struct _ISlot {
  _ISlot() : id(~0)
  { }

  _ISlot(unsigned long id_, std::string& word_, std::string& stem_) : id(id_),
                                                                      word(word_),
                                                                      stem(stem_)
  { }

  // identifier of a slot in an instance
  unsigned long id;
  // word that fills a slot in an instance
  std::string word;
  // stem of the word that fills a slot in an instance
  std::string stem;
} ISlot;

typedef std::vector<ISlot> Instance;

class Pattern {
public:
  Pattern(unsigned long id, unsigned long node_cnt) throw (cmlib::Exception);
  ~Pattern();

  // get pattern id
  unsigned long get_id() const;

  // get the number of slots in a pattern
  unsigned long get_slot_count() const;

  // get a slot by its identifier
  PSlot* get_slot(unsigned long id) const throw (cmlib::Exception);

  // get the total number of slot nodes of a pattern node
  unsigned long get_child_count(unsigned long id) const throw (cmlib::Exception);

  // check if a pattern is present in the parse tree
  bool match_tree(ParseTree* tree, Instance& inst);

  // print the pattern
  void print();

  // iterator to go through the child nodes of a pattern
  class PSlotChildIterator;
  friend class PSlotChildIterator;

  // iterator to go through the roots of a pattern
  class PatRootIterator;
  friend class PatRootIterator;

private:
  bool match_nodes(ParseTree *tree, PSlot *pnode,
                   Node *tnode, Instance& inst)
    throw();

  void print_slot(const PSlot *pslot, unsigned int level);

  void print_r(const PSlot *pslot);

private:
  // identifier of a pattern
  const unsigned long _id;
  // nodes of a pattern
  PSlot* _slots;
  // number of nodes in a pattern
  const unsigned long _slots_count;

  /* used for depth-first search */
  std::vector<int> _ord;
  unsigned int _cnt;
  unsigned int _level;
};

/*
 * iterator to traverse through the children of a slot
 * in a pattern
 */
class Pattern::PSlotChildIterator {
public:
  PSlotChildIterator(const Pattern *pat,
                     const PSlot *pnode) : _pslot(pnode),
                                           _pat(pat)
  {}

  // initialize iterator
  void begin();
  // move to the next child node
  void next();
  // check if the last child is reached
  bool end();
  // get the node
  const PSlot* slot();
private:
  const PSlot *_pslot;
  const Pattern *_pat;
  unsigned long _i;
};

/*
 * iterator to traverse through the roots of a pattern
 */
class Pattern::PatRootIterator {
public:
  PatRootIterator(const Pattern *pat) : _pat(pat),
                                        _i(0)
  { }

  // initialize iterator
  void begin();
  // move to the next root node
  void next();
  // check if the last root is reached
  bool end();
  // get the pattern node
  const PSlot* node();
private:
  const Pattern *_pat;
  unsigned long _i;
};

#endif // __pattern_hpp__
