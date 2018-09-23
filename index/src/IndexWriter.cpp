/* -*- C++ -*- */

// $Id: IndexWriter.cpp,v 1.13 2010/02/10 20:00:42 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <IndexWriter.hpp>

// process ID of the parser
static pid_t _parser_pid;
static volatile sig_atomic_t can_continue = 1;
static volatile sig_atomic_t parser_ready = 0;

void ctrlc_handler(int signo) {
  // making MINIPAR shut down
  kill(_parser_pid, SIGINT);

  // waiting for MINIPAR
  if(_parser_pid != r_wait(NULL))
    perror("(Index) Failed to wait for MINIPAR to close due to signal or error\n");

  can_continue = 0;
}

void usr_handler(int signo) {
  parser_ready = 1;
}

IndexWriter::IndexWriter(const char *conf_file, const char *temp_dir, bool verbose)
{
  std::string path = std::string(temp_dir) + "/" + I2P_FIFO_NAME;
  _i2p_path = cmlib::makeFullString(path.c_str());
  path = std::string(temp_dir) + "/" + P2I_FIFO_NAME;
  _p2i_path = cmlib::makeFullString(path.c_str());
  _sys_conf_path = conf_file;
  _index = 0;
  _verbose = verbose;
  _initialized = false;
  _i2p_fd = 0;
}

IndexWriter::~IndexWriter()
{
  if(_verbose)
    printf("Shutting down...");

  if(_i2p_fd != 0) {
    if((r_close(_i2p_fd) == -1)) {
      perror("Failed to close FIFO descriptors");
    }
  }

  unlink(_i2p_path.c_str());
  unlink(_p2i_path.c_str());

  if(_index != NULL)
    delete _index;

  if(_verbose)
    printf("OK\n");
}

