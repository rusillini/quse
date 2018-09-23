/* -*- C++ -*- */

// $Id: QProc.cpp,v 1.35 2010/04/25 04:05:17 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

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
#define NUM_TABLE_COLS 2

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
  unsigned int table_width = QUEST_COL_WIDTH+SNIP_COL_WIDTH;
  FCGI_printf("<form method=\"get\" accept-charset=\"iso-8859-1\" action=\"%s\">\n", action.c_str());
  FCGI_printf("<input type=\"hidden\" name=\"sess_id\" value=\"%u\"/>\n", sess_id);
  FCGI_printf("<input type=\"hidden\" name=\"beg_qnum\" value=\"%u\"/>\n", beg_qnum);
  FCGI_printf("<input type=\"hidden\" name=\"end_qnum\" value=\"%u\"/>\n", end_qnum);
  FCGI_printf("<input type=\"hidden\" name=\"qnum_total\" value=\"%u\"/>\n", qnum_total);
  FCGI_printf("<table border=\"0\" height=\"30\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td>&nbsp;</td></tr>\n");
  FCGI_printf("</table>\n");
  FCGI_printf("<table border=\"0\" width=\"%u\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n<td align=\"center\" valign=\"top\">\n");
  FCGI_printf("<table border=\"1\" width=\"%u\" cellpadding=\"4\" cellspacing=\"2\" bgcolor=\"#ffe2b1\" style=\"border-collapse:collapse\">\n", table_width);
  FCGI_printf("<tr>\n");
  FCGI_printf("<td width=\"%d\" style=\"font-weight:bold\">%s</td>\n", QUEST_COL_WIDTH, "Did you mean?");
  FCGI_printf("<td width=\"%d\" style=\"font-weight:bold\">%s</td>\n", SNIP_COL_WIDTH, "Answer");
  FCGI_printf("</tr>\n");
}

static void print_form_footer() {
  FCGI_printf("<tr>\n<td colspan=\"%d\" align=\"center\"><input style=\"font-family:arial,sans-serif;font-size:11pt\" type=\"submit\" value=\"Next\"></td>\n</tr>\n", NUM_TABLE_COLS);
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
      std::string ex_query, click_url;
      cQuestMapIt it;
      unsigned int cur_num;
      std::string click_url_base = url_base + qproc_script + std::string("?query=");

      for(it = qmap->begin(), cur_num = beg_qnum; it != qmap->end(); it++, cur_num++) {
        QuestInfo qi = it->second;
        ID quest_id = model->get_quest_id(sess_id, cur_num);
        FCGI_printf("<tr>\n");
        // printing question
        click_url = click_url_base + model->get_expanded_query(sess_id, qi.inst_id);
        FCGI_printf("<td><a href=\"%s\">%s</a></td>\n", click_url.c_str(), qi.quest.c_str());

        // printing answer
        FCGI_printf("<td>");
        const StrVec *cont = model->get_inst_context(qi.inst_id);
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

static void print_thanks_msg() {
  std::string sbox_url = url_base + sbox_script;
  FCGI_printf("<b>Click <a href=\"%s\">here</a> to go back to the search page and sumbit a new query.</b>\n", sbox_url.c_str());
  print_html_footer();
}

static void print_error_msg(std::string& msg) {
  FCGI_printf("<b>System configuration error: %s</b><br>\n", msg.c_str());
  print_html_footer();
}

static void initialize(const char *conf_path) {
  cmlib::SectionMap *conf = NULL;
  // DB connection parameters
  std::string db_name, host, user, passwd;
  // retrieval model parameters
  std::string okapi_k1_str, okapi_b_str, okapi_k3_str;
  // system configuration parameters
  std::string patf_path, codep_path, stopw_path, qnum_page_str, qnum_max_show_str, qnum_max_store_str;

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
  // current session ID
  unsigned int sess_id;
  // total number of questions in the current session
  unsigned int qnum_total;
  // question ranks to display for the current and the new page
  unsigned int cur_beg_qnum, cur_end_qnum, new_beg_qnum, new_end_qnum;
  // user submitted query
  std::string orig_query;
  std::string lowc_term;

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


        // searching for session ID in the URL string
        qParIt = query_params->find("sess_id");

        if (qParIt == query_params->end()) {
          // no session ID in URL - this is the first page of a newly started session
          QuestRankedList *rquests = NULL;
          // original query terms
          StrVec *qterms = NULL;
          // normalized query terms (lowercased, stop words removed)
          StrSet *qterms_norm = NULL;

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
          }

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
          sess_id = atoi(qParIt->second.c_str());

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
          // searching URL query string for the total number of questions
          qParIt = query_params->find("qnum_total");
          if (qParIt == query_params->end()) {
            err_msg = "no parameter 'qnum_total' found in the URL query string";
            error = true;
            print_error_msg(err_msg);
            delete query_params;
            continue;
          }

          qnum_total = atoi(qParIt->second.c_str());

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

        delete query_params;
        query_params = NULL;
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
