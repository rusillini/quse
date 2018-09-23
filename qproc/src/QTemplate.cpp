/* -*- C++ -*- */

// $Id: QTemplate.cpp,v 1.4 2010/04/24 21:30:03 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <QTemplate.hpp>

QTemplate::QTemplate(std::string& qtempl, const unsigned int focus) throw (cmlib::Exception) {
  _focus = focus;
  parse_template(qtempl);
}

QTemplate::QTemplate(const char* qtempl, const unsigned int focus) throw (cmlib::Exception) {
  _focus = focus;
  parse_template(qtempl);
}

bool QTemplate::has_slot(const ID slotID) throw() {
  return (_slot_info_map.find(slotID) != _slot_info_map.end());
}

QTemplate::SlotType QTemplate::get_slot_type(const ID slotID) throw() {
  cSlotInfoMapIt it = _slot_info_map.find(slotID);
  return it->second.type;
}

void QTemplate::fill_slot_pos(const ID slotID, const ID posID, const SlotPos& spos) throw() {
  cSlotInfoMapIt it = _slot_info_map.find(slotID);
  if(it == _slot_info_map.end())
    return;
  const SlotInfo &sinfo = it->second;
  if(sinfo.type == ST_TERM) {
    // to avoid multiple repetitions of terms if there is multi-word stem and term is used to fill in a slot
    if(posID == 0) {
      _quest_chunks[sinfo.chunkID].insert(std::make_pair(posID, spos.term));
    }
  } else {
    _quest_chunks[sinfo.chunkID].insert(std::make_pair(posID, spos.stem));
  }
}

void QTemplate::fill_slot_term(const ID slotID, const std::string& term) throw() {
  cSlotInfoMapIt it = _slot_info_map.find(slotID);
  if(it == _slot_info_map.end())
    return;
  const SlotInfo &sinfo = it->second;
  if(sinfo.type == ST_TERM) {
    _quest_chunks[sinfo.chunkID].insert(std::make_pair(0, term));
  }
}

void QTemplate::fill_slot_pos_stem(const ID slotID, const ID posID, const std::string& stem) throw() {
  cSlotInfoMapIt it = _slot_info_map.find(slotID);
  if(it == _slot_info_map.end())
    return;
  const SlotInfo &sinfo = it->second;
  if(sinfo.type == ST_STEM) {
    _quest_chunks[sinfo.chunkID].insert(std::make_pair(posID, stem));
  }
}

bool QTemplate::is_ready() const throw() {
  for(unsigned int i = 0; i < _quest_chunks.size(); i++) {
    if(!_quest_chunks[i].size())
      return false;
  }
  return true;
}

unsigned int QTemplate::get_focus() const throw() {
  return _focus;
}

std::string QTemplate::get_question() throw() {
  std::string quest = "";
  std::string slot_term;
  ChunkMapIt it;
  unsigned int cnt;

  for(unsigned i = 0; i < _quest_chunks.size(); i++) {
    slot_term = "";
    for(cnt = 0, it = _quest_chunks[i].begin(); it != _quest_chunks[i].end(); it++, cnt++) {
      slot_term += it->second;
      if(cnt != _quest_chunks[i].size()-1)
        slot_term += " ";
    }
    quest += slot_term;
    if(i != _quest_chunks.size()-2)
      quest += " ";
  }

  return quest;
}

void QTemplate::clear_template() throw() {
  for(SlotInfoMapIt it = _slot_info_map.begin(); it != _slot_info_map.end(); it++) {
    _quest_chunks[it->second.chunkID].clear();
  }
}

void QTemplate::parse_slot(const std::string &slot, const unsigned int chunkID) throw(cmlib::Exception) {
  ChunkMap cmap;
  std::string::size_type pos = slot.find(':', 0);
  if(pos == std::string::npos) {
    cmlib_throw("parsing question pattern", "invalid type of slot " + slot);
  }
  ID slotID = atoi(slot.substr(0, pos).c_str());
  std::string slot_type = slot.substr(pos+1);
  if(!slot_type.compare("term")) {
    _slot_info_map.insert(std::make_pair(slotID, SlotInfo(ST_TERM, chunkID)));
    _quest_chunks.push_back(cmap);
  } else if(!slot_type.compare("stem")) {
    _slot_info_map.insert(std::make_pair(slotID, SlotInfo(ST_STEM, chunkID)));
    _quest_chunks.push_back(cmap);
  } else {
    cmlib_throw("parsing question pattern", slot_type + " - invalid slot type");
  }
}

void QTemplate::parse_template(const std::string &qtempl) throw (cmlib::Exception) {
  std::string chunk;
  unsigned int chunk_num = 0;
  std::string::size_type lpos = 0, rpos = 0;
  while((lpos = qtempl.find('{', rpos)) != std::string::npos) {
    ChunkMap cmap;
    chunk = qtempl.substr(rpos, lpos-rpos);
    cmlib::strip(chunk);
    if(chunk.length()) {
      cmap.insert(std::make_pair(0, chunk));
      _quest_chunks.push_back(cmap);
      chunk_num = chunk_num + 1;
    }
    if((rpos = qtempl.find('}', ++lpos)) == std::string::npos) {
      cmlib_throw("parsing question pattern", "unmatched open bracket found");
    } else {
      parse_slot(qtempl.substr(lpos, rpos-lpos), chunk_num);
      chunk_num = chunk_num + 1;
      rpos = rpos + 1;
    }
  }

  if(rpos <= qtempl.length()-1) {
    ChunkMap cmap;
    chunk = qtempl.substr(rpos);
    cmlib::strip(chunk);
    if(chunk.length()) {
      cmap.insert(std::make_pair(0, chunk));
      _quest_chunks.push_back(cmap);
    }
  }
}

#ifdef _DEBUG

void QTemplate::print_quest_chunks() {
  for(unsigned int i = 0; i < _quest_chunks.size(); i++) {
    const ChunkMap &cmap = _quest_chunks.at(i);
    printf("Chunk %u:\n", i);
    for(cChunkMapIt it = cmap.begin(); it != cmap.end(); it++) {
      printf("  Pos: %u Word: %s\n", it->first, it->second.c_str());
    }
  }
}

void QTemplate::print_slot_info() {
  for(SlotInfoMapIt it = _slot_info_map.begin(); it != _slot_info_map.end(); it++) {
    printf("Slot %u: type=%s chunk=%u\n", it->first, (it->second.type == ST_TERM ? "term" : "stem"), it->second.chunkID);
  }
}

#endif
