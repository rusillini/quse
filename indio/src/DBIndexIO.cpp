/* -*- C++ -*- */

// $Id: DBIndexIO.cpp,v 1.19 2009/10/27 20:55:34 akotov2 Exp $

#include <DBIndexIO.hpp>

DBIndexIO::DBIndexIO(const char *server, const char *db, const char *user, const char *passwd) {
  _server = server;
  _db = db;
  _user = user;
  _passwd = passwd;
  _conn = new mysqlpp::Connection(false);
  _query = NULL;
}

DBIndexIO::~DBIndexIO() {
  if(_query != NULL) {
    delete _query;
    _query = NULL;
  }

  if(_conn != NULL) {
    if(_conn->connected())
      _conn->disconnect();
    delete _conn;
    _conn = NULL;
  }
}

bool DBIndexIO::init(const char **msg) throw() {
  if(!_conn->connect(_db, _server, _user, _passwd)) {
    *msg = _conn->error();
    return false;
  } else {
    *msg = NULL;
    _query = new mysqlpp::Query(_conn, false);
    return true;
  }
}

void DBIndexIO::connect() throw(cmlib::Exception) {
  if(_conn->connect(_db, _server, _user, _passwd)) {
    if(_query != NULL) {
      _query = new mysqlpp::Query(_conn, false);
    }
  } else {
    cmlib_throw("connecting to database", _conn->error());
  }
}

void DBIndexIO::close() throw() {
  if(_conn->connected())
    _conn->disconnect();
}

void DBIndexIO::get_col_stats(unsigned int &doc_cnt, unsigned int &doc_size) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT COUNT(id), SUM(word_count) FROM documents";
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    if(res.num_rows()) {
      doc_cnt = atoi(res[0][0]);
      doc_size = atoi(res[0][1]);
    } else {
      doc_cnt = 0;
      doc_size = 0;
    }
  } else {
    cmlib_throw("obtaining collection statistics", _query->error());
  }
}

IdVec* DBIndexIO::get_term_inst_vec(const ID termId) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT DISTINCT(inst_id) FROM instances WHERE term_id=" << termId;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    IdVec *instIds = new IdVec();
    for(unsigned int i = 0; i < res.num_rows(); i++) {
      instIds->push_back(atoi(res[i][0].c_str()));
    }
    return instIds;
  } else {
    cmlib_throw("searching for instances of a term", _query->error());
  }
}

IdVec* DBIndexIO::get_stem_inst_vec(const ID stemId) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT DISTINCT(inst_id) FROM instances WHERE stem_id=" << stemId;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    IdVec *instIds = new IdVec();
    for(unsigned int i = 0; i < res.num_rows(); i++) {
      instIds->push_back(atoi(res[i][0].c_str()));
    }
    return instIds;
  } else {
    cmlib_throw("searching for instances of a stem", _query->error());
  }
}

const StrVec* DBIndexIO::get_inst_context(const ID instID) throw(cmlib::Exception) {
  ID sent_id, doc_id;
  StrVec *cont = new StrVec();

  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT doc_id, sent_id FROM instances WHERE inst_id=" << instID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    doc_id = res.num_rows() ? atoi(res[0][0].c_str()) : 0;
    sent_id = res.num_rows() ? atoi(res[0][1].c_str()) : 0;
  } else {
    delete cont;
    cmlib_throw("(fetching context of an instance) looking up a document where an instance occurred", _query->error());
  }

  if(sent_id != 0) {
    mysqlpp::StoreQueryResult r;
    *_query << "SELECT sentence FROM sentences WHERE doc_id=" << doc_id << " AND sent_id=" << sent_id-1;
    r = _query->store();
    if(r.num_rows())
      cont->push_back(r[0][0].c_str());
    *_query << "SELECT sentence FROM sentences WHERE doc_id=" << doc_id << " AND sent_id=" << sent_id;
    r = _query->store();
    if(r.num_rows())
      cont->push_back(r[0][0].c_str());
    *_query << "SELECT sentence FROM sentences WHERE doc_id=" << doc_id << " AND sent_id=" << sent_id+1;
    r = _query->store();
    if(r.num_rows())
      cont->push_back(r[0][0].c_str());
  } else {
    delete cont;
    cmlib_throw("() looking up sentence where an instance occurred", _query->error());
  }

  return cont;
}

unsigned int DBIndexIO::get_col_vocab_size() throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT COUNT(id) FROM dictionary";
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return atoi(res[0][0].c_str());
  } else {
    cmlib_throw("looking up collection vocabulary size", _query->error());
  }
}

ID DBIndexIO::get_term_id(const std::string& term) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT id FROM dictionary WHERE term = " << mysqlpp::quote << term;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
   return res.num_rows() ? atoi(res[0][0].c_str()) : 0;
  } else {
   cmlib_throw("looking up a term ID in the dictionary", _query->error());
  }
}

