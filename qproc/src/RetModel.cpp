/* -*- C++ -*- */

// $Id: RetModel.cpp,v 1.14 2010/04/24 21:30:02 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <RetModel.hpp>

RetModel::RetModel(const char *host, const char *db, const char *user, const char *pass,
                   const char *patf_path, const char *codep_path, const char *stopw_path,
                   double k1, double b, double k3) throw (cmlib::Exception) {
  unsigned int tot_doc_cnt, tot_doc_length;
  const char *err_msg;
  _index = new DBIndexIO(host, db, user, pass);
  if(!_index->init(&err_msg)) {
    delete _index;
    _index = NULL;
    cmlib_throw("initializing retrieval model", std::string("can't connect to the database - ") + err_msg);
  }
  _okapi_k1 = k1;
  _okapi_b = b;
  _okapi_k3 = k3;
  _index->get_col_stats(tot_doc_cnt, tot_doc_length);
  _avg_doc_length = tot_doc_length / (double) tot_doc_cnt;
  _col_term_cnt = _index->get_col_vocab_size();
  read_pat_file(patf_path, codep_path);
  if(stopw_path != NULL)
    read_stopw_file(stopw_path);
}

RetModel::~RetModel() {
  if(_index != NULL) {
    delete _index;
    _index = NULL;
  }
}

void RetModel::read_pat_file(const char *qpat_path, const char *codep_path) throw(cmlib::Exception) {
  cmlib::SmlNode patterns;
  cmlib::SmlNode *pattern = NULL;
  cmlib::SmlNode *questions = NULL;
  cmlib::SmlNode *question = NULL;
  const char *pid;
  unsigned int pat_id;

  std::string full_codep_path = cmlib::makeFullString(codep_path);
  cmlib::SMLinitEncodingHandler(full_codep_path.c_str());
  std::string full_qpat_path = cmlib::makeFullString(qpat_path);
  cmlib::XMLreadFile(full_qpat_path.c_str(), patterns, true);

  for(unsigned int p = 0; p < patterns.nodeCount(); p++) {
    pattern = patterns.getChildNode(p);
    pid = pattern->getAttr("id");
    pat_id = atoi(pid);
    questions = pattern->getChildNodeByName("questions");

    for(unsigned int q = 0; q < questions->nodeCount(); q++) {
      question = questions->getChildNode(q);

      if(!question->hasAttr("text")) {
        cmlib_throw("reading configuration file", std::string("question in pattern ") + pid + " doesn't have the attribute 'text'.");
      } else if(!question->hasAttr("focus")) {
        cmlib_throw("reading configuration file", std::string("question in pattern ") + pid + " doesn't have the attribute 'focus'.");
      } else {
        _pat_qmap.insert(std::make_pair(pat_id, new QTemplate(question->getAttr("text"), atoi(question->getAttr("focus")))));
      }
    }
  }
}

void RetModel::read_stopw_file(const char *stopw_path) throw(cmlib::Exception) {
  char line_buf[256];
  std::string line;
  std::ifstream file;
  file.open(stopw_path);
  if(!file.is_open()) {
    cmlib_throw("initializing retrieval model", "can't open file with stop words");
  }
  while(!file.eof()) {
    file.getline(line_buf, 256);
    line = line_buf;
    cmlib::strip(line);
    _stop_words.insert(line);
  }
  file.close();
}

double RetModel::score_doc(const ID doc_id, const IdVec& term_ids) throw() {
  cCntMapIt it;
  double score = 0.0;
  ID term_id;
  unsigned int term_col_cnt, term_doc_cnt;
  unsigned int doc_length = _index->get_doc_length(doc_id);

  for(unsigned int i = 0; i < term_ids.size(); i++) {
    term_id = term_ids[i];
    term_col_cnt = _index->get_term_col_count(term_id);
    term_doc_cnt = _index->get_term_doc_count(term_id, doc_id);
    score += log((_col_term_cnt-term_col_cnt+0.5)/((double)term_col_cnt+0.5))*((_okapi_k1+1)*term_doc_cnt)/(_okapi_k1*((1-_okapi_b)+_okapi_b*_avg_doc_length/doc_length)+term_doc_cnt);
  }
  return score;
}

