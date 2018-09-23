/* -*- C++ -*- */

// $Id: IndexWriter.hpp,v 1.6 2010/02/10 10:07:06 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef __index_writer_hpp__
#define __index_writer_hpp__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <limits.h>
#include <signal.h>
#include <smldata.h>
#include <smlxt.h>
#include <envparm.h>
#include <file_utils.h>
#include <restart.h>
#include <strutils.h>
#include <except.h>
#include <fstream>
#include <Types.hpp>
#include <Pattern.hpp>
#include <ParseTree.hpp>
#include <Directory.hpp>
#include <DBIndexIO.hpp>

#define I2P_FIFO_PERMS (S_IRUSR | S_IWUSR)
#define P2I_FIFO_PERMS (S_IRUSR | S_IWUSR)

/* name of the FIFO from indexer to parser */
#define I2P_FIFO_NAME "index2par"
/* name of the FIFO from parser to indexer */
#define P2I_FIFO_NAME "par2index"

/* parser name */
#define PARSER_NAME "parser"

/* what entities to detect by MINIPAR */
#define FEATURES "appo att be cn genetive neg passive pn prog tense sem"

// maximum size of the line to send to parser
#define MAX_LINE_SIZE 1000

/* Ctrl-C handler */
void ctrlc_handler(int signo);

/* SIGUSR1 handler */
void usr_handler(int signo);

class IndexWriter {
public:
  IndexWriter(const char *conf_file, const char *temp_dir, bool verbose);
  ~IndexWriter();

  /* initialize the indexer */
  void init() throw (cmlib::Exception);

  /*
   * index the enitre directory:
   *  - data_path: path to the data directory;
   *  - index_path: directory where to store the index files.
   */
  void index_dir(const char *data_dir) throw(cmlib::Exception);

  /* print informational messages during parsing */
  void set_verbose(bool flag);

  /* check if indexer is in verbose mode */
  bool is_verbose() const;

private:
  /* collection of patterns */
  typedef std::vector<Pattern* > Patterns;

private:
  /* read patterns configuration file*/
  void read_pat_config(const std::string &pat_conf_path, const std::string &codep_path) throw(cmlib::Exception);

  /* process one node of a pattern */
  void process_slot(const cmlib::SmlNode* , Pattern* )
    throw (cmlib::Exception);

  /* clear pattern configuration */
  void clean_patterns()
    throw();

  /* parse a new sentence and return its parse tree */
  ParseTree* parse_sentence(const char *sent)
    throw (cmlib::Exception);

  /* process one line of output from the parser */
  void parse_line(ParseTree* tree, const char *line);

  /* parse property string */
  void parse_props(Props& vect, char *str);

  /* parse semantic properties */
  Props* parse_sem(char *str);

private:
  // show the list of files as they are indexed
  bool _verbose;
  // indexer state
  bool _initialized;
  // patterns to look for in the data
  Patterns _patterns;
  // full path to the FIFO from indexer to parser
  std::string _i2p_path;
  // full path to the FIFO from parser to indexer
  std::string _p2i_path;
  // path to the system configuration file
  const char *_sys_conf_path;
  // file descriptor of the FIFO from indexer to parser
  int _i2p_fd;
  // buffer used to read from the pipe
  char _buf[PIPE_BUF];
  // fetcher
  DBIndexIO *_index;
  // ID of the current instance
  ID _cur_inst_id;
};

#endif // __index_writer_hpp__
