/* -*- C++ -*- */

#include <file_utils.h>

cmlib::SectionMap* cmlib::read_config_file(const char *path) throw (cmlib::Exception) {
  char line[LINE_BUF_SIZE];
  std::string sline, cur_sect, cur_param, cur_value;
  std::string::size_type pos, lpos, rpos;
  unsigned int line_no;
  std::ifstream conf_file;
  std::string full_path = cmlib::makeFullString(path);
  conf_file.open(full_path.c_str());
  if(!conf_file.is_open()) {
    cmlib_throw(std::string("reading system configuration file"), std::string("can't open configuration file ") + full_path);
  }
  cmlib::SectionMap * smap = new cmlib::SectionMap();
  line_no = 0;
  cur_sect = "";
  while(!conf_file.eof()) {
    line_no++;
    conf_file.getline(line, LINE_BUF_SIZE);
    sline = line;
    // searching and removing comments
    pos = sline.find("#");
    if(pos != std::string::npos)
      sline.erase(pos);
    cmlib::strip(sline);
    if(!sline.length())
      continue;
    if((pos = sline.find("[")) != std::string::npos) {
      // section name on this line
      lpos = pos;
      rpos = sline.find("]", lpos);
      if(rpos == std::string::npos) {
        char str_line_no[14];
        sprintf(str_line_no, "%u", line_no);
        cmlib_throw(std::string("quse.conf (line ") + str_line_no + ")", "missing closing ']'");
      } else {
        cur_sect = std::string(sline, lpos+1, rpos-lpos-1);
        cmlib::ParamMap pmap;
        (*smap)[cur_sect] = pmap;
      }
    } else if((pos = sline.find("=")) != std::string::npos) {
      // parameter definition on this line
      cur_param = std::string(sline, 0, pos);
      cur_value = std::string(sline, pos+1);
      if(!cur_sect.length()) {
        char str_line_no[14];
        sprintf(str_line_no, "%u", line_no);
        cmlib_throw(std::string("quse.conf (line ") + str_line_no + ")", std::string("parameter '") + cur_param + "' is defined outside of any section");
      } else {
        (*smap)[cur_sect][cur_param] = cur_value;
      }
    }
  }
  conf_file.close();
  return smap;
}

bool cmlib::get_param_value(std::string &value, cmlib::SectionMap *conf, const char *sec, const char *param) {
  cmlib::ParamMapIt pMapIt;
  cmlib::SectionMapIt sMapIt;

  sMapIt = conf->find(sec);
  if(sMapIt == conf->end()) {
    return false;
  } else {
    pMapIt = sMapIt->second.find(param);
    if(pMapIt == sMapIt->second.end()) {
      return false;
    } else {
      value = pMapIt->second;
      return true;
    }
  }
}