void IndexWriter::init() throw (cmlib::Exception)
{
  // database connection parameters
  std::string host, db_name, user, passwd;
  // path to the codepages file
  std::string codep_path;
  // path to the patterns file
  std::string pat_conf_path;
  // path to the parser data
  std::string parser_data_path;
  // path to the parser
  std::string parser_path;
  // initialization error
  const char *msg;

  if(_verbose)
    printf("Reading system configuration...");

  cmlib::SectionMap *conf = cmlib::read_config_file(_sys_conf_path);

  if(!cmlib::get_param_value(db_name, conf, "db", "db_name") || !cmlib::get_param_value(host, conf, "db", "host") ||
     !cmlib::get_param_value(user, conf, "db", "user") || !cmlib::get_param_value(passwd, conf, "db", "passwd" ) ||
     !cmlib::get_param_value(codep_path, conf, "system", "codepages") || !cmlib::get_param_value(pat_conf_path, conf, "system", "patterns") ||
     !cmlib::get_param_value(parser_data_path, conf, "index", "parser_data") || !cmlib::get_param_value(parser_path, conf, "index", "parser")) {
    cmlib_throw("reading system config", "config is missing required parameter(s)");
  }

  delete conf;

  if(_verbose)
    printf("OK\n");

  if(_verbose)
    printf("Connecting to database...");

  _index = new DBIndexIO(host.c_str(), db_name.c_str(), user.c_str(), passwd.c_str());
  if(!_index->init(&msg)) {
    if(_verbose)
      printf("Failed\n");
    cmlib_throw("can't connect to database", msg);
  }

  if(_verbose)
      printf("OK\n");

  _cur_inst_id = _index->get_last_inst_id();

  if(_verbose)
    printf("Reading patterns file...");

  read_pat_config(pat_conf_path, codep_path);

  if(_verbose)
    printf("OK\n");

  /* Create a FIFO to return parsed sentences from parser to indexer */
  if((mkfifo(_p2i_path.c_str(), P2I_FIFO_PERMS) == -1) && (errno != EEXIST)) {
    cmlib_throw("IndexWriter::init()","Can't create a FIFO from parser to indexer");
  }

  /* Create a FIFO to feed MINIPAR with sentences to parse */
  if((mkfifo(_i2p_path.c_str(), I2P_FIFO_PERMS) == -1) && (errno != EEXIST)) {
    cmlib_throw("IndexWriter::init()","Can't create a FIFO from indexer to parser");
  }

  struct sigaction usr_act, int_act;
  usr_act.sa_handler = usr_handler;
  usr_act.sa_flags = 0;
  if((sigemptyset(&usr_act.sa_mask) == -1) || (sigaction(SIGUSR1, &usr_act, NULL) == -1)) {
    cmlib_throw("IndexWriter::init()", "(IndexWriter) Failed to set SIGUSR1 handler\n");
  }

  /* spanning a new process that will run MINIPAR */
  if((_parser_pid = fork()) == -1) {
    cmlib_throw("IndexWriter::init()","Can't fork a new process for the parser");
  }

  if(_parser_pid == 0) {
    /* code to launch the parser */
    int i2p_fd, p2i_fd;
    std::string full_parser_path = cmlib::makeFullString(parser_path.c_str());
    std::string full_parser_data_path = cmlib::makeFullString(parser_data_path.c_str());
    std::string features = std::string(FEATURES);

    /* Opening the FIFO to indexer for reading and writing */
    while(((p2i_fd = open(_p2i_path.c_str(), O_RDWR)) == -1) && (errno == EINTR));
    /* Opening the FIFO from indexer for reading */
    while(((i2p_fd = open(_i2p_path.c_str(), O_RDONLY)) == -1) && (errno == EINTR));

    if(dup2(i2p_fd, STDIN_FILENO) == -1) {
      cmlib_throw("IndexWriter::init()","(Parser) Failed to redirect STDIN");
    } else if(dup2(p2i_fd, STDOUT_FILENO) == -1) {
      cmlib_throw("IndexWriter::init()","(Parser) Failed to redirect STDOUT");
    } else if((r_close(i2p_fd) == -1) || (r_close(p2i_fd) == -1)) {
      cmlib_throw("IndexWriter::init()","(Parser) Failed to close extra file descriptors");
    } else {
      kill(getppid(), SIGUSR1);
      execl(full_parser_path.c_str(), PARSER_NAME, "-p", full_parser_data_path.c_str(), "-i", "-l", "-f", features.c_str(), NULL);
    }
  } else {
    int p2i_fd;
    /* indexer code */

    if(_verbose)
      printf("Launching parser...");

    /* Opening the FIFO from indexer for reading and writing */
    while(((_i2p_fd = open(_i2p_path.c_str(), O_RDWR)) == -1) && (errno == EINTR));
    /* Opening the FIFO from parser for reading */
    while(((p2i_fd = open(_p2i_path.c_str(), O_RDONLY)) == -1) && (errno == EINTR));

    if(dup2(p2i_fd, STDIN_FILENO) == -1) {
      cmlib_throw("IndexWriter::init()","(IndexWriter) Failed to redirect STDIN");
    }

    if(r_close(p2i_fd) == -1) {
      cmlib_throw("IndexWriter::init()","(IndexWriter) Failed to close extra file descriptors");
    }

    int_act.sa_handler = ctrlc_handler;
    int_act.sa_flags = 0;
    if((sigemptyset(&int_act.sa_mask) == -1) || (sigaction(SIGINT, &int_act, NULL) == -1)) {
      cmlib_throw("IndexWriter::launch_parser()", "(IndexWriter) Failed to set Ctrl-C handler\n");
    }

    /* waiting for the parser to start */
    sigset_t maskmost;
    sigfillset(&maskmost);
    sigdelset(&maskmost, SIGUSR1);
    while(!parser_ready)
      sigsuspend(&maskmost);
    _initialized = true;
    if(_verbose)
      printf("OK\n");
  }
}

