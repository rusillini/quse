/* -*- C++ -*- */

// $Id: QProcEval.cpp,v 1.2 2010/04/25 03:52:51 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2010.

#include <fcgi_stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <except.h>
#include <strutils.h>
#include <file_utils.h>
#include <envparm.h>
#include <Types.hpp>
#include <IndexIO.hpp>
#include <DBIndexIO.hpp>
#include <RetModel.hpp>
#include <QuestRankedList.hpp>

// Result presentation parameters
#define QUEST_COL_WIDTH 500
#define SNIP_COL_WIDTH 600
#define EVAL_COL_WIDTH 30
#define NUM_TABLE_COLS 5

// possible types of user judgments
typedef enum _JType {
  JT_WELLF,  // question is well-formed
  JT_INTER,  // question is interesting
  JT_RELEV   // question is relevant
} JType;

static bool error;
static std::string err_msg;
static std::string url_base;
static std::string qproc_script;
static std::string sbox_script;
static RetModel *model;
// number of questions to show for judgment per page for both feedback and non-feedback sessions
static unsigned int qnum_page;
// maximum number of questions to show for user judgment for non-feedback sessions
static unsigned int qnum_max_show;
// maximum number of questions to store for non-feedback sessions
static unsigned int qnum_max_store;
// maximum number of questions to show for user judgment for feedback sessions
static unsigned int qfeed_max_show;
// maximum number of questions to store for feedback sessions
static unsigned int qfeed_max_store;
// how many top questions are clickable
static unsigned int click_max;

/*
 * Determine the range of questions to be displayed on the next evaluation page
 */
static void get_new_quest_range(const unsigned int old_end_qnum, const unsigned int qnum_page, const unsigned int tot_qnum,
                                unsigned int &new_beg_qnum, unsigned int &new_end_qnum) {
  if(old_end_qnum+1 == tot_qnum) {
    new_beg_qnum = tot_qnum+1;
    new_end_qnum = tot_qnum+1;
    return;
  } else {
    new_beg_qnum = old_end_qnum+1;
  }

  if(new_beg_qnum+qnum_page-1 > tot_qnum) {
    new_end_qnum = tot_qnum;
  } else {
    new_end_qnum = new_beg_qnum+qnum_page-1;
  }
}

static void print_html_header() {
  FCGI_printf("Content-Type:text/html;charset=iso-8859-1\r\n\r\n");
  FCGI_printf("<html>\n<body>\n");
  FCGI_printf("<table border=\"0\" height=\"100%%\" width=\"100%%\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n");
  FCGI_printf("<tr>\n<td align=\"center\" valign=\"top\">\n");
}

static void print_html_footer() {
  FCGI_printf("</td>\n</tr>\n</table>\n</body>\n</html>\n");
}

static void print_form_header(const unsigned int sess_id, const unsigned int beg_qnum,
                              const unsigned int end_qnum, const unsigned int qnum_total) {
  std::string action = url_base + qproc_script;
  unsigned int table_width = 3*EVAL_COL_WIDTH+QUEST_COL_WIDTH+SNIP_COL_WIDTH;
  FCGI_printf("<form method=\"get\" accept-charset=\"iso-8859-1\" action=\"%s\">\n", action.c_str());
  FCGI_printf("<input type=\"hidden\" name=\"fb\" value=\"0\"/>\n");
  FCGI_printf("<input type=\"hidden\" name=\"sess_id\" value=\"%u\"/>\n", sess_id);
  FCGI_printf("<input type=\"hidden\" name=\"beg_qnum\" value=\"%u\"/>\n", beg_qnum);
  FCGI_printf("<input type=\"hidden\" name=\"end_qnum\" value=\"%u\"/>\n", end_qnum);
  FCGI_printf("<input type=\"hidden\" name=\"qnum_total\" value=\"%u\"/>\n", qnum_total);
  FCGI_printf("<table border=\"0\" height=\"30\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td>&nbsp;</td></tr>\n");
  FCGI_printf("</table>\n");
  FCGI_printf("<table border=\"1\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td><b>INSTRUCTIONS:</b>&nbsp;1.FIRST, click on the most interesting questions among the top 10 results to further explore the topic.");
  FCGI_printf("2. Then judge the well-formedness, interestingness, and relevance of these 30 questions by marking the check boxes, after that click on \"Evaluate\".</br>");
  FCGI_printf("<b>W&nbsp;=&nbsp;Well-formed</b> (grammatically correct and meaningful);&nbsp;<b>I&nbsp;=&nbsp;Interesting</b> (encourages further exploration of the topic);&nbsp;<b>R&nbsp;=&nbsp;Relevant</b> (helps to satisfy information need)</td></tr>\n");
  FCGI_printf("</table>\n");
  FCGI_printf("<table border=\"0\" height=\"10\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td>&nbsp;</td></tr>\n");
  FCGI_printf("</table>\n");
  FCGI_printf("<table border=\"0\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td align=\"center\" valign=\"top\">\n");
  FCGI_printf("<table border=\"1\" width=\"%u\" cellpadding=\"4\" cellspacing=\"2\" bgcolor=\"#ffe2b1\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n");
  FCGI_printf("<td width=\"%d\" valign=\"middle\" align=\"center\" style=\"font-weight:bold\">W</td>\n", EVAL_COL_WIDTH);
  FCGI_printf("<td width=\"%d\" valign=\"middle\" align=\"center\" style=\"font-weight:bold\">I</td>\n", EVAL_COL_WIDTH);
  FCGI_printf("<td width=\"%d\" valign=\"middle\" align=\"center\" style=\"font-weight:bold\">R</td>\n", EVAL_COL_WIDTH);
  FCGI_printf("<td width=\"%d\" style=\"font-weight:bold\">%s</td>\n", QUEST_COL_WIDTH, "Did you mean?");
  FCGI_printf("<td width=\"%d\" style=\"font-weight:bold\">%s</td>\n", SNIP_COL_WIDTH, "Answer");
  FCGI_printf("</tr>\n");
}

