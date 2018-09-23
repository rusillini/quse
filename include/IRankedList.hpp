/* -*- C++ -*- */

// $Id: IRankedList.hpp,v 1.3 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef _irankedlist_hpp_
#define _irankedlist_hpp_

/*
 * Interface for ranked lists
 */

template<class T> class IRankedList {
public:
  virtual ~IRankedList<T>() { }
  virtual void begin() = 0;
  virtual bool has_next() = 0;
  virtual void next() = 0;
  virtual void add(const T&) = 0;
  virtual T& get() const = 0;
  virtual void set(T&) = 0;
  virtual T& operator[](unsigned int) = 0;
  virtual unsigned int size() const = 0;
  virtual T& get_by_index(unsigned int) = 0;
};

#endif /* _irankedlist_hpp_ */
