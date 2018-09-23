/* -*- C++ -*- */

// $Id: RetModel.hpp,v 1.11 2010/04/24 21:30:03 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009.

#ifndef _retmodel_hpp_
#define _retmodel_hpp_

#include <math.h>
#include <except.h>
#include <envparm.h>
#include <fstream>
#include <smldata.h>
#include <smlxt.h>
#include <Types.hpp>
#include <QTemplate.hpp>
#include <IndexIO.hpp>
#include <DBIndexIO.hpp>
#include <Question.hpp>
#include <QuestRankedList.hpp>

#define OKAPI_K1_DEF 1.2
#define OKAPI_B_DEF 0.75
#define OKAPI_K3_DEF 1000

class RetModel {
public:
  /*
   *  Initialize retrieval model:
   *    * host - host where the database is located;
   *    * db - database with the index;
   *    * user - user name to connect to DB;
   *    * pass - password to connect to DB;
   *    * patf_path - path to the XML file with syntactic patterns;
   *    * codep_path - path to code pages, needed for XML parser;
   *    * stopw_path - path to the list of stop words (NULL by default means no stop words);
   *    * qnum_max - maximum number of questions in the ranked list;
   *    * k1 - parameter k1 in BM25/Okapi retrieval formula;
   *    * b - parameter b in BM25/Okapi retrieval formula;
   *    * k3 - parameter k3 in BM25/Okapi.
   */
  RetModel(const char *host, const char *db, const char *user, const char *pass, const char *patf_path,
           const char *codep_path, const char *stopw_path=NULL, double k1=OKAPI_K1_DEF, double b=OKAPI_B_DEF,
           double k3=OKAPI_K3_DEF) throw(cmlib::Exception);

  virtual ~RetModel();

  /*
   * Process the query and return the ranked list of questions.
   * Input arguments:
   *   * qterms - a set of query terms;
   *   * num_quest - the maximum number of questions returned
   * Output:
   *   * ranked list of questions
   */
  QuestRankedList* search(const StrSet &qterms, unsigned int num_quest) throw(cmlib::Exception);

  // is term a stopword
  bool is_stopword(const std::string& term);

  // create new search session
  ID create_new_session(const std::string& query, bool fback_sess) throw(cmlib::Exception);

  // set query type for a session
  void set_query_type(const unsigned int sess_id, const unsigned int qtype) throw(cmlib::Exception);

  // add new question
  void add_new_question(const QuestInfo &qinfo) throw(cmlib::Exception);

  // get session questions
  const QuestMap* get_session_questions(const ID sess_id, const unsigned int beg_rank, const unsigned int end_rank) throw(cmlib::Exception);

  // get the original, query corresponding to a session
  std::string get_session_query(const ID sess_id) throw(cmlib::Exception);

  // get a session query, augmented with terms from the slots of an instance
  std::string get_expanded_query(const ID sess_id, const ID inst_id) throw(cmlib::Exception);

  // get ID of a question in a session by its session rank
  ID get_quest_id(const ID sess_id, const unsigned int quest_rank) throw(cmlib::Exception);

  // get a slot map for an instance of syntactic pattern
  const SlotMap* get_slot_map_for_inst(const ID inst_id) throw(cmlib::Exception);

  // get the context of an instance
  const StrVec* get_inst_context(const ID inst_id) throw(cmlib::Exception);

  // mark question as well-formed
  void mark_quest_wellf(const ID sess_id, const ID quest_rank) throw(cmlib::Exception);

  // mark question as interesting
  void mark_quest_inter(const ID sess_id, const ID quest_rank) throw(cmlib::Exception);

  // mark question as interesting
  void mark_quest_relev(const ID sess_id, const ID quest_rank) throw(cmlib::Exception);

  // mark question as shown
  void mark_quest_shown(const ID quest_id) throw(cmlib::Exception);

  // mark question as clicked
  void mark_quest_clicked(const ID quest_id) throw(cmlib::Exception);

  // check if a question was clicked before
  bool check_quest_clicked(const ID quest_id) throw(cmlib::Exception);

  // set parameter k1 of BM25/Okapi retrieval formula
  inline void set_okapi_k1(float k1) {
    _okapi_k1 = k1;
  }

  // get parameter k1 of BM25/Okapi retrieval formula
  inline double get_okapi_k1() const {
    return _okapi_k1;
  }

  // set parameter k1 of BM25/Okapi retrieval formula
  inline void set_okapi_b(float b) {
    _okapi_b = b;
  }

  // get parameter k1 of BM25/Okapi retrieval formula
  inline double get_okapi_b() const {
    return _okapi_b;
  }

  // set parameter k1 of BM25/Okapi retrieval formula
  inline void set_okapi_k3(float k3) {
    _okapi_k3 = k3;
  }

  // get parameter k1 of BM25/Okapi retrieval formula
  inline double get_okapi_k3() const {
    return _okapi_k3;
  }

private:
  /* read the file with syntactic patterns and question templates */
  void read_pat_file(const char *qpat_path, const char *codep_path) throw(cmlib::Exception);
  /* read the file with a list of stopwords */
  void read_stopw_file(const char *stopw_path) throw(cmlib::Exception);
  /* score a document with respect to a set of query terms */
  double score_doc(const ID doc_id, const IdVec& term_ids) throw();
  /* get an intersection of vectors of term instances */
  IdVec* term_inst_vec_inter(const IdVec &termIds, IdVecMap& termInstVecs);
  /* check if the intersection of two sets is non-empty */
  bool sets_intersect(const StrSet& set1, const StrSet& set2);
  /* get power set for a set of term IDs */
  IdPowSet* powerset(const IdVec & set);
  /* generate a set of subsets of higher cardinality for superset */
  void new_level(const VecOfIdVecs &old_fringe, VecOfIdVecs &new_fringe,
                   const unsigned int level, const unsigned int num_levels);
  /* check if a string belongs to the set */
  inline bool set_has_str(const StrSet& set, const std::string& str) {
    return set.find(str) != set.end();
  }
private:
  // fetcher
  IndexIO *_index;
  // list of stop words
  StrSet _stop_words;
  // mapping from pattern ID to its question templates
  QuestTemplMap _pat_qmap;
  // Okapi/BM25 parameters
  double _okapi_k1;
  double _okapi_b;
  double _okapi_k3;
  // average length of a documents in the collection
  double _avg_doc_length;
  // total number of terms in the collection
  unsigned int _col_term_cnt;
};

#endif /* _retmodel_hpp_ */