static void print_feed_form_header(const unsigned int fsess_id, const unsigned int fbeg_qnum,
                                   const unsigned int fend_qnum, const unsigned int fqnum_total,
                                   const std::string &osess_id, const std::string &obeg_qnum,
                                   const std::string &oend_qnum, const std::string &oqnum_total) {
  std::string action = url_base + qproc_script;
  unsigned int table_width = 3*EVAL_COL_WIDTH+QUEST_COL_WIDTH+SNIP_COL_WIDTH;
  FCGI_printf("<form method=\"get\" accept-charset=\"iso-8859-1\" action=\"%s\">\n", action.c_str());
  FCGI_printf("<input type=\"hidden\" name=\"fb\" value=\"1\"/>\n");
  FCGI_printf("<input type=\"hidden\" name=\"fsess_id\" value=\"%u\"/>\n", fsess_id);
  FCGI_printf("<input type=\"hidden\" name=\"fbeg_qnum\" value=\"%u\"/>\n", fbeg_qnum);
  FCGI_printf("<input type=\"hidden\" name=\"fend_qnum\" value=\"%u\"/>\n", fend_qnum);
  FCGI_printf("<input type=\"hidden\" name=\"fqnum_total\" value=\"%u\"/>\n", fqnum_total);
  FCGI_printf("<input type=\"hidden\" name=\"osess_id\" value=\"%s\"/>\n", osess_id.c_str());
  FCGI_printf("<input type=\"hidden\" name=\"obeg_qnum\" value=\"%s\"/>\n", obeg_qnum.c_str());
  FCGI_printf("<input type=\"hidden\" name=\"oend_qnum\" value=\"%s\"/>\n", oend_qnum.c_str());
  FCGI_printf("<input type=\"hidden\" name=\"oqnum_total\" value=\"%s\"/>\n", oqnum_total.c_str());
  FCGI_printf("<table border=\"0\" height=\"30\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td>&nbsp;</td></tr>\n");
  FCGI_printf("</table>\n");
  FCGI_printf("<table border=\"1\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td><b>INSTRUCTIONS:</b>&nbsp;Judge the well-formedness, interestingness, and relevance of these 10 feedback questions by marking the check boxes, after that click on \"Evaluate\".</br>");
  FCGI_printf("<b>W&nbsp;=&nbsp;Well-formed</b> (grammatically correct and meaningful);&nbsp;<b>I&nbsp;=&nbsp;Interesting</b> (encourages further exploration of the topic);&nbsp;<b>R&nbsp;=&nbsp;Relevant</b> </b> (helps to satisfy information need)</td></tr>\n");
  FCGI_printf("</table>\n");
  FCGI_printf("<table border=\"0\" height=\"10\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td>&nbsp;</td></tr>\n");
  FCGI_printf("</table>\n");
  FCGI_printf("<table border=\"0\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td align=\"center\" valign=\"top\">\n");
  FCGI_printf("<table border=\"1\" width=\"%u\" cellpadding=\"4\" cellspacing=\"2\" bgcolor=\"#ffe2b1\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n");
  FCGI_printf("<td width=\"%d\" valign=\"middle\" align=\"center\" style=\"font-weight:bold\">W</td>\n", EVAL_COL_WIDTH);
  FCGI_printf("<td width=\"%d\" valign=\"middle\" align=\"center\" style=\"font-weight:bold\">I</td>\n", EVAL_COL_WIDTH);
  FCGI_printf("<td width=\"%d\" valign=\"middle\" align=\"center\" style=\"font-weight:bold\">R</td>\n", EVAL_COL_WIDTH);
  FCGI_printf("<td width=\"%d\" style=\"font-weight:bold\">%s</td>\n", QUEST_COL_WIDTH, "Did you mean?");
  FCGI_printf("<td width=\"%d\" style=\"font-weight:bold\">%s</td>\n", SNIP_COL_WIDTH, "Answer");
  FCGI_printf("</tr>\n");
}

