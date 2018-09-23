/* -*- C++ -*- */

// $Id: Types.hpp,v 1.13 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef _types_hpp_
#define _types_hpp_

#include <set>
#include <map>
#include <string>
#include <vector>

typedef std::vector<std::string> StrVec;
typedef StrVec::iterator StrVecIt;
typedef StrVec::const_iterator cStrVecIt;

typedef std::vector<StrVec> VecOfStrVecs;
typedef VecOfStrVecs::iterator VecOfStrVecsIt;
typedef VecOfStrVecs::const_iterator cVecOfStrVecsIt;

typedef std::set<std::string> StrSet;
typedef StrSet::iterator StrSetIt;
typedef StrSet::const_iterator cStrSetIt;

typedef std::map<std::string, std::string> StrStrMap;
typedef StrStrMap::iterator StrStrMapIt;
typedef StrStrMap::const_iterator cStrStrMapIt;

typedef unsigned int ID;
typedef std::vector<ID> IdVec;
typedef IdVec::iterator IdVecIt;
typedef IdVec::const_iterator cIdVecIt;

typedef std::set<ID> IdSet;
typedef IdSet::iterator IdSetIt;
typedef IdSet::const_iterator cIdSetIt;

typedef std::map<ID, std::string> IdStrMap;
typedef IdStrMap::iterator IdStrMapIt;
typedef IdStrMap::const_iterator cIdStrMapIt;

typedef std::vector<IdVec> VecOfIdVecs;
typedef VecOfIdVecs::iterator VecOfIdVecsIt;
typedef VecOfIdVecs::const_iterator cVecOfIdVecsIt;

typedef std::map<unsigned int, VecOfIdVecs> IdPowSet;
typedef IdPowSet::iterator IdPowSetIt;
typedef IdPowSet::const_iterator cIdPowSetIt;
typedef IdPowSet::reverse_iterator rIdPowSetIt;

typedef std::map<ID, IdVec*> IdVecMap;
typedef IdVecMap::iterator IdVecMapIt;
typedef IdVecMap::const_iterator cIdVecMapIt;

typedef std::map<std::string, std::string> StrStrMap;
typedef StrStrMap::iterator StrStrMapIt;
typedef StrStrMap::const_iterator cStrStrMapIt;

typedef std::map<ID, double> ScoreMap;
typedef ScoreMap::iterator ScoreMapIt;
typedef ScoreMap::const_iterator cScoreMapIt;

typedef std::map<unsigned int, unsigned int> CntMap;
typedef CntMap::iterator CntMapIt;
typedef CntMap::const_iterator cCntMapIt;

// One position in a slot of a syntactic pattern
typedef struct SlotPos_ {
  SlotPos_(const std::string& term_, const std::string& stem_ ) : term(term_), stem(stem_)
  { }

  SlotPos_() : term(""), stem("")
  { }
  // term filling a slot
  std::string term;
  // stem filling a slot
  std::string stem;
} SlotPos;

// A slot is a numbered set of filled positions
typedef std::map<unsigned int, SlotPos> Slot;
typedef Slot::iterator SlotIt;
typedef Slot::const_iterator cSlotIt;

// mapping from slot IDs to slot contents
typedef std::map<unsigned int, Slot> SlotMap;
typedef SlotMap::iterator SlotMapIt;
typedef SlotMap::const_iterator cSlotMapIt;

typedef struct QuestInfo_ {
  QuestInfo_() : inst_id(0), sess_id(0), qterms(0), rank(0), num_matches(0), score(0.0),
                 quest(""), shown(false), clicked(false), wellf(false), inter(false),
                 relev(false), origq_id(0)
  { }
  // ID of an instance, corresponding to a question
  unsigned int inst_id;
  // ID of a session during which this question was shown
  unsigned int sess_id;
  // number of query terms contained in the question
  unsigned int qterms;
  // rank of the question in the list of query results
  unsigned int rank;
  // number of query terms that an instance matches
  unsigned int num_matches;
  // score of the document associated with the question
  double score;
  // question itself
  std::string quest;
  // question was shown
  bool shown;
  // question was clicked
  bool clicked;
  // question is well-formed
  bool wellf;
  // question is interesting
  bool inter;
  // question is relevant
  bool relev;
  // ID of original clicked question (for feedback questions)
  unsigned int origq_id;
} QuestInfo;

typedef std::map<unsigned int, QuestInfo> QuestMap;
typedef QuestMap::iterator QuestMapIt;
typedef QuestMap::const_iterator cQuestMapIt;

typedef struct QueryStats_ {
  QueryStats_(const std::string &query_,
              const unsigned int type_,
              const unsigned int count_) : query(query_),
                                           type(type_),
                                           count(count_)
  { }

  QueryStats_() : query(""),
                  type(0),
                  count(0)
  { }

  // query itself
  std::string query;
  // query type
  unsigned int type;
  // number of times a query has been posed already
  unsigned int count;
} QueryStats;

typedef std::vector<QueryStats> QueryStatsVec;
typedef QueryStatsVec::iterator QueryStatsVecIt;
typedef QueryStatsVec::const_iterator cQueryStatsVecIt;

// single entry in the instances table of the index
typedef struct InstInfo_ {
  InstInfo_() : inst_id(0), doc_id(0),
                sent_id(0), term_id(0),
                stem_id(0), pat_id(0),
                slot_id(0)
  { }

  // instance ID
  unsigned int inst_id;
  // document ID
  unsigned int doc_id;
  // sentence ID
  unsigned int sent_id;
  // term ID
  unsigned int term_id;
  // stem ID
  unsigned int stem_id;
  // pattern ID
  unsigned int pat_id;
  // slot ID
  unsigned int slot_id;
  // position within the slot
  unsigned int slot_pos;
} InstInfo;

 /* collection of information tuples about instances */
typedef std::vector<InstInfo> InstInfoVec;
typedef InstInfoVec::iterator InstInfoVecIt;
typedef InstInfoVec::const_iterator cInstInfoVecIt;

#endif /* _types_hpp_ */
