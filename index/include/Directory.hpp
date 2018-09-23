/* -*- C++ -*- */

// $Id: Directory.hpp,v 1.3 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#ifndef __directory_hpp__
#define __directory_hpp__

#include<stdio.h>
#include<envparm.h>
#include<dirent.h>
#include<except.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<limits.h>
#include<exception>
#include<string>
#include<stack>

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

/* Class providing access to directory contents, including subdirectories */

class Directory {
public:
  Directory()
  { }
  Directory(std::string path) : _path(path)
  { }
  ~Directory();
  /* check if a file exists in directory */
  static bool fileExistsInDir(const char *file, const char *dir = NULL);
  /* check if path is a directory */
  static bool isDir(const char *path);
  /* open directory for reading */
  void open() throw(cmlib::Exception);
  void open(std::string path) throw(cmlib::Exception);
  /* get the name of the next file with full path to it */
  std::string getFullFileName();
  /* get the name of the next file with the path relative
     to the current directory */
  std::string getRelFileName();
  /* go to the next file */
  bool goToNext();
  /* get normalized path */
  std::string getPath() const;
private:
  void addDir(const char* path) throw(cmlib::Exception);
private:
  std::stack<DIR* > _dirs;
  std::string _rel_path;
  std::string _path;
  char _cwd[PATH_MAX];
  dirent *_dire;
};

#endif // __directory_hpp__
