/* -*- C++ -*- */

// $Id: IndexIO.hpp,v 1.14 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef _indexio_hpp_
#define _indexio_hpp_

#include <except.h>
#include "Types.hpp"

class IndexIO {
public:
  virtual ~IndexIO() {
  }
  // initialize the document collection
  virtual bool init(const char **) throw() = 0;
  // close the document collection
  virtual void close() throw() = 0;
  // get collection statistics (total number and size of all documents)
  virtual void get_col_stats(unsigned int &doc_cnt, unsigned int &doc_size) throw(cmlib::Exception) = 0;
  // get total number of all terms in the collection vocabulary
  virtual unsigned int get_col_vocab_size() throw(cmlib::Exception) = 0;
  // given term ID, get the IDs of all instances, which contain the term
  virtual IdVec* get_term_inst_vec(const ID termID) throw(cmlib::Exception) = 0;
  // given stem ID, get the IDs of all instances, which contain the stem
  virtual IdVec* get_stem_inst_vec(const ID stemID) throw(cmlib::Exception) = 0;
  // get context of an instance
  virtual const StrVec* get_inst_context(const ID instID) throw(cmlib::Exception) = 0;
  // get the ID of a term
  virtual ID get_term_id(const std::string& term) throw(cmlib::Exception) = 0;
  // get a term for its ID; empty string on error
  virtual std::string get_term_for_id(const ID termID) throw(cmlib::Exception) = 0;
  // get the number of occurrences of a term in a document
  virtual unsigned int get_term_doc_count(const ID termID, const ID docID) throw(cmlib::Exception) = 0;
  // get the number of occurrences of a term in a collection
  virtual unsigned int get_term_col_count(const ID termID) throw(cmlib::Exception) = 0;
  // get document ID for instance ID
  virtual ID get_doc_id_for_inst(const ID instID) throw(cmlib::Exception) = 0;
  // get document ID for sentence ID
  virtual ID get_doc_id_for_sent(const ID sentID) throw(cmlib::Exception) = 0;
  // get sentence ID for instance ID
  virtual ID get_sent_id_for_inst(const ID instID) throw(cmlib::Exception) = 0;
  // get pattern ID for instance ID
  virtual ID get_pat_id_for_inst(const ID instID) throw(cmlib::Exception) = 0;
  // get info for all slots of the pattern
  virtual const SlotMap* get_slot_map_for_inst(const ID instID) throw(cmlib::Exception) = 0;
  // get the number of words in a document
  virtual unsigned int get_doc_length(const ID docID) throw(cmlib::Exception) = 0;
  // check if a session with the given ID already exists
  virtual bool session_exists(const ID sessID) throw(cmlib::Exception) = 0;
  // create a new session and return its ID
  virtual ID create_new_session(const std::string &query, bool feedback=false) throw(cmlib::Exception) = 0;
  // add new question to the database
  virtual void add_new_question(const QuestInfo& qinfo) throw(cmlib::Exception) = 0;
  // get absolute question ID for session ID and its rank
  virtual ID get_quest_id(const ID sessID, unsigned int rank) throw(cmlib::Exception) = 0;
  // get questions from a session
  virtual const QuestMap* get_session_questions(const ID sessID, const unsigned int beg_rank, const unsigned int end_rank) throw(cmlib::Exception) = 0;
  // get a query for an existing session
  virtual std::string get_session_query(const ID sessID) throw(cmlib::Exception) = 0;
  // get query suggestions
  virtual QueryStatsVec* get_query_suggestions() throw(cmlib::Exception) = 0;
  // mark question as shown
  virtual void mark_quest_shown(const ID questID) throw(cmlib::Exception) = 0;
  // mark question as clicked
  virtual void mark_quest_clicked(const ID sessID, const unsigned int quest_rank) throw(cmlib::Exception) = 0;
  // check if question was clicked before
  virtual bool check_quest_clicked(const ID questID) throw(cmlib::Exception) = 0;
  // judge question well-formedness
  virtual void mark_quest_wellf(const ID sessID, const unsigned int quest_rank, bool wellf=true) throw(cmlib::Exception) = 0;
  // judge question interestingness
  virtual void mark_quest_inter(const ID sessID, const unsigned int quest_rank, bool inter=true) throw(cmlib::Exception) = 0;
  // judge question relevance
  virtual void mark_quest_relev(const ID sessID, const unsigned int quest_rank, bool relev=true) throw(cmlib::Exception) = 0;
  // mark question as clicked
  virtual void mark_quest_clicked(const ID questID, bool clicked=true) throw(cmlib::Exception) = 0;
  // check if document is already indexed
  virtual bool check_doc_indexed(const ID docID) throw(cmlib::Exception) = 0;
  // add document to index
  virtual ID add_new_doc(const std::string &path) throw(cmlib::Exception) = 0;
  // mark document as indexed
  virtual void mark_doc_indexed(const ID docID, unsigned int word_count) throw(cmlib::Exception) = 0;
  // add new sentence
  virtual ID add_new_sentence(const ID docID, const char *sent) throw(cmlib::Exception) = 0;
  // get last instance ID
  virtual ID get_last_inst_id() throw(cmlib::Exception) = 0;
  // add new instance
  virtual void add_instance(const InstInfo& inst) throw(cmlib::Exception) = 0;
  // add new term
  virtual ID add_new_term(const std::string& term) throw(cmlib::Exception) = 0;
  // clear stored document sentences
  virtual void clear_doc_sents(const ID docID) throw(cmlib::Exception) = 0;
  // get document ID by document path
  virtual ID get_doc_id_for_path(const std::string &path) throw(cmlib::Exception) = 0;
  // set the type of a query
  virtual void set_query_type(const ID sessID, const unsigned int qtype) throw(cmlib::Exception) = 0;
};

#endif /* _indexio_hpp_ */