void IndexWriter::read_pat_config(const std::string &pat_conf_path, const std::string &codep_path) throw(cmlib::Exception)
{
  Pattern* pattern;
  cmlib::SmlNode pats;
  cmlib::SmlNode* cur_pat;
  cmlib::SmlNode* slots;
  unsigned long pat_id;
  const char *pid;

  std::string code_table = cmlib::makeFullString(codep_path.c_str());
  cmlib::SMLinitEncodingHandler(code_table.c_str());
  std::string full_config = cmlib::makeFullString(pat_conf_path.c_str());

  cmlib::XMLreadFile(full_config.c_str(), pats, true);

  for(unsigned int i = 0; i < pats.nodeCount(); i++) {
    cur_pat = pats.getChildNode(i);
    if(!cur_pat->hasAttr("id")) {
      cmlib_throw("reading configuration file", "there is a pattern, which doesn't have an 'id' attribute");
    } else {
      pat_id = (unsigned long) cur_pat->getLongAttr("id");
      pid = cur_pat->getAttr("id");
    }
    slots = cur_pat->getChildNodeByName("slots");
    if(slots == NULL)
      cmlib_throw("reading configuration file", std::string("pattern with id ") + pid + " doesn't have any slots.");
    pattern = new Pattern(pat_id, slots->nodeCount());
    for(unsigned int j = 0; j < slots->nodeCount(); j++)
      process_slot(slots->getChildNode(j), pattern);
    _patterns.push_back(pattern);
  }
}

void IndexWriter::process_slot(const cmlib::SmlNode* slot, Pattern* p)
  throw (cmlib::Exception)
{
  char pid[14];
  const char* nid;
  unsigned long id;
  PSlot* n;

  sprintf(pid, "%lu", p->get_id());

  if(!slot->hasAttr("id")) {
    cmlib_throw("reading configuration file", std::string("one of the slots in pattern with id ") + pid + " doesn't have attribute 'id'.");
  } else {
    nid = slot->getAttr("id");
    id = (unsigned long) atol(nid);
    n = p->get_slot(id);
    n->id = id;
  }

  if(!slot->hasAttr("role")) {
    cmlib_throw("reading configuration file", std::string("node ") + nid + " in pattern with id " + pid + " doesn't have attribute 'role'.");
  } else {
    n->role = slot->getAttr("role");
  }

  if(!slot->hasAttr("parent")) {
    cmlib_throw("reading configuration file", std::string("node ") + nid + " in pattern with id " + pid + " doesn't have attribute 'parent'.");
  } else {
    unsigned long parent;
    std::string par = slot->getAttr("parent");
    if(!par.length())
      n->parent = (unsigned long) ~0;
    else {
      parent = (unsigned long) slot->getLongAttr("parent");
      n->parent = parent;
      p->get_slot(parent)->children.push_back(id);
    }
  }

  if(!slot->hasAttr("attrs")) {
    cmlib_throw("reading configuration file", std::string("node ") + nid + " in pattern with id " + pid + " doesn't have attribute 'attrs'.");
  } else {
    n->attr = slot->getAttr("attrs");
  }
}