QuestRankedList* RetModel::search(const StrSet &qterms, unsigned int num_quest) throw(cmlib::Exception) {
  ID slotID, patID, instID, termID, docID;
  Question quest;
  const SlotMap *inst_smap = NULL;
  cSlotMapIt ism_it;
  cSlotIt slot_it;
  QTemplate *quest_ptr = NULL;
  QuestTemplIt pqm_lb, pqm_ub, pqm_it;
  // cache for document-query scores
  ScoreMap doc_score_cache;
  cScoreMapIt score_cache_it;
  unsigned int qterms_cnt, match_cnt, focus;
  double doc_score;

  IdVec qtermIDs;
  IdVecMap inst_vecs;
  IdVec *insts=NULL, *inter=NULL;

  for(cStrSetIt it = qterms.begin(); it != qterms.end(); it++) {
    termID = _index->get_term_id(*it);
    // termID is 0 for unknown terms
    if(termID != 0) {
      insts = _index->get_stem_inst_vec(termID);
      inst_vecs[termID] = insts;
      qtermIDs.push_back(termID);
    }
  }

  QuestRankedList *rquest_list = new QuestRankedList(num_quest);

  if(!qtermIDs.size())
    return rquest_list;

  IdPowSet *ps = powerset(qtermIDs);

  // we are going from large cardinality sets (more query terms) to smaller cardinality ones
  for (rIdPowSetIt it_ps = ps->rbegin(); it_ps != ps->rend(); it_ps++) {
    match_cnt = it_ps->first;
    quest.set_match_cnt(match_cnt);
    for (cVecOfIdVecsIt it_vv = it_ps->second.begin(); it_vv
        != it_ps->second.end(); it_vv++) {
      // getting IDs of instances matching a vector of query terms
      inter = term_inst_vec_inter(*it_vv, inst_vecs);

      if (inter->size() != 0) {
        for (unsigned int i = 0; i < inter->size(); i++) {
          instID = inter->at(i);
          quest.set_inst_id(instID);
          patID = _index->get_pat_id_for_inst(instID);
          docID = _index->get_doc_id_for_inst(instID);
          quest.set_doc_id(docID);
          if ((score_cache_it = doc_score_cache.find(docID)) != doc_score_cache.end()) {
            doc_score = doc_score_cache[docID];
            quest.set_doc_score(doc_score_cache[docID]);
          } else {
            doc_score = score_doc(docID, qtermIDs);
            doc_score_cache[docID] = doc_score;
            quest.set_doc_score(doc_score);
          }
          inst_smap = _index->get_slot_map_for_inst(instID);
          pqm_lb = _pat_qmap.lower_bound(patID);
          pqm_ub = _pat_qmap.upper_bound(patID);
          for (pqm_it = pqm_lb; pqm_it != pqm_ub; pqm_it++) {
            StrSet fslot_terms;
            quest_ptr = pqm_it->second;
            focus = quest_ptr->get_focus();
            ism_it = inst_smap->find(focus);

            for (slot_it = ism_it->second.begin(); slot_it
                != ism_it->second.end(); slot_it++) {
              fslot_terms.insert(slot_it->second.stem);
            }
            if (!sets_intersect(fslot_terms, qterms))
              continue;
            qterms_cnt = 0;
            for (ism_it = inst_smap->begin(); ism_it != inst_smap->end(); ism_it++) {
              slotID = ism_it->first;
              if (quest_ptr->has_slot(slotID)) {
                if (quest_ptr->get_slot_type(slotID) == QTemplate::ST_TERM) {
                  slot_it = ism_it->second.begin();
                  quest_ptr->fill_slot_term(slotID, slot_it->second.term);
                  if (set_has_str(qterms, slot_it->second.term))
                    qterms_cnt++;
                } else {
                  for (slot_it = ism_it->second.begin(); slot_it
                      != ism_it->second.end(); slot_it++) {
                    quest_ptr->fill_slot_pos_stem(slotID, slot_it->first,
                        slot_it->second.stem);
                    if (set_has_str(qterms, slot_it->second.stem))
                      qterms_cnt++;
                  }
                }
              }
            }

            quest.set_qterms_cnt(qterms_cnt);
            quest.set_quest(quest_ptr->get_question());
            rquest_list->add(quest);
            quest_ptr->clear_template();
          }
          delete inst_smap;
        }
      }
      delete inter;
    }
  }

  for(IdVecMapIt it = inst_vecs.begin(); it != inst_vecs.end(); it++)
    delete it->second;
  delete ps;

  return rquest_list;
}

