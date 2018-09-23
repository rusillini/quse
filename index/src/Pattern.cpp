/* -*- C++ -*- */

// $Id: Pattern.cpp,v 1.4 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <Pattern.hpp>

Pattern::Pattern(unsigned long id, unsigned long count)
  throw (cmlib::Exception) : _id(id), _slots_count(count)
{
  _slots = NULL;
  if(id == 0 || id == (unsigned int) ~0) {
    cmlib_throw("Pattern::Pattern()", "config.xml - unacceptable pattern id");
  }
  if(count == 0) {
    char buf[14];
    sprintf(buf, "%lu", id);
    cmlib_throw("Pattern::Pattern()", std::string("config.xml - ") + "no <node> child nodes for pattern " + buf);
  } else
    _slots = new PSlot[count];
}

Pattern::~Pattern()
{
  if(_slots != NULL)
    delete [] _slots;
}

unsigned long Pattern::get_id() const
{ return _id; }

PSlot* Pattern::get_slot(unsigned long id) const throw (cmlib::Exception)
{
  if(id < 0 || id > _slots_count) {
    char cid[14], pid[14];
    sprintf(cid, "%lu", id);
    sprintf(pid, "%lu", _id);
    cmlib_throw("Pattern::get_node", std::string("can't find a node with id = ") + cid + " in pattern with id = " + pid);
  }
  else
    return &_slots[id];
}

unsigned long Pattern::get_slot_count() const
{ return _slots_count; }

unsigned long Pattern::get_child_count(unsigned long id) const throw (cmlib::Exception)
{
  if(id < 0 || id > _slots_count) {
    char cid[14], pid[14];
    sprintf(cid, "%lu", id);
    sprintf(pid, "%lu", _id);
    cmlib_throw("Pattern::get_child_count", std::string("can't find a node with id = ") + cid + " in pattern with id = " + pid);
  }
  else
    return _slots[id].children.size();
}

bool Pattern::match_tree(ParseTree* tree, Instance& inst)
{
  bool matched = true;
  Node* n = NULL;
  PSlot *p = &_slots[0], *cp;
  ISlot i;
  ParseTree::CondNodeIterator it(tree, p->role, p->attr);

  for(it.begin(); !it.end(); it.next()) {
    n = it.node();
    for(unsigned int j = 0; j < p->children.size(); j++) {
      cp = &_slots[p->children[j]];
      if(!match_nodes(tree, cp, n, inst)) {
        matched = false;
        break;
      }
    }
    if(matched) {
      i.id = p->id;
      i.word = n->word;
      i.stem = n->stem;
      inst.push_back(i);
      return true;
    }
  }
  return false;
}

bool Pattern::match_nodes(ParseTree *tree, PSlot *pnode,
                          Node* tnode, Instance& inst)
  throw()
{
  bool matched;
  Node* n = NULL;
  PSlot *p = NULL;
  ISlot i;

  ParseTree::CondChildIterator it(tree, tnode, ((PSlot* )pnode)->role, ((PSlot* )pnode)->attr);

  for(unsigned int j = 0; j < pnode->children.size(); j++) {
    matched = false;
    p = &_slots[pnode->children[j]];
    for(it.begin(); !it.end(); it.next()) {
      n = it.node();
      if(match_nodes(tree, p, n, inst)) {
        matched = true;
        break;
      }
    }
    if(!matched)
      return false;
  }

  it.begin();
  if(!it.end()) {
    n = it.node();
    i.id = pnode->id;
    i.word = n->word;
    i.stem = n->stem;
    inst.push_back(i);
    return true;
  } else
    return false;
}

void Pattern::print()
{
  Pattern::PatRootIterator it(this);

  if(_slots_count == 0)
    return;

  _ord.clear();
  _ord.assign(_slots_count, -1);
  _cnt = 0;
  _level = 0;

  for(it.begin(); !it.end(); it.next())
    print_r(it.node());
}

void Pattern::print_slot(const PSlot *pslot, unsigned int level)
{
  unsigned int i;
  for(i = 0; i < level; i++)
    printf(" ");
  printf("%s: {%s}\n", pslot->role.c_str(), pslot->attr.c_str());
}

void Pattern::print_r(const PSlot *pslot)
{
  _ord[pslot->id] = _cnt++;
  print_slot(pslot, _level);
  _level++;
  Pattern::PSlotChildIterator it(this, pslot);
  for(it.begin(); !it.end(); it.next())
    if(_ord[it.slot()->id] == -1)
      print_r(it.slot());
  _level--;
}


/* PNodeChildIterator */

void Pattern::PSlotChildIterator::begin()
{
  _i = 0;
}

void Pattern::PSlotChildIterator::next()
{
  if (_i < _pslot->children.size())
    _i++;
}

bool Pattern::PSlotChildIterator::end()
{
  return _i == _pslot->children.size();
}

const PSlot* Pattern::PSlotChildIterator::slot()
{
  return (const PSlot* ) &_pat->_slots[_pslot->children[_i]];
}

/* PatRootIterator */

void Pattern::PatRootIterator::begin()
{
  if(_pat->_slots[_i].parent == (unsigned long) ~0)
    return;
  else
    next();
}

void Pattern::PatRootIterator::next()
{
  while(++_i < _pat->_slots_count)
    if(_pat->_slots[_i].parent == (unsigned long) ~0)
      break;
}

bool Pattern::PatRootIterator::end()
{
  return _i == _pat->_slots_count;
}

const PSlot* Pattern::PatRootIterator::node()
{
  return (const PSlot* ) &_pat->_slots[_i];
}