std::string DBIndexIO::get_term_for_id(const ID termID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT term FROM dictionary WHERE id=" << termID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? "" : res[0][0];
  } else {
    cmlib_throw("looking up a term in the dictionary", _query->error());
  }
}

unsigned int DBIndexIO::get_term_doc_count(const ID termID, const ID docID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT COUNT(doc_id) FROM instances WHERE stem_id=" << termID << " AND doc_id=" << docID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? 0 : atoi(res[0][0].c_str());
  } else {
    cmlib_throw("getting term count in a document", _query->error());
  }
}

unsigned int DBIndexIO::get_doc_length(const ID docID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT word_count FROM documents WHERE id=" << docID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? 0 : atoi(res[0][0].c_str());
  } else {
    cmlib_throw("getting document length", _query->error());
  }
}

unsigned int DBIndexIO::get_term_col_count(const ID termID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT COUNT(inst_id) FROM instances WHERE stem_id=" << termID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? 0 : atoi(res[0][0].c_str());
  } else {
    cmlib_throw("getting term collection count", _query->error());
  }
}

ID DBIndexIO::get_doc_id_for_inst(const ID instID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT doc_id FROM instances WHERE inst_id=" << instID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? 0 : atoi(res[0][0].c_str());
  } else {
    cmlib_throw("searching document with instance", _query->error());
  }
}

ID DBIndexIO::get_doc_id_for_sent(const ID sentID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT doc_id FROM sentences WHERE sent_id=" << sentID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? 0 : atoi(res[0][0].c_str());
  } else {
    cmlib_throw("searching document with sentence", _query->error());
  }
}

ID DBIndexIO::get_sent_id_for_inst(const ID instID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT sent_id FROM instances WHERE inst_id=" << instID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? 0 : atoi(res[0][0].c_str());
  } else {
    cmlib_throw("searching sentence with instance", _query->error());
  }
}

ID DBIndexIO::get_pat_id_for_inst(const ID instID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT DISTINCT(pat_id) FROM instances WHERE inst_id=" << instID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return atoi(res[0][0].c_str());
  } else {
    cmlib_throw("searching pattern of an instance", _query->error());
  }
}

const SlotMap* DBIndexIO::get_slot_map_for_inst(const ID instID) throw(cmlib::Exception) {
  std::string term, stem;
  unsigned int slotID, slot_pos, termID, stemID;
  SlotMapIt smap_it;

  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT slot_id, slot_pos, term_id, stem_id FROM instances WHERE inst_id=" << instID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    SlotMap *smap = new SlotMap();
    for(unsigned int i=0; i < res.num_rows(); i++) {
      slotID = atoi(res[i]["slot_id"].c_str());
      slot_pos = atoi(res[i]["slot_pos"].c_str());
      termID = atoi(res[i]["term_id"].c_str());
      term = get_term_for_id(termID);
      stemID = atoi(res[i]["stem_id"].c_str());
      stem = get_term_for_id(stemID);
      smap_it = smap->find(slotID);
      if(smap_it == smap->end()) {
        Slot slot;
        slot.insert(std::make_pair(slot_pos, SlotPos(term, stem)));
        smap->insert(std::make_pair(slotID, slot));
      } else {
        smap_it->second.insert(std::make_pair(slot_pos, SlotPos(term, stem)));
      }
    }
    return smap;
  } else {
    cmlib_throw("getting pattern slots", _query->error());
  }
}

bool DBIndexIO::session_exists(const ID sessID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT id FROM sessions WHERE id=" << sessID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return res.num_rows();
  } else {
    cmlib_throw("checking if session already exists", _query->error());
  }
}

ID DBIndexIO::create_new_session(const std::string &query, bool fb) throw(cmlib::Exception) {
  if(!_conn->connected()) {
     cmlib_throw("creating new session", "not connected to database");
  }

  *_query << "INSERT INTO sessions(query, fb) VALUES(" << mysqlpp::quote << query << "," << (fb ? "'1'" : "'0'") << ")";
  if(_query->exec()) {
    *_query << "SELECT LAST_INSERT_ID()";
    if(mysqlpp::StoreQueryResult res = _query->store()) {
      return atoi(res[0][0].c_str());
    } else {
      cmlib_throw("obtaining ID of newly added session", _query->error());
    }
  } else {
    cmlib_throw("adding new session", _query->error());
  }
}

