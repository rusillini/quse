/* -*- C++ -*- */

#ifndef __cmlib_strutils_h__
#define __cmlib_strutils_h__

#include <config.h>
#include <except.h>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <set>

#define SPACE_CHARS " \t\n"

namespace cmlib {
  // check if a string is a number
  bool is_number(const char* str);

  // check if a string is empty
  bool isEmptyStr(const char* s);

  // check if a character belongs to a set of characters
  bool isSymOneOf(char c, const char* t);

  // find the first occurrence of a symbol from a set of symbols in a string,
  // if neg=false. Or find the first occurrence of a symbol which is not in a
  // given set of symbols, if neg=true
  const char* findLsym(const char* s, const char* t, bool neg=false);

  // find the last occurrence of a symbol from a set of symbols in a string,
  // if neg=false. Or find the last occurrence of a symbol which is not in a
  // given set of symbols, if neg=true
  const char* findRsym(const char* s, const char* t, bool neg=false);

  // find the left trimming location
  const char* findLtrim(const char* s, const char* t=SPACE_CHARS);

  // find the right trimming location
  const char* findRtrim(const char* s, const char* t=SPACE_CHARS);

  // check if a string is composed of all whitespaces
  bool isWhiteSpace(const char *s);

  // right-trim a string (s is a set of space symbols to remove)
  void rstrip(std::string& str, const char *s=SPACE_CHARS);

  // left-trim a string (s is a set of space symbols to remove)
  void lstrip(std::string& str, const char *s=SPACE_CHARS);

  // trim a string from both ends (s is a set of space symbols to remove)
  void strip(std::string& str, const char *s=SPACE_CHARS);

  // convert string characters to lower case
  std::string lower_str(const std::string& str);

  /*
   * Split a string into segments:
   *   Args:
   *     * str - string to split;
   *     * segs - vector of resulting segments
   *     * lpos - left boundary of a string
   *     * rpos - right boundary of a string
   *     * seps - list of separators
   *   Output:
   *     number of string segments
   */
  unsigned int split(const std::string& str, std::vector<std::string>& segs, const char *seps=" ",
                     const std::string::size_type lpos=0, const std::string::size_type rpos=std::string::npos);

  /*
   * Extract tokens divided by a separator from a string into a vector
   *   Args:
   *     * str - string to parse;
   *     * sep - token separator;
   *   Output:
   *     vector of parsed tokens.
   */
  std::vector<std::string>* parse_strtok_vec(const std::string& str,  const char sep);

  /*
   * Extract tokens divided by a separator from a string into a set
   *   Args:
   *     * str - string to parse;
   *     * sep - token separator;
   *   Output:
   *     set of parsed tokens.
   */
  std::set<std::string>* parse_strtok_set(const std::string& str, const char sep);

  /*
   * Parse query string produced by the GET method into parameter-value pairs
   *   Args:
   *     * query_string - query string to parse;
   *     * param_map - mapping of parameters to their values.
   *   Output:
   *     none
   */
  std::map<std::string, std::string>* parse_query_string(const char *query_string);

  /*
   * Concatenate with struts a vector of strings
   *   Args:
   *     * str_vec - vector of strings
   *     * str - string to store the result
   *     * strut - strut to concatenate the strings
   */
  void str_vec_to_string(const std::vector<std::string> &str_vec, std::string& str, const char* strut = " ");

  /*
   * Concatenate with struts a set of strings
   *   Args:
   *     * str_set - set of strings
   *     * str - string to store the result
   *     * strut - strut to concatenate the strings
   */
  void str_set_to_string(const std::set<std::string> &str_set, std::string& str, const char* strut = " ");

  /*
   * Replace all occurrences of a character with another character
   *   Args:
   *     * orig - string, which characters to replace
   *     * pat - character to replace
   *     * rep - character to replace with
   */
  std::string replace_str_chars(const std::string &orig, const char pat, const char rep);

  /*
   * Remove all non alphabetic characters from a string
   */
  void clear_nonalnum(std::string &str);

  class string_ref
  {
  protected:
    const char* s_;

  public:
    string_ref() : s_(NULL) {}
    string_ref(const char* s) : s_(s) {}
    string_ref(const string_ref& s) : s_(s.s_) {}
    ~string_ref() { s_ = NULL; }

    int compare(const char* s) const;
    int compare(const string_ref& s) const
    { return compare(s.s_); }

    string_ref& operator=(const char* s)
    {
      s_ = s;
      return *this;
    }
    string_ref& operator=(const string_ref& s)
    {
      s_ = s.s_;
      return *this;
    }

    const char* c_str() const { return s_ ? s_ : ""; }

    bool operator<(const char* s) const { return compare(s) < 0; }
    bool operator<(const string_ref& s) const { return compare(s) < 0; }

    bool operator>(const char* s) const { return compare(s) > 0; }
    bool operator>(const string_ref& s) const { return compare(s) > 0; }

    bool operator<=(const char* s) const { return compare(s) <= 0; }
    bool operator<=(const string_ref& s) const { return compare(s) <= 0; }

    bool operator>=(const char* s) const { return compare(s) >= 0; }
    bool operator>=(const string_ref& s) const { return compare(s) >= 0; }

    bool operator==(const char* s) const { return compare(s) == 0; }
    bool operator==(const string_ref& s) const { return compare(s) == 0; }

    bool operator!=(const char* s) const { return compare(s) != 0; }
    bool operator!=(const string_ref& s) const { return compare(s) != 0; }
  };
}

#endif // __cmlib_strutils_h__