void RetModel::new_level(const VecOfIdVecs &old_fringe, VecOfIdVecs &new_fringe,
                          const unsigned int level, const unsigned int num_levels) {
  unsigned int i, j, max_num = 0;
  for(i = 0; i < old_fringe.size(); i++) {
    max_num = old_fringe[i][old_fringe[i].size()-1];
    for(j = max_num+1; j < num_levels; j++) {
      IdVec vec = IdVec(old_fringe[i]);
      vec.push_back(j);
      new_fringe.push_back(vec);
    }
  }
}

IdPowSet* RetModel::powerset(const IdVec &termIds) {
  IdVec index;
  cIdVecIt it;
  VecOfIdVecs vec;
  unsigned int i, j, level, num_levels = termIds.size();

  if(num_levels == 0)
    return NULL;

  IdPowSet *pset = new IdPowSet();
  VecOfIdVecs vv;

  for(i = 0, it = termIds.begin(); it != termIds.end(); it++, i++) {
    index.push_back(*it);
    vec.push_back(IdVec(1, i));
    IdVec v;
    v.push_back(*it);
    vv.push_back(v);
  }

  pset->insert(std::make_pair(1, vv));

  VecOfIdVecs &oldf = vec;

  for(level = 2; level <= num_levels; level++) {
    VecOfIdVecs newf;
    new_level(oldf, newf, level, num_levels);
    VecOfIdVecs vv;
    for(i = 0; i < newf.size(); i++) {
      IdVec v;
      for(j = 0; j < newf[i].size(); j++)
        v.push_back(index[newf[i][j]]);
      vv.push_back(v);
    }
    pset->insert(std::make_pair(level, vv));
    oldf = newf;
  }

  return pset;
}

IdVec* RetModel::term_inst_vec_inter(const IdVec &termIds, IdVecMap& termInstVecs) {
  std::map<unsigned int, unsigned int> cntMap;
  std::map<unsigned int, unsigned int>::iterator it;
  IdVec *inter = new IdVec();

  unsigned int num_vecs = termIds.size();

  if(!num_vecs) {
    return inter;
  }

  for(unsigned int i = 0; i < num_vecs; i++) {
    for(unsigned int j = 0; j < termInstVecs[termIds[i]]->size(); j++) {
      it = cntMap.find(termInstVecs[termIds[i]]->at(j));
      if(it != cntMap.end())
        it->second += 1;
      else
        cntMap[termInstVecs[termIds[i]]->at(j)] = 1;
    }
  }

  for(it = cntMap.begin(); it != cntMap.end(); it++) {
    if(it->second >= num_vecs) {
      inter->push_back(it->first);
    }
  }

  return inter;
}

bool RetModel::sets_intersect(const StrSet& set1, const StrSet& set2) {
  cStrSetIt lgs_it, sms_it;

  if(set1.size() >= set2.size()) {
    for(lgs_it = set1.begin(); lgs_it != set1.end(); lgs_it++) {
      if((sms_it = set2.find(*lgs_it)) != set2.end()) {
        return true;
      }
    }
  } else {
    for(lgs_it = set2.begin(); lgs_it != set2.end(); lgs_it++) {
      if((sms_it = set1.find(*lgs_it)) != set1.end()) {
        return true;
      }
    }
  }

  return false;
}