void IndexWriter::index_dir(const char *data_path) throw(cmlib::Exception)
{
  unsigned int j, k;
  unsigned int file_length = 0;
  int line_len;
  bool store_next;
  std::string line, cur_line, prev_line;
  sigset_t maskall, maskold;
  std::string fpath;
  ID docID, sentID, termID, stemID;
  Directory d;
  Instance inst;
  ParseTree tree;
  InstInfo index_entry;

  if(!_initialized) {
    cmlib_throw("indexing directory", "indexer is not initialized");
  }

  std::string data_dir_path = cmlib::makeFullString(data_path);

  if(!Directory::isDir(data_dir_path.c_str())) {
    cmlib_throw("indexing directory", data_dir_path + " is not a directory");
  } else {
    d.open(data_dir_path);
  }

  sigfillset(&maskall);
  /* indexing files in directory */
  while(can_continue && d.goToNext()) {
    sigprocmask(SIG_SETMASK, &maskall, &maskold); // block all signals
    fpath = d.getFullFileName();

    if(_verbose)
      fprintf(stderr, "Indexing file %s...", fpath.c_str());

    docID = _index->get_doc_id_for_path(fpath);

    /* checking if the file is has been already indexed */
    if(docID == 0 || !_index->check_doc_indexed(docID)) {
      std::ifstream file(fpath.c_str());

      if(!file.is_open()) {
        fprintf(stderr, "can't open file %s\n", fpath.c_str());
        continue;
      }

      if(docID == 0) {
        // this is a new document, so register it
        docID = _index->add_new_doc(fpath);
      } else {
        // this is a known document, clean its contents from database
        _index->clear_doc_sents(docID);
      }

      index_entry.doc_id = docID;

      file_length = 0;
      prev_line = "";
      cur_line = "";
      store_next = false;

      while(std::getline(file, cur_line)) {
        cmlib::strip(cur_line);
        line_len = cur_line.length();

        if(!line_len || line_len >= MAX_LINE_SIZE)  {
          // skip empty and too long lines
          continue;
        } else {
          if(store_next) {
            _index->add_new_sentence(docID, cur_line.c_str());
            store_next = false;
          }
        }

        cur_line += '\n';
        line_len += 1;

#ifdef _VERBOSE_DEBUG
        printf("Sending to MINIPAR:\n{%s}", cur_line.c_str());
#endif

        if(r_write(_i2p_fd, (char *) cur_line.c_str(), line_len) != line_len) {
          cmlib_throw("indexing a file", "can't send a sentence to parser");
        }

        tree.clear_tree();

        while( 1 ) {
          std::getline(std::cin, line);
          if(!line.length())
            break;
          parse_line(&tree, line.c_str());
          if(line.compare(")") == 0)
            break;
        }

#ifdef _VERBOSE_DEBUG
        tree.normalize();
        printf("Parsed tree:\n");
        tree.print();
#endif
        file_length += tree.get_words_cnt();
        /* match tree against a pattern */
        sentID = 0;
        for(k = 0; k < _patterns.size(); k++) {
          inst.clear();
#ifdef _VERBOSE_DEBUG
          printf("Trying pattern %lu:\n", _patterns[k]->get_id());
          _patterns[k]->print();
#endif
          if(_patterns[k]->match_tree(&tree, inst)) {

            if(!sentID) {
              // we have not stored the context of the matched instance yet
              if(!prev_line.size())
                _index->add_new_sentence(docID, prev_line.c_str());
              sentID = _index->add_new_sentence(docID, cur_line.c_str());
              store_next = true;
            }

            index_entry.pat_id = _patterns[k]->get_id();
            index_entry.inst_id = ++_cur_inst_id;
            index_entry.sent_id = sentID;
            for(j = 0; j < inst.size(); j++) {
#ifdef _VERBOSE_DEBUG
              printf("Instance: %lu %s %s %d\n", inst[j].id, inst[j].word.c_str(), inst[j].stem.c_str(), inst[j].index ? 1 : 0);
#endif
              StrVec stem_toks;

              std::string word = cmlib::lower_str(inst[j].word);
              std::string stem = cmlib::lower_str(inst[j].stem);

              if((termID = _index->get_term_id(word)) == 0) {
                /* we found a new word, add it to the dictionary */
                termID = _index->add_new_term(word);
              }

              cmlib::split(stem, stem_toks);
              for(unsigned int i = 0; i < stem_toks.size(); i++) {
                cmlib::clear_nonalnum(stem_toks[i]);
                if(stem_toks[i].length()) {
                  if((stemID = _index->get_term_id(stem_toks[i])) == 0) {
                    /* we found a new stem, add it to the dictionary */
                    stemID = _index->add_new_term(stem_toks[i]);
                  }

                  index_entry.slot_id = inst[j].id;
                  index_entry.slot_pos = i;
                  index_entry.stem_id = stemID;
                  index_entry.term_id = termID;
                  _index->add_instance(index_entry);

                }
              }
            }
            break;
          }
        }
        prev_line = cur_line;
      }
      /* mark the document as indexed */
      _index->mark_doc_indexed(docID, file_length);
      file.close();
      if(_verbose)
        printf("OK\n");
    } else {
      if(_verbose)
        printf("OK\n");
    }
    sigprocmask(SIG_SETMASK, &maskold, NULL); /* unblock the signals */
  } /* all files in dir */

}

