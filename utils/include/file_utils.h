/* -*- C++ -*- */

// $Id: file_utils.h,v 1.4 2009/06/08 22:04:14 akotov2 Exp $

/*
 * Functions to read and parse configuration files
 */

#ifndef _file_utils_h_
#define _file_utils_h_

#include <config.h>
#include <map>
#include <string>
#include <fstream>
#include <except.h>
#include <envparm.h>
#include <strutils.h>

#define LINE_BUF_SIZE 256

namespace cmlib {

  /* mapping from parameter names to values */
  typedef std::map<std::string, std::string> ParamMap;
  typedef ParamMap::iterator ParamMapIt;
  typedef ParamMap::const_iterator cParamMapIt;

  /* mapping from configuration file sections to parameter values */
  typedef std::map<std::string, ParamMap> SectionMap;
  typedef SectionMap::iterator SectionMapIt;
  typedef SectionMap::const_iterator cSectionMapIt;

  /* read INI-like configuration file and return a map of parameters */
  SectionMap* read_config_file(const char *path) throw (cmlib::Exception);
  /* get the value of a parameter from an INI-like configuration file */
  bool get_param_value(std::string &value, SectionMap *conf, const char *sec, const char *param);
}

#endif /* _file_utils_h_ */