static void print_form_footer() {
  FCGI_printf("<tr>\n<td colspan=\"%d\" align=\"center\"><input style=\"font-family:arial,sans-serif;font-size:11pt\" type=\"submit\" value=\"Evaluate\"></td>\n</tr>\n", NUM_TABLE_COLS);
  FCGI_printf("</table>\n</tr></table></form>\n");
}

static void print_form_message(const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  FCGI_printf("<tr>\n<td colspan=\"%d\" align=\"center\">", NUM_TABLE_COLS);
  FCGI_vprintf(msg, args);
  FCGI_printf("</td>\n</tr>\n");
  va_end(args);
}

static void print_questions(const ID sess_id, const unsigned int beg_qnum,
                            const unsigned int end_qnum, const unsigned int qnum_total) {
  const QuestMap *qmap = NULL;
  try {
    qmap = model->get_session_questions(sess_id, beg_qnum, end_qnum);
    if(qmap == NULL) {
      print_form_message("Error: can't get a list of questions!");
      return;
    } else {
      char buf[20];
      cQuestMapIt it;
      unsigned int cur_num;
      snprintf(buf, 10, "%u", sess_id);
      std::string click_url_base = url_base + qproc_script + "?fb=1&osess_id=" + buf;
      snprintf(buf, 10, "%u", beg_qnum);
      click_url_base += std::string("&obeg_qnum=") + buf;
      snprintf(buf, 10, "%u", end_qnum);
      click_url_base += std::string("&oend_qnum=") + buf;
      snprintf(buf, 10, "%u", qnum_total);
      click_url_base += std::string("&oqnum_total=") + buf;
      click_url_base += std::string("&query=");

      for(it = qmap->begin(), cur_num = beg_qnum; it != qmap->end(); it++, cur_num++) {
        QuestInfo qi = it->second;
        ID quest_id = model->get_quest_id(sess_id, cur_num);

        const StrVec *cont = model->get_inst_context(qi.inst_id);
        FCGI_printf("<tr>\n");
        snprintf(buf, 10, "q%u_w", it->first);
        FCGI_printf("<td><input type=\"checkbox\" name=\"%s\"%s></td>\n", buf, qi.wellf ? " checked" : "");
        snprintf(buf, 10, "q%u_i", it->first);
        FCGI_printf("<td><input type=\"checkbox\" name=\"%s\"%s></td>\n", buf, qi.inter ? " checked" : "");
        snprintf(buf, 10, "q%u_r", it->first);
        FCGI_printf("<td><input type=\"checkbox\" name=\"%s\"%s></td>\n", buf, qi.relev ? " checked" : "");

        if(cur_num <= click_max) {
          // question is clickable
          if(!model->check_quest_clicked(quest_id)) {
            std::string ex_query = model->get_expanded_query(sess_id, qi.inst_id);
            snprintf(buf, 10, "%u", quest_id);
            std::string click_url = click_url_base + ex_query + "&origq_id=" + buf;
            FCGI_printf("<td><a href=\"%s\">%s</a></td>\n", click_url.c_str(), qi.quest.c_str());
            FCGI_printf("<td>");
          } else {
            FCGI_printf("<td>%s</td>\n", qi.quest.c_str());
            FCGI_printf("<td>");
          }
        } else {
          // question is not clickable
          FCGI_printf("<td>%s</td>\n", qi.quest.c_str());
          FCGI_printf("<td>");
        }

        for(unsigned int i = 0; i < cont->size(); i++) {
          if(cont->size() == 3 && i == 1) {
            FCGI_printf("<b>%s</b>", cont->at(i).c_str());
          } else {
            FCGI_printf("%s", cont->at(i).c_str());
          }
        }
        FCGI_printf("</td>\n</tr>\n");
        delete cont;
        model->mark_quest_shown(quest_id);
      }
      delete qmap;
    }
  } catch(cmlib::Exception &e) {
    print_form_message(e.what());
    if(qmap != NULL)
      delete qmap;
    return;
  }
}

