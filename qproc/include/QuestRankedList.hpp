/* -*- C++ -*- */

// $Id: QuestRankedList.hpp,v 1.4 2010/04/24 21:30:03 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group

#ifndef _questrankedlist_hpp_
#define _questrankedlist_hpp_

#include <IRankedList.hpp>
#include <Question.hpp>

/*
 * Ranked list of questions for a textual query
 */
class QuestRankedList : public IRankedList<Question> {
protected:
  // maximum size of the ranked list
  unsigned int _max_size;
  QuestVec _inst_vec;
  QuestVecIt _inst_vec_it;

public:

  // Initialize question list with its maximum size
  QuestRankedList(unsigned int max_size) : _max_size(max_size) {
    _inst_vec_it = _inst_vec.end();
  }

  virtual ~QuestRankedList()
  {  }

  // start iterating through the ranked list
  virtual void begin() {
    _inst_vec_it = _inst_vec.begin();
  }

  // check if there is a next element in the ranked list
  virtual bool has_next() {
    return _inst_vec_it != _inst_vec.end();
  }

  // move to the next element in the ranked list
  virtual void next() {
    _inst_vec_it++;
  }

  // get the element from the ranked list currently pointed to by the iterator
  virtual Question& get() const {
    return *_inst_vec_it;
  }

  // assign the element of the ranked list currently pointed to by the iterator
  virtual void set(Question &i) {
    *_inst_vec_it = i;
  }

  // add element t the ranked list (it is ranked while being added)
  virtual void add(const Question &i) {
    QuestVecIt it;
    for (it = _inst_vec.begin(); it != _inst_vec.end(); it++) {
      if(it->same(i)) {
        if(*it < i) {
          _inst_vec.erase(it);
          add(i);
          return;
        } else {
          return;
        }
      }
      if (*it < i) {
        _inst_vec.insert(it, i);
        if(_max_size != 0 && _inst_vec.size() > _max_size)
          _inst_vec.pop_back();
        return;
      }
    }

    if(_max_size != 0) {
      if(_inst_vec.size() < _max_size)
        _inst_vec.push_back(i);
    } else {
      _inst_vec.push_back(i);
    }
  }

  // get the number of hits in the ranked list
  virtual unsigned int size() const {
    return _inst_vec.size();
  }

  // get an element of the ranked list by its ordinal number
  virtual Question& get_by_index(unsigned int n) throw (std::exception) {
    return _inst_vec.at(n);
  }

  virtual Question& operator[](unsigned int n) throw (std::exception) {
    return _inst_vec.at(n);
  }
};

#endif /* _questrankedlist_hpp_ */