void DBIndexIO::add_new_question(const QuestInfo& qinfo) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "INSERT INTO questions(inst_id,sess_id,qterms,rank,matches,origq_id,score,quest) VALUES(" << qinfo.inst_id << ",";
  *_query << qinfo.sess_id << "," << qinfo.qterms << "," << qinfo.rank << "," << qinfo.num_matches << "," << qinfo.origq_id << "," << qinfo.score << "," << mysqlpp::quote << qinfo.quest << ")";
  if(!_query->exec()) {
    std::string loc = std::string("adding new question ") + qinfo.quest;
    cmlib_throw(loc, _query->error());
  }
}

ID DBIndexIO::get_quest_id(const ID sessID, unsigned int rank) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT id FROM questions WHERE sess_id=" << sessID << " AND rank=" << rank;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return atoi(res[0][0].c_str());
  } else {
    cmlib_throw("getting absolute question ID", _query->error());
  }
}

const QuestMap* DBIndexIO::get_session_questions(const ID sess_id, const unsigned int beg_rank, const unsigned int end_rank)
   throw(cmlib::Exception)
{
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT * FROM questions WHERE sess_id=" << sess_id << " AND rank BETWEEN " << beg_rank << " AND " << end_rank;
  if(mysqlpp::UseQueryResult res = _query->use()) {
    QuestMap *qmap = new QuestMap();
    while(mysqlpp::Row row = res.fetch_row()) {
      QuestInfo qinfo;
      unsigned int rank = atoi(row["rank"].c_str());
      qinfo.inst_id = atoi(row["inst_id"].c_str());
      qinfo.sess_id = atoi(row["sess_id"].c_str());
      qinfo.score = atof(row["score"].c_str());
      qinfo.quest = row["quest"].c_str();
      qinfo.shown = !row["shown"].compare("0") ? false : true;
      qinfo.clicked = !row["clicked"].compare("0") ? false : true;
      qinfo.wellf = !row["wellf"].compare("0") ? false : true;
      qinfo.inter = !row["inter"].compare("0") ? false : true;
      qinfo.relev = !row["relev"].compare("0") ? false : true;
      qmap->insert(std::make_pair(rank, qinfo));
    }
    return qmap;
  } else {
    cmlib_throw("getting session questions", _query->error());
  }
}

std::string DBIndexIO::get_session_query(const ID sessID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT query FROM sessions WHERE id=" << sessID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return res[0][0].c_str();
  } else {
    cmlib_throw("getting session query", _query->error());
  }
}

QueryStatsVec* DBIndexIO::get_query_suggestions() throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  QueryStatsVec *qsug = new QueryStatsVec();
  *_query << "SELECT * FROM suggestions";
  if(mysqlpp::UseQueryResult res = _query->use()) {
    while(mysqlpp::Row row = res.fetch_row())
      qsug->push_back(QueryStats(row["query"].c_str(), atoi(row["type"].c_str()), 0));
  } else {
    delete qsug;
    cmlib_throw("getting query suggestions", _query->error());
  }

  for(unsigned int i = 0; i < qsug->size(); i++) {
    *_query << "SELECT COUNT(id) FROM sessions WHERE query=" << mysqlpp::quote << qsug->at(i).query;
    if(mysqlpp::StoreQueryResult res_cnt = _query->store()) {
      qsug->at(i).count = atoi(res_cnt[0][0].c_str());
    } else {
      delete qsug;
      cmlib_throw("getting session counts for query suggestions", _query->error());
    }
  }

  return qsug;
}


void DBIndexIO::mark_quest_shown(const ID quest_id) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "UPDATE questions SET shown='1' WHERE id=" << quest_id;
  if(!_query->exec()) {
    cmlib_throw("marking questions as shown", _query->error());
  }
}

void DBIndexIO::mark_quest_clicked(const ID quest_id, bool clicked) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "UPDATE questions SET clicked='1' WHERE id=" << quest_id;
  if(!_query->exec()) {
    cmlib_throw("marking question as clicked", _query->error());
  }
}

void DBIndexIO::mark_quest_clicked(const ID sess_id, const unsigned int rank) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "UPDATE questions SET clicked='1' WHERE sess_id=" << sess_id << " AND rank=" << rank;
  if (!_query->exec()) {
    cmlib_throw("marking question as clicked", _query->error());
  }
}

bool DBIndexIO::check_quest_clicked(const ID questID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT clicked FROM questions WHERE id=" << questID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return atoi(res[0][0].c_str());
  } else {
    cmlib_throw("checking if a question is clicked", _query->error());
  }
}

void DBIndexIO::mark_quest_wellf(const ID sess_id, const unsigned int rank, bool wellf) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  std::string new_val = wellf ? "1" : "0";
  *_query << "UPDATE questions SET wellf='" << new_val << "' WHERE sess_id=" << sess_id << " AND rank=" << rank;
  if (!_query->exec()) {
    cmlib_throw("marking questions as well-formed", _query->error());
  }
}

