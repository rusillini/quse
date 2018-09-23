/* -*- C++ -*- */

// $Id: Directory.cpp,v 1.3 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <Directory.hpp>

Directory::~Directory()
{
  while(!_dirs.empty()) {
    while((closedir(_dirs.top()) == -1) && (errno = EINTR));
    _dirs.pop();
  }
}

bool Directory::fileExistsInDir(const char *file, const char *dir)
{
  DIR* dirp = NULL;
  struct dirent *dirent;

  if(dir == NULL) {
    dirp = opendir(".");
  } else {
    std::string dir_path = cmlib::makeFullString(dir);
    if(!Directory::isDir(dir_path.c_str()))
      return false;
    else
      dirp = opendir(dir_path.c_str());
  }
  while((dirent = readdir(dirp)) != NULL) {
    if(strcmp(dirent->d_name, file) == 0)
      return true;
  }
  return false;
}

bool Directory::isDir(const char *path)
{
  struct stat statbuf;
  std::string dir_path = cmlib::makeFullString(path);
  if(stat(dir_path.c_str(), &statbuf) == -1)
    return false;
  else
    return S_ISDIR(statbuf.st_mode);
}

void Directory::open() throw(cmlib::Exception)
{
  struct stat statbuf;

    _path = cmlib::makeFullString(_path.c_str());
    if(stat(_path.c_str(), &statbuf) == -1) {
      cmlib_throw("Directory::open()", "Can't execute stat");
    } else {
      if(!S_ISDIR(statbuf.st_mode)) {
        cmlib_throw("Directory::open()", _path + std::string(" is not a directory"));
      }
    }
    _rel_path = "";
    addDir(_path.c_str());
}

void Directory::open(std::string path) throw(cmlib::Exception)
{
  _path = path;
  open();
}

void Directory::addDir(const char* path) throw(cmlib::Exception)
{
  DIR* dir = opendir(path);
  if(dir == NULL) {
    if(errno == EACCES) {
      cmlib_throw("Directory::open()", "Access denied");
    } else {
      cmlib_throw("Directory::open()", "Can't open directory");
    }
  }
  else {
    _dirs.push(dir);
    if(chdir(path) == -1) {
      cmlib_throw("Directory::addDir()", "Can't change directory");
    } else {
      getcwd(_cwd, PATH_MAX);
    }
  }
}

bool Directory::goToNext()
{
  DIR* dir;
  std::string::size_type pos;
  while(!_dirs.empty()) {
    dir = _dirs.top();
    if((_dire = readdir(dir)) == NULL) {
      while((closedir(_dirs.top()) == -1) && (errno == EINTR));
      _dirs.pop();
      pos = _rel_path.rfind('/');
      if(pos != std::string::npos)
        _rel_path = _rel_path.substr(0, pos);
      else
        _rel_path = "";
      if(chdir("..") == -1)
        return false;
      else
        getcwd(_cwd, PATH_MAX);
    }
    else {
      if(_dire->d_type == DT_DIR) {
        if(strcmp(_dire->d_name, "..") != 0 &&
           strcmp(_dire->d_name, ".") != 0) {
          try {
            addDir(_dire->d_name);
            if(_rel_path.length() != 0)
              _rel_path = _rel_path + "/";
            _rel_path += std::string(_dire->d_name);
          }
          catch(std::exception &e) { }
        }
      }
      else if(_dire->d_type == DT_REG) {
        break;
      }
    }
  }
  return _dire != NULL;
}

std::string Directory::getFullFileName()
{
  return std::string(_cwd) + "/" + _dire->d_name;
}

std::string Directory::getRelFileName()
{
  return (_rel_path.length() ? _rel_path + "/" + _dire->d_name : _dire->d_name);
}

std::string Directory::getPath() const
{
  return _path;
}


