/* -*- C++ -*- */

// $Id: Question.hpp,v 1.4 2010/04/24 21:30:03 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef _question_hpp_
#define _question_hpp_

#include <string>
#include <vector>
#include <Types.hpp>

/*
 * Instance of a question pattern matching one or more query terms
 */

class Question {
public:
  /*
   * inst_id - ID of an instance of syntactic pattern, corresponding to the question;
   * doc_id - ID of the document corresponding to an instance;
   * qterms_cnt - number of query terms matched by the question
   * match_cnt - number of query terms matched by an instance, corresponding to the question pattern;
   * score - score of the document corresponding to an instance;
   * quest - question, corresponding to an instance
   */
  Question(ID inst_id, ID doc_id, unsigned int qterms_cnt, double score, unsigned int match_cnt, const std::string &quest) :
           _inst_id(inst_id),_qterms_cnt(qterms_cnt),_match_cnt(match_cnt),_doc_id(doc_id),_score(score),_quest(quest)
  { }

  /*
   * match_cnt - number of query terms matched by an instance;
   * doc_id - ID of the document corresponding to an instance;
   * score - score of the document corresponding to an instance;
   * quest - question, corresponding to an instance
   */
  Question(ID inst_id, ID doc_id, unsigned int qterms_cnt, double score, unsigned int match_cnt) :
           _inst_id(inst_id),_qterms_cnt(qterms_cnt),_match_cnt(match_cnt),_doc_id(doc_id),_score(score)
  { }

  Question() : _inst_id(0),_qterms_cnt(0),_match_cnt(0),_doc_id(0),_score(0.0)
  { }

  ~Question()
  { }

  unsigned int get_qterms_cnt() const {
     return _qterms_cnt;
  }

  void set_qterms_cnt(unsigned int cnt) {
     _qterms_cnt = cnt;
  }

  unsigned int get_match_cnt() const {
    return _match_cnt;
  }

  void set_match_cnt(unsigned int cnt) {
    _match_cnt = cnt;
  }

  ID get_inst_id() const {
    return _inst_id;
  }

  void set_inst_id(ID inst_id) {
    _inst_id = inst_id;
  }

  ID get_doc_id() const {
    return _doc_id;
  }

  void set_doc_id(ID doc_id) {
    _doc_id = doc_id;
  }

  double get_doc_score() const {
    return _score;
  }

  void set_doc_score(double score) {
    _score = score;
  }

  void set_quest(std::string quest) {
    _quest = quest;
  }

  const std::string& get_quest() const {
    return _quest;
  }

  /*
   *  ATTENTION: QUESTION RANKING IS DONE HERE
   *  questions are first ranked by the number of query terms they include;
   *  then by scores of documents, containing instances of syntactic patterns, corresponding to a question;
   *  finally by the number of query terms contained in the slots of syntactic pattern, corresponding to a question;
   */
  bool operator< (const Question &i) const {
    if(_qterms_cnt > i._qterms_cnt) {
      return false;
    } else if(_qterms_cnt < i._qterms_cnt) {
      return true;
    } else {
      if(_match_cnt > i._match_cnt) {
         return false;
      } else if(_match_cnt < i._match_cnt) {
         return true;
      } else {
         return _score < i._score;
      }
    }
  }

  bool same (const Question &i) const {
    if(_quest.compare(i._quest) != 0)
      return false;
    else if(_inst_id != i._inst_id)
      return false;
    else
      return true;
  }

private:
  // ID of an instance of syntactic pattern, corresponding to the question
  ID _inst_id;
  // number of query terms contained in the question
  unsigned int _qterms_cnt;
  // number of query terms matched by an instance
  unsigned int _match_cnt;
  // ID of a document corresponding to an instance
  ID _doc_id;
  // score of the document, containing a syntactic instance
  double _score;
  // question, corresponding to a syntactic instance
  std::string _quest;
};

typedef std::vector<Question> QuestVec;
typedef QuestVec::iterator QuestVecIt;
typedef QuestVec::const_iterator cQuestVecIt;

#endif /* _question_hpp_ */
