
#ifndef _dbindexio_hpp_
#define _dbindexio_hpp_

#include <Types.hpp>
#include <IndexIO.hpp>
#include <utility>
#include <mysql++.h>
#include <except.h>
#include <stdlib.h>

class DBIndexIO: public IndexIO {
public:
  DBIndexIO(const char *server, const char *db, const char *user,
      const char *passwd);
  virtual ~DBIndexIO();
  // perform initialization
  virtual bool init(const char **msg) throw();
  // establish connection with the database
  void connect() throw(cmlib::Exception);
  // disconnect from the database
  virtual void close() throw();
  // get collection statistics (total number and size of all documents); NULL on error
  virtual void get_col_stats(unsigned int &doc_cnt, unsigned int &doc_size) throw(cmlib::Exception);
  // get total number of all terms in the collection vocabulary
  virtual unsigned int get_col_vocab_size() throw(cmlib::Exception);
  // given term ID, get the IDs of all instances, which contain the term
  virtual IdVec* get_term_inst_vec(const ID termID) throw(cmlib::Exception);
  // given stem ID, get the IDs of all instances, which contain the stem
  virtual IdVec* get_stem_inst_vec(const ID stemID) throw(cmlib::Exception);
  // get context of an instance
  virtual const StrVec* get_inst_context(const ID instID) throw(cmlib::Exception);
  // get the ID of a term; 0 on error
  virtual ID get_term_id(const std::string& term) throw(cmlib::Exception);
  // get a term for its ID; empty string on error
  virtual std::string get_term_for_id(const ID termID) throw(cmlib::Exception);
  // get the number of occurrences of a term in a document; 0 on error
  virtual unsigned int get_term_doc_count(const ID termID, const ID docID)
      throw(cmlib::Exception);
  // get the number of occurrences of a term in a collection; 0 on error
  virtual unsigned int get_term_col_count(const ID termID) throw(cmlib::Exception);
  // get document ID for instance ID
  virtual ID get_doc_id_for_inst(const ID instID) throw(cmlib::Exception);
  // get document ID for sentence ID
  virtual ID get_doc_id_for_sent(const ID sentID) throw(cmlib::Exception);
  // get sentence ID for instance ID
  virtual ID get_sent_id_for_inst(const ID instID) throw(cmlib::Exception);
  // get pattern ID for instance ID
  virtual ID get_pat_id_for_inst(const ID instID) throw(cmlib::Exception);
  // get info for all slots of the pattern
  virtual const SlotMap* get_slot_map_for_inst(const ID instID) throw(cmlib::Exception);
  // get the number of words in a document
  virtual unsigned int get_doc_length(const ID docID) throw(cmlib::Exception);
  // check if a session with the given ID already exists
  virtual bool session_exists(const ID sessID) throw(cmlib::Exception);
  // create a new session and return its ID
  virtual ID create_new_session(const std::string &query, bool feedback=false)
      throw(cmlib::Exception);
  // add new question to the database
  virtual void add_new_question(const QuestInfo& qinfo) throw(cmlib::Exception);
  // get absolute question ID for session ID and its rank
  virtual ID get_quest_id(const ID sessID, unsigned int rank) throw(cmlib::Exception);
  // get questions from an existing session
  virtual const QuestMap* get_session_questions(const ID sessID, const unsigned int beg_rank, const unsigned int end_rank)
      throw(cmlib::Exception);
  // get a query for an existing session
  virtual std::string get_session_query(const ID sessID) throw(cmlib::Exception);
  // get query suggestions
  virtual QueryStatsVec* get_query_suggestions() throw(cmlib::Exception);
  // mark question as shown
  virtual void mark_quest_shown(const ID questID)
      throw(cmlib::Exception);
  // mark question as clicked
  virtual void mark_quest_clicked(const ID sess_id, const unsigned int rank)
      throw(cmlib::Exception);
  // check if question was clicked before
  virtual bool check_quest_clicked(const ID questID)
      throw(cmlib::Exception);
  // mark question as clicked
  virtual void mark_quest_clicked(const ID questID, bool clicked=true)
      throw(cmlib::Exception);
  // judge question well-formedness
  virtual void mark_quest_wellf(const ID sess_id, unsigned int rank, bool wellf)
      throw(cmlib::Exception);
  // judge question interestingness
  virtual void mark_quest_inter(const ID sess_id, unsigned int rank, bool inter)
      throw(cmlib::Exception);
  // judge question relevance
  virtual void mark_quest_relev(const ID sess_id, unsigned int rank, bool relev)
      throw(cmlib::Exception);
  // check if document is already indexed
  virtual bool check_doc_indexed(const ID docID)
      throw(cmlib::Exception);
  // add document to index
  virtual ID add_new_doc(const std::string &path)
      throw(cmlib::Exception);
  // mark document as indexed
  virtual void mark_doc_indexed(const ID docID, unsigned int word_count)
      throw(cmlib::Exception);
  // add new sentence
  virtual ID add_new_sentence(const ID docID, const char *sent)
      throw(cmlib::Exception);
  // get ID of the last instance ID
  virtual ID get_last_inst_id()
      throw(cmlib::Exception);
  // add new instance
  virtual void add_instance(const InstInfo& inst)
      throw(cmlib::Exception);
  // add new term
  virtual ID add_new_term(const std::string& term)
      throw(cmlib::Exception);
  // clear stored document sentences
  virtual void clear_doc_sents(const ID docID)
      throw(cmlib::Exception);
  // get document ID by document path
  virtual ID get_doc_id_for_path(const std::string &path)
      throw(cmlib::Exception);
  // set the type of a query
  virtual void set_query_type(const ID sessID, const unsigned int qtype)
      throw(cmlib::Exception);

protected:
  const char *_server;
  const char *_db;
  const char *_user;
  const char *_passwd;
  mysqlpp::Connection *_conn;
  mysqlpp::Query* _query;
};

#endif /* _dbindexio_hpp_ */
