/* -*- C++ -*- */

#include <strutils.h>

bool cmlib::is_number(const char* str) {
  while (*str) {
    if (!isdigit(*str))
      return false;
    else
      str++;
  }
  return true;
}

bool cmlib::isEmptyStr(const char* s) {
  if (s == NULL || *s == '\0')
    return true;
  return false;
}

bool cmlib::isSymOneOf(char c, const char* t) {
  if (c == '\0')
    return true;
  if (t == NULL)
    return false;
  while (*t) {
    if (*t == c)
      return true;
    ++t;
  }
  return false;
}

const char* cmlib::findLsym(const char* s, const char* t, bool neg) {
  if (s == NULL)
    return NULL;
  while (*s) {
    if (cmlib::isSymOneOf(*s, t)) {
      if (!neg)
        return s;
    } else {
      if (neg)
        return s;
    }
    ++s;
  }
  return s;
}

const char* cmlib::findRsym(const char* s, const char* t, bool neg) {
  const char* p = NULL;
  if (s == NULL)
    return NULL;
  while (*s) {
    s = cmlib::findLsym(s, t, neg);
    if (s == p) {
      ++s;
      p = s;
    } else if (*s)
      p = s;
  }
  return p ? p : s;
}

const char* cmlib::findLtrim(const char* s, const char* t) {
  return cmlib::findLsym(s, t, true);
}

const char* cmlib::findRtrim(const char* s, const char* t) {
  return cmlib::findRsym(s, t, true);
}

bool cmlib::isWhiteSpace(const char *s) {
  const char *pos = cmlib::findLtrim(s);
  return pos == s+strlen(s);
}


void cmlib::rstrip(std::string& str, const char *s) {
  std::string::size_type pos = str.find_first_not_of(s);
  str.erase(0, pos);
}

void cmlib::lstrip(std::string& str, const char *s) {
  std::string::size_type pos = str.find_last_not_of(s);
  str.erase(pos + 1);
}

std::string cmlib::lower_str(const std::string& str) {
  char chr[2];
  std::string low_str = "";
  for(std::string::const_iterator it = str.begin(); it != str.end(); it++) {
    snprintf(chr, 2, "%c", tolower(*it));
    low_str += std::string(chr);
  }
  return low_str;
}

void cmlib::strip(std::string& str, const char *s) {
  cmlib::rstrip(str, s);
  cmlib::lstrip(str, s);
}

unsigned int cmlib::split(const std::string& str, std::vector<std::string>& segs, const char *seps,
                          const std::string::size_type lpos, const std::string::size_type rpos) {
  std::string::size_type prev = lpos; // previous delimiter position
  std::string::size_type cur = lpos; // current position of the delimiter
  std::string::size_type rbound = rpos == std::string::npos ? str.length()-1 : rpos;

  if (lpos >= rpos || lpos >= str.length() || rbound >= str.length())
    return 0;

  while (1) {
    cur = str.find_first_of(seps, prev);
    if (cur == std::string::npos || cur > rbound) {
      segs.push_back(str.substr(prev, rbound-prev+1));
      break;
    } else {
      if (cur != prev)
        segs.push_back(str.substr(prev, cur-prev));
      prev = cur + 1;
    }
  }
  return segs.size();
}

std::vector<std::string>* cmlib::parse_strtok_vec(const std::string& str, const char sep) {
  std::string::size_type cur_pos = 0, prev_pos = 0;
  std::vector<std::string>* toks = new std::vector<std::string>();

  if (!str.length())
    return toks;

  while ((cur_pos = str.find(sep, prev_pos)) != std::string::npos) {
    toks->push_back(str.substr(prev_pos, cur_pos-prev_pos));
    prev_pos = cur_pos+1;
  }

  if (prev_pos != 0)
    toks->push_back(str.substr(prev_pos, str.length()-prev_pos));
  else
    toks->push_back(str);

  return toks;
}

std::set<std::string>* cmlib::parse_strtok_set(const std::string& str, const char sep) {
  std::string::size_type cur_pos = 0, prev_pos = 0;
  std::set<std::string>* toks = new std::set<std::string>();

  if (!str.length())
    return toks;

  while ((cur_pos = str.find(sep, prev_pos)) != std::string::npos) {
    toks->insert(str.substr(prev_pos, cur_pos-prev_pos));
    prev_pos = cur_pos+1;
  }

  if (prev_pos != 0)
    toks->insert(str.substr(prev_pos, str.length()-prev_pos));
  else
    toks->insert(str);

  return toks;
}

std::map<std::string, std::string>* cmlib::parse_query_string(const char *query_string) {
  unsigned int cur_pos = 0, prev_pos = 0, len = strlen(query_string);
  std::string param;

  if (len == 0)
    return NULL;

  std::map<std::string, std::string>* param_map = new std::map<std::string, std::string>;

  for (cur_pos = 0; cur_pos < len; cur_pos++) {
    if (query_string[cur_pos] == '=') {
      param = std::string(query_string, prev_pos, cur_pos - prev_pos);
      (*param_map)[param] = "";
      prev_pos = cur_pos + 1;
    } else if (query_string[cur_pos] == '&') {
      std::string value = std::string(query_string, prev_pos, cur_pos-prev_pos);
      (*param_map)[param] = value;
      prev_pos = cur_pos + 1;
    } else if (cur_pos == len - 1) {
      std::string value = std::string(query_string, prev_pos, len - prev_pos);
      (*param_map)[param] = value;
    }
  }

  return param_map;
}

void cmlib::str_vec_to_string(const std::vector<std::string> &str_vec, std::string& str, const char* strut) {
  std::vector<std::string>::const_iterator it;

  str = "";
  for(it = str_vec.begin(); it != str_vec.end(); it++) {
    if(it != str_vec.begin())
      str += strut;
    str += *it;
  }
}

void cmlib::str_set_to_string(const std::set<std::string> &str_set, std::string& str, const char* strut) {
  std::set<std::string>::const_iterator it;

  str = "";
  for(it = str_set.begin(); it != str_set.end(); it++) {
    if(it != str_set.begin())
      str += strut;
    str += *it;
  }
}

std::string cmlib::replace_str_chars(const std::string &orig, const char pat, const char rep) {
  std::string res="";
  std::string::const_iterator it;
  for(it = orig.begin(); it != orig.end(); it++)
    res += *it == pat ? rep : *it;
  return res;
}

void cmlib::clear_nonalnum(std::string &str) {
  std::string::iterator it = str.begin();
  while(it != str.end()) {
    if(!isalnum(*it)) {
      it = str.erase(it);
    } else {
      it++;
    }
  }
}