static void print_feed_questions(const unsigned int sess_id, const unsigned int beg_qnum, const unsigned int end_qnum) {
  const QuestMap *qmap = NULL;
  try {
    qmap = model->get_session_questions(sess_id, beg_qnum, end_qnum);
    if(qmap == NULL) {
      print_form_message("Error: can't get a list of questions!");
      return;
    } else {
      char cbox_name[10];
      cQuestMapIt it;
      unsigned int cur_num;
      for(it = qmap->begin(), cur_num = beg_qnum; it != qmap->end(); cur_num++, it++) {
        QuestInfo qi = it->second;
        ID quest_id = model->get_quest_id(sess_id, cur_num);
        const StrVec *cont = model->get_inst_context(qi.inst_id);
        FCGI_printf("<tr>\n");
        snprintf(cbox_name, 10, "q%u_w", it->first);
        FCGI_printf("<td><input type=\"checkbox\" name=\"%s\"%s></td>\n", cbox_name, qi.wellf ? " checked" : "");
        snprintf(cbox_name, 10, "q%u_i", it->first);
        FCGI_printf("<td><input type=\"checkbox\" name=\"%s\"%s></td>\n", cbox_name, qi.inter ? " checked" : "");
        snprintf(cbox_name, 10, "q%u_r", it->first);
        FCGI_printf("<td><input type=\"checkbox\" name=\"%s\"%s></td>\n", cbox_name, qi.relev ? " checked" : "");
        FCGI_printf("<td>%s</td>\n", qi.quest.c_str());
        FCGI_printf("<td>");
        for(unsigned int i = 0; i < cont->size(); i++) {
          if(cont->size() == 3 && i == 1) {
            FCGI_printf("<b>%s</b>", cont->at(i).c_str());
          } else {
            FCGI_printf("%s", cont->at(i).c_str());
          }
        }
        FCGI_printf("</td>\n</tr>\n");
        delete cont;
        model->mark_quest_shown(quest_id);
      }
      delete qmap;
    }
  } catch(cmlib::Exception &e) {
    print_form_message(e.what());
    if(qmap != NULL)
      delete qmap;
    return;
  }
}

static void print_nores_msg() {
  std::string sbox_url = url_base + sbox_script;
  FCGI_printf("<b>No results found! Click <a href=\"%s\">here</a> to go back and sumbit a new query.</b>\n", sbox_url.c_str());
  print_html_footer();
}

static void print_nores_feed_msg(std::string& sess_id, std::string& beg_qnum, std::string &end_qnum,
                                 std::string& qnum_total) {
  std::string sbox_url = url_base + sbox_script + "?fb=0&sess_id=" + sess_id + "&beg_qnum="
                         + beg_qnum + "&end_qnum=" + end_qnum + "&qnum_total=" + qnum_total;
  FCGI_printf("<b>No results found! Click <a href=\"%s\">here</a> to go back and submit a new query.</b>\n", sbox_url.c_str());
  print_html_footer();
}

static void print_thanks_msg() {
  std::string sbox_url = url_base + sbox_script;
  FCGI_printf("<b>Thanks for your evaluation of QUSE! Close the window or click <a href=\"%s\">here</a> to go back and sumbit a new query.</b>\n", sbox_url.c_str());
  print_html_footer();
}

static void print_thanks_feed_msg(std::string& sess_id, std::string& beg_qnum, std::string &end_qnum,
                                  std::string &qnum_total) {
  std::string qproc_url = url_base + qproc_script + "?fb=0&sess_id=" + sess_id + "&beg_qnum="
                         + beg_qnum + "&end_qnum=" + end_qnum + "&qnum_total=" + qnum_total;
  FCGI_printf("<b>Question feedback evaluation complete. Click <a href=\"%s\">here</a> to go back and continue evaluating other questions.</b>\n", qproc_url.c_str());
  print_html_footer();
}

static void print_error_msg(std::string& msg) {
  FCGI_printf("<b>System configuration error: %s</b><br>\n", msg.c_str());
  print_html_footer();
}

static bool parse_judgment(const std::string& pname, unsigned int &inum, JType &jtype) {
  std::string::size_type spos, epos;
  if((spos = pname.find('q')) == std::string::npos ||
     (epos = pname.find('_', spos+1)) == std::string::npos) {
    return false;
  } else {
    inum = atoi(pname.substr(spos+1,epos-spos-1).c_str());
    std::string jtype_str = pname.substr(epos+1);
    if(!jtype_str.compare("w")) {
      jtype = JT_WELLF;
      return true;
    } else if(!jtype_str.compare("i")) {
      jtype = JT_INTER;
      return true;
    } else if(!jtype_str.compare("r")) {
      jtype = JT_RELEV;
      return true;
    } else {
      return false;
    }
  }
}