void IndexWriter::parse_line(ParseTree* tree, const char *line)
{
  unsigned int i, num_chars;
  char *token, *space_pos;
  char stem[4096];
  Node* node = NULL;
  // to know that we need to add a new node
  bool new_node = false;
  unsigned int num = ~0;

  token = strtok((char* ) line, "\t");

  for(i = 1; token != NULL; i++) {
    if(i == 1) {
      if(!cmlib::is_number(token))
        return;
      else {
        num = (unsigned int) atoi(token);
        node = tree->get_node(num);
        if(node == NULL) {
          node = new Node();
          node->n = num;
          new_node = true;
        } else
          node->n = num;
      }
    }
    else if(i == 2) {
      // parsing word
      node->word = token + 1;
      if((*(token+1)) != ',' && (*(token+1)) != '.' &&
         (*(token+1)) != '(' && (*(token+1)) != ')' &&
         (*(token+1)) != '-' && (*(token+1)) != ';' &&
         (*(token+1)) != '!' && (*(token+1)) != '?' &&
         (*(token+1)) != ':' && (*(token+1)) != '/' &&
         (*(token+1)) != '"')
        tree->inc_words_cnt();
    }
    else if(i == 3) {
      space_pos = strrchr(token, ' ');
      if (space_pos != NULL) {
        num_chars = space_pos-token;
        strncpy(stem, token, num_chars);
        stem[num_chars] = '\0';

        if(stem[0] == '~')
          node->stem = node->word;
        else
          node->stem = stem;
      } else {
        node->stem = node->word;
      }
    }
    else if(i == 4) {
      unsigned int parent;
      if(cmlib::is_number(token)) {
        parent = (unsigned int) atoi(token);
        tree->add_child_node(parent, num);
        node->parent = parent;
      }
    }
    else if(i == 5) {
      node->role = token;
    }
    else if(i == 7) {
      // parsing syntactic and semantic properties
      parse_props(node->props, token);
    }
    token = strtok(NULL, "\t");
  }

  if(new_node)
    tree->add_node(node, num);
}

void IndexWriter::parse_props(Props& vect, char *str)
{
  char* token;
  char buf[512];
  char* sign_loc;
  unsigned int num_chars;
  std::string prop;
  int state = 0; // state of the parser
  // 1. syntactic
  // 2. semantic
  token = strtok(str, "()");
  do {
    if(!((strlen(token) == 0 || strlen(token) == 1) && token[0] == ' ')) {
      if(strstr(token, "atts") != NULL)
        state = 1;
      else if(strstr(token, "sem") != NULL)
        state = 2;
      else {
        if(state == 1) {
          sign_loc = strchr(token, '+');
          if(sign_loc != NULL) {
            num_chars = sign_loc-token-1;
            strncpy(buf, token, num_chars);
            buf[num_chars] = '\0';
            prop = std::string(buf);
            vect.insert(prop);
          }
        } else if(state == 2) {
          sign_loc = strchr(token, '+');
          if(sign_loc != NULL) {
            num_chars = sign_loc-token;
            strcpy(buf, token+num_chars);
            Props* sem_props = parse_sem(buf);
            for(PropsIt it = sem_props->begin(); it != sem_props->end(); it++)
              vect.insert(*it);
            delete sem_props;
          }
        }
      }
    }
    token = strtok(NULL, "()");
  } while(token != NULL);
}

Props* IndexWriter::parse_sem(char *str)
{
  char* token;
  Props* vect = new Props();
  token = strtok(str, "+ ");
  do {
    vect->insert(std::string(token));
    token = strtok(NULL, "+ ");
  } while(token != NULL);

  return vect;
}

void IndexWriter::set_verbose(bool status)
{
  _verbose = status;
}

bool IndexWriter::is_verbose() const
{
  return _verbose;
}