void DBIndexIO::mark_quest_inter(const ID sess_id, const unsigned int rank, bool inter) throw(cmlib::Exception) {
  if(!_conn->ping()) {
      connect();
  }

  std::string new_val = inter ? "1" : "0";
  *_query << "UPDATE questions SET inter='" << new_val << "' WHERE sess_id=" << sess_id << " AND rank=" << rank;
  if (!_query->exec()) {
    cmlib_throw("marking questions as shown", _query->error());
  }
}

void DBIndexIO::mark_quest_relev(const ID sess_id, const unsigned int rank, bool relev) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  std::string new_val = relev ? "1" : "0";
  *_query << "UPDATE questions SET relev='" << new_val << "' WHERE sess_id=" << sess_id << " AND rank=" << rank;
  if (!_query->exec()) {
    cmlib_throw("marking questions as relevant", _query->error());
  }
}

bool DBIndexIO::check_doc_indexed(const ID docID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT indexed FROM documents WHERE id=" << docID;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return atoi(res[0][0].c_str());
  } else {
    cmlib_throw("checking if document is indexed", _query->error());
  }
}

ID DBIndexIO::add_new_doc(const std::string &path) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "INSERT INTO documents(path) VALUES(" << mysqlpp::quote << path << ")";
  if(_query->exec()) {
    *_query << "SELECT LAST_INSERT_ID()";
    if(mysqlpp::StoreQueryResult res = _query->store()) {
      return atoi(res[0][0].c_str());
    } else {
      cmlib_throw("obtaining ID of a newly added document", _query->error());
    }
  } else {
    cmlib_throw("adding new document", _query->error());
  }

  return 0;
}

void DBIndexIO::set_query_type(const ID sessID, const unsigned int qtype) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "UPDATE sessions SET type='" << qtype << "' WHERE id=" << sessID;
  if(!_query->exec()) {
    cmlib_throw("setting query type", _query->error());
  }
}


void DBIndexIO::mark_doc_indexed(const ID docID, unsigned int word_count) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "UPDATE documents SET indexed='1', word_count=" << word_count << " WHERE id=" << docID;
  if (!_query->exec()) {
    cmlib_throw("marking document as indexed", _query->error());
  }
}

ID DBIndexIO::add_new_sentence(const ID docID, const char *sent) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "INSERT INTO sentences(doc_id,sentence) VALUES(" << docID << "," << mysqlpp::quote << sent << ")";
  if(_query->exec()) {
    *_query << "SELECT LAST_INSERT_ID()";
    if(mysqlpp::StoreQueryResult res = _query->store()) {
      return atoi(res[0][0].c_str());
    } else {
      cmlib_throw("obtaining ID of a newly added sentence", _query->error());
    }
  } else {
    cmlib_throw("adding new sentence", _query->error());
  }
}

ID DBIndexIO::get_last_inst_id() throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT MAX(inst_id) FROM instances";
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return res.num_rows() ? atoi(res[0][0].c_str()) : 0;
  } else {
    cmlib_throw("looking up ID of the last instance", _query->error());
  }
}

void DBIndexIO::add_instance(const InstInfo& inst) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "INSERT INTO instances(inst_id,doc_id,sent_id,term_id,stem_id,pat_id,slot_id,slot_pos) VALUES(" << inst.inst_id << "," << inst.doc_id << "," << inst.sent_id << "," << inst.term_id << "," << inst.stem_id << "," << inst.pat_id << "," << inst.slot_id << "," << inst.slot_pos << ")";
  if(!_query->exec()) {
    cmlib_throw("adding new instance", _query->error());
  }
}

ID DBIndexIO::add_new_term(const std::string& term) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "INSERT INTO dictionary(term) VALUES(" << mysqlpp::quote << term << ")";
  if(_query->exec()) {
    *_query << "SELECT LAST_INSERT_ID()";
    if(mysqlpp::StoreQueryResult res = _query->store()) {
      return atoi(res[0][0].c_str());
    } else {
      cmlib_throw("obtaining ID of a newly added term", _query->error());
    }
  } else {
    cmlib_throw("adding new term to dictionary", _query->error());
  }
}

void DBIndexIO::clear_doc_sents(const ID docID) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "DELETE FROM sentences WHERE doc_id=" << docID;
  if(!_query->exec()) {
    cmlib_throw("deleting stored document sentences", _query->error());
  }
}

ID DBIndexIO::get_doc_id_for_path(const std::string &path) throw(cmlib::Exception) {
  if(!_conn->ping()) {
    connect();
  }

  *_query << "SELECT id FROM documents WHERE path=" << mysqlpp::quote << path;
  if(mysqlpp::StoreQueryResult res = _query->store()) {
    return !res.num_rows() ? 0 : atoi(res[0][0].c_str());
  } else {
    cmlib_throw("looking up a term in the dictionary", _query->error());
  }
}