static void initialize(const char *conf_path) {
  cmlib::SectionMap *conf = NULL;
  // DB connection parameters
  std::string db_name, host, user, passwd;
  // retrieval model parameters
  std::string okapi_k1_str, okapi_b_str, okapi_k3_str;
  // system configuration parameters
  std::string patf_path, codep_path, stopw_path, qnum_page_str, qnum_max_show_str;
  std::string qnum_max_store_str, qfeed_max_show_str, qfeed_max_store_str, click_max_str;

  try {
    // parsing the configuration file
    conf = cmlib::read_config_file(conf_path);
    // reading parameters from the configuration
    if(!cmlib::get_param_value(db_name, conf, "db", "db_name")) {
      error = true;
      err_msg = "configuration file is missing required paramter 'db_name' in section 'db'";
    } else if (!cmlib::get_param_value(host, conf, "db", "host")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'host' in section 'db'";
    } else if(!cmlib::get_param_value(user, conf, "db", "user")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'user' in section 'db'";
    } else if(!cmlib::get_param_value(passwd, conf, "db", "passwd")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'passwd' in section 'db'";
    } else if(!cmlib::get_param_value(patf_path, conf, "system", "patterns")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'patterns' in section 'system'";
    } else if(!cmlib::get_param_value(codep_path, conf, "system", "codepages")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'codepages' in section 'system'";
    } else if(!cmlib::get_param_value(okapi_k1_str, conf, "qproc", "okapi_k1")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'okapi_k1' in section 'qproc'";
    } else if(!cmlib::get_param_value(okapi_b_str, conf, "qproc", "okapi_b")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'okapi_b' in section 'qproc'";
    } else if(!cmlib::get_param_value(okapi_k3_str, conf, "qproc", "okapi_k3")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'okapi_k3' in section 'qproc'";
    } else if(!cmlib::get_param_value(qnum_page_str, conf, "qproc", "qnum_page")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'qnum_page' in section 'qproc'";
    } else if(!cmlib::get_param_value(qnum_max_show_str, conf, "qproc", "qnum_max_show")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'qnum_max_show' in section 'qproc'";
    } else if(!cmlib::get_param_value(qnum_max_store_str, conf, "qproc", "qnum_max_store")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'qnum_max_store' in section 'qproc'";
    } else if(!cmlib::get_param_value(qfeed_max_show_str, conf, "qproc", "qfeed_max_show")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'qfeed_max_show' is section 'qproc'";
    } else if(!cmlib::get_param_value(qfeed_max_store_str, conf, "qproc", "qfeed_max_store")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'qfeed_max_store' in section 'qproc'";
    } else if(!cmlib::get_param_value(click_max_str, conf, "qproc", "click_max")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'click_max' in section 'qproc'";
    } else if(!cmlib::get_param_value(url_base, conf, "system", "url_base")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'url_base' in section 'system'";
    } else if(!cmlib::get_param_value(qproc_script, conf, "qproc", "script")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'script' in section 'qproc'";
    } else if(!cmlib::get_param_value(sbox_script, conf, "sbox", "script")) {
      error = true;
      err_msg = "configuration file is missing required parameter 'script' in section 'sbox'";
    }

    if(error) {
      delete conf;
      return;
    }

    cmlib::get_param_value(stopw_path, conf, "qproc", "stopwords");

    qnum_page = atoi(qnum_page_str.c_str());
    qnum_max_show = atoi(qnum_max_show_str.c_str());
    qnum_max_store = atoi(qnum_max_store_str.c_str());
    qfeed_max_show = atoi(qfeed_max_show_str.c_str());
    qfeed_max_store = atoi(qfeed_max_store_str.c_str());
    click_max = atoi(click_max_str.c_str());

    model = new RetModel(host.c_str(), db_name.c_str(), user.c_str(), passwd.c_str(), patf_path.c_str(),
                         codep_path.c_str(), stopw_path.length() ? stopw_path.c_str() : NULL,
                         atof(okapi_k1_str.c_str()), atof(okapi_b_str.c_str()), atof(okapi_k3_str.c_str()));
    delete conf;
  } catch(cmlib::Exception &e) {
    if(conf != NULL)
      delete conf;
    err_msg = e.what();
    error = true;
  }
}