ID RetModel::create_new_session(const std::string& query, bool fback_sess) throw(cmlib::Exception) {
  return _index->create_new_session(query, fback_sess);
}

void RetModel::set_query_type(const unsigned int sess_id, const unsigned int type) throw(cmlib::Exception) {
  _index->set_query_type(sess_id, type);
}

void RetModel::add_new_question(const QuestInfo &qinfo) throw(cmlib::Exception) {
  _index->add_new_question(qinfo);
}

const QuestMap* RetModel::get_session_questions(const ID sess_id, const unsigned int beg_rank, const unsigned int end_rank)
   throw(cmlib::Exception) {
  return _index->get_session_questions(sess_id, beg_rank, end_rank);
}

std::string RetModel::get_session_query(const ID sess_id) throw(cmlib::Exception) {
  return _index->get_session_query(sess_id);
}

std::string RetModel::get_expanded_query(const ID sess_id, const ID inst_id) throw(cmlib::Exception) {
  Slot slot;
  ID slot_id;
  unsigned int i;
  StrSet ex_query_terms;
  std::string ex_query="";
  std::string orig_query = _index->get_session_query(sess_id);
  StrVec* qterms = cmlib::parse_strtok_vec(orig_query, ' ');
  for(i = 0; i < qterms->size(); i++)
     ex_query_terms.insert(qterms->at(i));
  delete qterms;
  const SlotMap *sm = _index->get_slot_map_for_inst(inst_id);

  for(cSlotMapIt sm_it = sm->begin(); sm_it != sm->end(); sm_it++) {
    slot_id = sm_it->first;
    for(cSlotIt sl_it = sm_it->second.begin(); sl_it != sm_it->second.end(); sl_it++) {
      const std::string &term = sl_it->second.stem;
      if(_stop_words.find(term) == _stop_words.end())
        ex_query_terms.insert(sl_it->second.stem);
    }
  }

  unsigned int size = ex_query_terms.size();
  StrSetIt it;
  for(i = 0, it = ex_query_terms.begin(); it != ex_query_terms.end(); i++, it++) {
     ex_query += *it;
     if(i != size-1) {
        ex_query += std::string("+");
     }
  }
  delete sm;
  return ex_query;
}

ID RetModel::get_quest_id(const ID sess_id, const unsigned int quest_rank) throw(cmlib::Exception) {
  return _index->get_quest_id(sess_id, quest_rank);
}

const SlotMap* RetModel::get_slot_map_for_inst(const ID inst_id) throw(cmlib::Exception) {
  return _index->get_slot_map_for_inst(inst_id);
}

const StrVec* RetModel::get_inst_context(const ID inst_id) throw(cmlib::Exception) {
  return _index->get_inst_context(inst_id);
}

void RetModel::mark_quest_wellf(const ID sess_id, const ID quest_rank) throw(cmlib::Exception) {
  _index->mark_quest_wellf(sess_id, quest_rank, true);
}

void RetModel::mark_quest_inter(const ID sess_id, const ID quest_rank) throw(cmlib::Exception) {
  _index->mark_quest_inter(sess_id, quest_rank, true);
}

void RetModel::mark_quest_relev(const ID sess_id, const ID quest_rank) throw(cmlib::Exception) {
  _index->mark_quest_relev(sess_id, quest_rank, true);
}

void RetModel::mark_quest_shown(const ID quest_id) throw(cmlib::Exception) {
  _index->mark_quest_shown(quest_id);
}

void RetModel::mark_quest_clicked(const ID quest_id) throw(cmlib::Exception) {
  _index->mark_quest_clicked(quest_id, true);
}

bool RetModel::check_quest_clicked(const ID quest_id) throw(cmlib::Exception) {
  return _index->check_quest_clicked(quest_id);
}

bool RetModel::is_stopword(const std::string& term) {
  return _stop_words.find(term) != _stop_words.end();
}




