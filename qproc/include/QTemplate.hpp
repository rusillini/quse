/* -*- C++ -*- */

// $Id: QTemplate.hpp,v 1.4 2010/04/24 21:30:03 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009.

#ifndef _qtemplate_hpp_
#define _qtemplate_hpp_

#include <utility>
#include <Types.hpp>
#include <except.h>
#include <strutils.h>
#include <stdlib.h>

/*
 * Question template, from which question instances are generated. Templates consist of a set of numbered
 * slots. A slot can consist of multiple terms (e.g, John Fitzgerald Kennedy) and thus have multiple positions
 * within it. In order to generate a question from the template, two conditions must hold:
 * 1. Question should be focused on at least one query term (a query term should be in the slot of the corresponding
 * pattern instance, which is designated by the 'focus' parameter);
 * 2. All slots of the pattern must be filled in with the terms from the slots of the corresponding syntactic
 * pattern.
 */
class QTemplate {
public:
  typedef enum SlotType_ {ST_TERM, ST_STEM} SlotType;

public:
  /*
   * initialize the question template with a string and set the focus parameter from the configuration file
   */
  QTemplate(std::string& qtempl, const unsigned int focus) throw (cmlib::Exception);
  /*
   * initialize the question template with a string and set the focus parameter from the configuration file
   */
  QTemplate(const char* qtempl, const unsigned int focus) throw (cmlib::Exception);

  ~QTemplate()
  { }

  // check if a question template has a slot with designated ID
  bool has_slot(const ID slotID) throw();

  // get the type of the slot with designated ID
  SlotType get_slot_type(const ID slotID) throw();

  // copy the specific position of the specific slot of an instance of syntactic pattern into the
  // same position in the corresponding slot of the question template (if exists);
  // Returns true if the designated slot exists in the question template.
  void fill_slot_pos(const ID slotID, const ID posID, const SlotPos& spos) throw();

  // copy a term in the specific slot of an instance of syntactic pattern into the corresponding
  // slot of the question template (if the slot exists);
  // Returns true if the designated slot exists in the question template, false otherwise.
  void fill_slot_term(const ID slotID, const std::string& term) throw();

  // copy a stem at the specific position of the specific slot of an instance of syntactic pattern
  // into the same position in the corresponding slot of the question template (if exists);
  // Returns true if the designated position of the designated slot exists in the question template,
  // false otherwise.
  void fill_slot_pos_stem(const ID slotID, const ID posID, const std::string& stem) throw();

  // check if all the slots of the template are filled and we can get an instance of the question
  bool is_ready() const throw();

  // get an instance of a question
  std::string get_question() throw();

  // clear the question template
  void clear_template() throw();

  // get the focus of a question
  unsigned int get_focus() const throw();

#ifdef _DEBUG
  // print debug information
  void print_quest_chunks();
  void print_slot_info();
#endif

private:

  typedef struct SlotInfo_ {
    SlotInfo_(SlotType t, const ID cid) : type(t), chunkID(cid)
    { }

    SlotType type;
    ID chunkID;
  } SlotInfo;

  // mapping from question template slot IDs to question chunk IDs
  typedef std::map<ID, SlotInfo> SlotInfoMap;
  typedef SlotInfoMap::iterator SlotInfoMapIt;
  typedef SlotInfoMap::const_iterator cSlotInfoMapIt;

  // mapping from chunk positions to chunk terms
  typedef std::map<unsigned int, std::string> ChunkMap;
  typedef ChunkMap::iterator ChunkMapIt;
  typedef ChunkMap::const_iterator cChunkMapIt;

  // vector of chunk maps
  typedef std::vector<ChunkMap> VecOfChunkMaps;

private:
  void parse_slot(const std::string &slot, const unsigned int slot_num) throw (cmlib::Exception);
  void parse_template(const std::string &qtempl) throw (cmlib::Exception);

private:
  // slots of the question pattern
  SlotInfoMap _slot_info_map;
  // question chunks
  VecOfChunkMaps _quest_chunks;
  // question focus
  unsigned int _focus;
};

typedef std::multimap<unsigned int, QTemplate*> QuestTemplMap;
typedef QuestTemplMap::iterator QuestTemplIt;
typedef QuestTemplMap::const_iterator cQuestTemplIt;

#endif // _qtemplate_hpp_