int main(void) {
  // path to the configuration file should be in the QUSE_CONF environment variable
  const char *conf_path = getenv("QUSE_CONF");

  if (conf_path == NULL) {
    err_msg = "environment variable QUSE_CONF is not defined";
    error = true;
  } else {
    initialize(conf_path);
  }

  while (FCGI_Accept() >= 0) {
    print_html_header();
    if (error) {
      print_error_msg(err_msg);
    } else {
      StrStrMapIt qParIt;

      try {
        // reading and parsing URL query string
        StrStrMap* query_params = cmlib::parse_query_string(getenv("QUERY_STRING"));

        if (query_params == NULL) {
          err_msg = "no parameters found in URL query string";
          error = true;
          print_error_msg(err_msg);
          continue;
        }

        // check if session is a feedback session
        qParIt = query_params->find("fb");

        if(qParIt == query_params->end()) {
          err_msg = "no parameter 'fb' found in the URL query string";
          error = true;
          print_error_msg(err_msg);
          delete query_params;
          query_params = NULL;
          continue;
        }

        if (!qParIt->second.compare("0")) {
          // this is a usual query session

          // current session ID
          unsigned int sess_id;
          // total number of questions in the current session
          unsigned int qnum_total;
          // question ranks to display for the current and the new page
          unsigned int cur_beg_qnum, cur_end_qnum, new_beg_qnum, new_end_qnum;
          // query type
          unsigned int qtype = 0;

          // searching for session ID in the URL string
          qParIt = query_params->find("sess_id");

          if (qParIt == query_params->end()) {
            // no session ID in URL - this is the first page of a newly started session
            QuestRankedList *rquests = NULL;
            // original query terms
            StrVec *qterms = NULL;
            // normalized query terms (lowercased, stop words removed)
            StrSet *qterms_norm = NULL;

            qParIt = query_params->find("type");

            if (qParIt != query_params->end())
              qtype = atoi(qParIt->second.c_str());

            // session has just been started with a new query, which we parse from the URL
            qParIt = query_params->find("query");

            if (qParIt == query_params->end()) {
               err_msg = "no parameter 'query' found in the URL query string";
               error = true;
               print_error_msg(err_msg);
               delete query_params;
               query_params = NULL;
               continue;
            }

            qterms = cmlib::parse_strtok_vec(qParIt->second, '+');

            delete query_params;
            query_params = NULL;

            if (!qterms->size()) {
              FCGI_printf("<b>Empty query was submitted</b><br>\n");
              print_html_footer();
              delete qterms;
              qterms = NULL;
              continue;
            } else {
              std::string orig_query;
              std::string lowc_term;

              qterms_norm = new StrSet();

              for(cStrVecIt it = qterms->begin(); it != qterms->end(); it++) {
                lowc_term = cmlib::lower_str(*it);
                if(!model->is_stopword(lowc_term))
                  qterms_norm->insert(lowc_term);
              }

              cmlib::str_vec_to_string(*qterms, orig_query, " ");
              delete qterms;
              qterms = NULL;

              sess_id = model->create_new_session(orig_query, false);

              if (qtype != 0) {
                model->set_query_type(sess_id, qtype);
              }
            }

            rquests = model->search(*qterms_norm, qnum_max_store);
            delete qterms_norm;
            qterms_norm = NULL;

            qnum_total = rquests->size();
            if(qnum_total == 0) {
              // no results for the query
              print_nores_msg();
            } else {
              QuestInfo qinfo;
              unsigned int cur_rank = 1;
              for (rquests->begin(); rquests->has_next(); rquests->next()) {
                Question &quest = rquests->get();
                qinfo.inst_id = quest.get_inst_id();
                qinfo.sess_id = sess_id;
                qinfo.rank = cur_rank;
                qinfo.num_matches = quest.get_match_cnt();
                qinfo.score = quest.get_doc_score();
                qinfo.quest = quest.get_quest();
                qinfo.qterms = quest.get_qterms_cnt();
                model->add_new_question(qinfo);
                cur_rank++;
              }

              cur_beg_qnum = 1;
              if(qnum_max_show != 0 && qnum_total > qnum_max_show)
                qnum_total = qnum_max_show;
              cur_end_qnum = qnum_total < qnum_page ? qnum_total: qnum_page;
              get_new_quest_range(cur_end_qnum, qnum_page, qnum_total, new_beg_qnum, new_end_qnum);
              print_form_header(sess_id, new_beg_qnum, new_end_qnum, qnum_total);
              print_questions(sess_id, cur_beg_qnum, cur_end_qnum, qnum_total);
              print_form_footer();
              print_html_footer();
            }

            delete rquests;
            rquests = NULL;
          } else {
            // session already started, this is at least the second page of questions

            JType jtype = JT_WELLF;
            // rank of the judged question
            unsigned int jquest_rank;

            sess_id = atoi(qParIt->second.c_str());
            // first we update the judgments from the previous page
            for (qParIt = query_params->begin(); qParIt != query_params->end(); qParIt++) {
              if (parse_judgment(qParIt->first, jquest_rank, jtype)) {
                if (!qParIt->second.compare("on")) {
                  switch (jtype) {
                  case JT_WELLF:
                    model->mark_quest_wellf(sess_id, jquest_rank);
                    break;
                  case JT_INTER:
                    model->mark_quest_inter(sess_id, jquest_rank);
                    break;
                  case JT_RELEV:
                    model->mark_quest_relev(sess_id, jquest_rank);
                    break;
                  }
                }
              }
            }
            // searching URL for the number of the first question in this page
            qParIt = query_params->find("beg_qnum");
            if (qParIt == query_params->end()) {
              err_msg = "no parameter 'beg_qnum' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            }
            cur_beg_qnum = atoi(qParIt->second.c_str());
            // searching URL for the number of the last question in this page
            qParIt = query_params->find("end_qnum");
            if (qParIt == query_params->end()) {
              err_msg = "no parameter 'end_qnum' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            }
            cur_end_qnum = atoi(qParIt->second.c_str());
            // searching URL query string for the total number of instances
            qParIt = query_params->find("qnum_total");
            if (qParIt == query_params->end()) {
              err_msg = "no parameter 'qnum_total' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            }
            qnum_total = atoi(qParIt->second.c_str());
            delete query_params;
            query_params = NULL;

            if (cur_beg_qnum == qnum_total + 1) {
              print_thanks_msg();
            } else {
              get_new_quest_range(cur_end_qnum, qnum_page, qnum_total,
                  new_beg_qnum, new_end_qnum);
              print_form_header(sess_id, new_beg_qnum, new_end_qnum, qnum_total);
              print_questions(sess_id, cur_beg_qnum, cur_end_qnum, qnum_total);
              print_form_footer();
              print_html_footer();
            }
          }
        } else {
          // this is a feedback query session

          // original session ID
          std::string osess_id;
          // question ranks to display for the original session
          std::string obeg_qnum, oend_qnum;
          // total number of questions in the original session
          std::string oqnum_total;
          // current feedback session ID
          unsigned int fsess_id;
          // question ranks to display for the current and the new feedback pages
          unsigned int cur_beg_fqnum, cur_end_fqnum, new_beg_fqnum, new_end_fqnum;
          // total number of questions in the current feedback session
          unsigned int fqnum_total;

          qParIt = query_params->find("osess_id");

          if (qParIt == query_params->end()) {
            err_msg = "no parameter 'osess_id' found in the URL query string";
            error = true;
            print_error_msg(err_msg);
            delete query_params;
            continue;
          } else {
            osess_id = qParIt->second;
          }

          qParIt = query_params->find("obeg_qnum");

          if (qParIt == query_params->end()) {
            err_msg = "no parameter 'obeg_qnum' found in the URL query string";
            error = true;
            print_error_msg(err_msg);
            delete query_params;
            continue;
          } else {
            obeg_qnum = qParIt->second;
          }

          qParIt = query_params->find("oend_qnum");

          if (qParIt == query_params->end()) {
            err_msg = "no parameter 'oend_qnum' found in the URL query string";
            error = true;
            print_error_msg(err_msg);
            delete query_params;
            continue;
          } else {
            oend_qnum = qParIt->second;
          }

          qParIt = query_params->find("oqnum_total");

          if (qParIt == query_params->end()) {
            err_msg = "no parameter 'oqnum_total' found in the URL query string";
            error = true;
            print_error_msg(err_msg);
            delete query_params;
            continue;
          } else {
            oqnum_total = qParIt->second;
          }

          // searching for feedback session ID in the URL string
          qParIt = query_params->find("fsess_id");

          if(qParIt == query_params->end()) {
            // no feedback session ID in the URL - this is the first page of a
            // newly started feedback session
            QuestInfo qinfo;
            // current feedback session query
            std::string query;
            // ID of an original question, by clicking one which the feedback was initiated
            unsigned int origq_id;

            qParIt = query_params->find("origq_id");

            if(qParIt == query_params->end()) {
              err_msg = "no parameter 'origq_id' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            } else {
              origq_id = atoi(qParIt->second.c_str());
              model->mark_quest_clicked(origq_id);
            }

            // feedback session has just been started with a new query, which we parse from the URL
            qParIt = query_params->find("query");

            if(qParIt == query_params->end()) {
              err_msg = "no parameter 'query' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            }

            StrVec* qterms_orig = cmlib::parse_strtok_vec(qParIt->second, '+');
            delete query_params;
            query_params = NULL;

            StrSet* qterms = new StrSet();
            for(cStrVecIt it = qterms_orig->begin(); it != qterms_orig->end(); it++) {
              qterms->insert(*it);
            }

            cmlib::str_vec_to_string(*qterms_orig, query, " ");
            delete qterms_orig;
            qterms_orig = NULL;

            fsess_id = model->create_new_session(query, true);
            QuestRankedList *rquests = model->search(*qterms, qfeed_max_store);
            delete qterms;
            qterms = NULL;

            fqnum_total = rquests->size();
            if(fqnum_total == 0) {
              // no results for the query
              print_nores_feed_msg(osess_id, obeg_qnum, oend_qnum, oqnum_total);
            } else {
              QuestInfo qinfo;
              qinfo.origq_id = origq_id;
              unsigned int cur_rank = 1;
              for(rquests->begin(); rquests->has_next(); rquests->next()) {
                Question &quest = rquests->get();
                qinfo.inst_id = quest.get_inst_id();
                qinfo.sess_id = fsess_id;
                qinfo.rank = cur_rank;
                qinfo.num_matches = quest.get_match_cnt();
                qinfo.score = quest.get_doc_score();
                qinfo.quest = quest.get_quest();
                qinfo.qterms = quest.get_qterms_cnt();
                qinfo.origq_id = origq_id;
                model->add_new_question(qinfo);
                cur_rank++;
              }

              cur_beg_fqnum = 1;
              if(qfeed_max_show != 0 && fqnum_total > qfeed_max_show)
                fqnum_total = qfeed_max_show;
              cur_end_fqnum = fqnum_total < qnum_page ? fqnum_total : qnum_page;
              get_new_quest_range(cur_end_fqnum, qnum_page, fqnum_total, new_beg_fqnum, new_end_fqnum);
              print_feed_form_header(fsess_id, new_beg_fqnum, new_end_fqnum, fqnum_total,
                                     osess_id, obeg_qnum, oend_qnum, oqnum_total);
              print_feed_questions(fsess_id, cur_beg_fqnum, cur_end_fqnum);
              print_form_footer();
              print_html_footer();
            }

            delete rquests;
            rquests = NULL;
          } else {
            // feedback session has already been started, this is at least
            // the second page of feedback questions
            JType jtype = JT_WELLF;
            // rank of the judged question
            unsigned int jquest_rank;

            fsess_id = atoi(qParIt->second.c_str());
            // first we update the judgments from the previous page
            for (qParIt = query_params->begin(); qParIt != query_params->end(); qParIt++) {
              if (parse_judgment(qParIt->first, jquest_rank, jtype)) {
                if (!qParIt->second.compare("on")) {
                  switch (jtype) {
                    case JT_WELLF:
                    model->mark_quest_wellf(fsess_id, jquest_rank);
                    break;
                  case JT_INTER:
                    model->mark_quest_inter(fsess_id, jquest_rank);
                    break;
                  case JT_RELEV:
                    model->mark_quest_relev(fsess_id, jquest_rank);
                    break;
                  }
                }
              }
            }

            // searching URL for the number of the first question in this page
            qParIt = query_params->find("fbeg_qnum");
            if (qParIt == query_params->end()) {
              err_msg = "no parameter 'fbeg_qnum' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            } else {
              cur_beg_fqnum = atoi(qParIt->second.c_str());
            }

            // searching URL for the number of the last question in this page
            qParIt = query_params->find("fend_qnum");
            if (qParIt == query_params->end()) {
              err_msg = "no parameter 'fend_qnum' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            } else {
              cur_end_fqnum = atoi(qParIt->second.c_str());
            }


            // searching URL query string for the total number of instances
            qParIt = query_params->find("fqnum_total");
            if (qParIt == query_params->end()) {
              err_msg = "no parameter 'fqnum_total' found in the URL query string";
              error = true;
              print_error_msg(err_msg);
              delete query_params;
              continue;
            } else {
              fqnum_total = atoi(qParIt->second.c_str());
            }

            delete query_params;
            query_params = NULL;

            if (cur_beg_fqnum == fqnum_total + 1) {
              print_thanks_feed_msg(osess_id, obeg_qnum, oend_qnum, oqnum_total);
            } else {
              get_new_quest_range(cur_end_fqnum, qnum_page, fqnum_total,
                  new_beg_fqnum, new_end_fqnum);
              print_feed_form_header(fsess_id, new_beg_fqnum, new_end_fqnum, fqnum_total, osess_id,
                                     obeg_qnum, oend_qnum, oqnum_total);
              print_feed_questions(fsess_id, cur_beg_fqnum, cur_end_fqnum);
              print_form_footer();
              print_html_footer();
            }
          }
        }
      } catch (cmlib::Exception &e) {
        FCGI_printf("<b>System configuration error: %s</b><br>\n", e.what());
        print_html_footer();
      }
    }
  }

  if (model != NULL)
    delete model;

  return 0;
}
