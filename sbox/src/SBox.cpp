/* -*- C++ -*- */

// $Id: SBox.cpp,v 1.12 2010/04/25 04:44:07 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string>
#include <strutils.h>
#include <file_utils.h>
#include <envparm.h>

static std::string url_base;
static std::string qproc_script;
static bool error;
static std::string err_msg;

static void print_html_header() {
  FCGI_printf("Content-Type:text/html;charset=iso-8859-1\r\n\r\n"
              "<html>\n<head>\n"
              "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"
              "<title>QUestion Guided SEarch Engine</title>\n"
              "</head>\n<body>\n");
}

static void print_html_footer() {
  FCGI_printf("</body>\n</html>\n");
}

static void print_error_msg(const char* msg) {
  FCGI_printf("<table border=\"0\" height=\"100%%\" width=\"100%%\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n"
              "<tbody>\n<tr>\n<td align=\"center\" valign=\"middle\">\n");
  FCGI_printf("<b>System configuration error: %s</b><br>\n", msg);
  FCGI_printf("</td>\n</tr>\n</tbody>\n</table>\n");
}

static void print_sbox_header() {
  std::string action = url_base + qproc_script;
  FCGI_printf("<table border=\"0\" height=\"100%%\" width=\"100%%\" cellpadding=\"0\" style=\"border-collapse:collapse\">\n"
              "<tbody>\n<tr>\n<td height=\"260px\" valign=\"middle\" align=\"center\">\n<img src=\"quse_logo.jpg\">\n</td>\n</tr>\n"
              "<tr>\n<td align=\"center\" valign=\"top\">\n"
              "<table border=\"1\" height=\"40\" width=\"660\" cellpadding=\"4\" bgcolor=\"#ffe2b1\" style=\"border-collapse:collapse\">\n"
              "<tbody>\n<tr>\n<td align=\"center\" valign=\"middle\" nowrap=\"true\">\n"
              "<form method=\"get\" accept-charset=\"iso-8859-1\" action=\"%s\">\n"
              "<div style=\"font-family:arial,sans-serif;font-size:11pt\">Enter keyword query:&nbsp;&nbsp;<input type=\"text\" style=\"font-family:arial,sans-serif;font-size:11pt;\" name=\"query\" size=\"60\" maxlength=\"80\" tabindex=\"1\" align=\"middle\">&nbsp;&nbsp;\n"
              "<input style=\"font-family:arial,sans-serif;font-size:11pt;\" type=\"submit\" value=\"Search\" size=\"12\">\n"
              "</div>\n</form>\n</td>\n</tr>\n</tbody>\n</table>\n"
              "<table border=\"0\" height=\"10\" width=\"800\" style=\"border-collapse:collapse\">\n"
              "<tbody>\n<tr>\n<td height=\"20px\">\n</td>\n</tr>\n"
              "<tr>\n<td height=\"10px\">\n</td>\n</tr>\n</tbody>\n</table>\n", action.c_str());
}

static void print_sbox_footer() {
  FCGI_printf("</td>\n</tr>\n</tbody>\n</table>\n");
}

static void initialize(const char *conf_path) {
  std::string host;
  std::string db_name;
  std::string user;
  std::string passwd;

  try {
    cmlib::SectionMap *conf = cmlib::read_config_file(conf_path);
    if(!cmlib::get_param_value(db_name, conf, "db", "db_name") || !cmlib::get_param_value(host, conf, "db", "host") ||
       !cmlib::get_param_value(user, conf, "db", "user") || !cmlib::get_param_value(passwd, conf, "db", "passwd" ) ||
       !cmlib::get_param_value(url_base, conf, "system", "url_base") || !cmlib::get_param_value(qproc_script, conf, "qproc", "script")) {
      err_msg = "configuration file is missing required parameter(s)";
      error = true;
      delete conf;
      return;
    }
    delete conf;
  } catch(cmlib::Exception &e) {
    err_msg = e.what();
    error = true;
  }
}

int main(void) {

  const char *conf_path = getenv("QUSE_CONF");
  if(conf_path == NULL) {
    err_msg = "environment variable QUSE_CONF is not defined";
    error = true;
  } else {
    initialize(conf_path);
  }

  while(FCGI_Accept() >= 0) {
    print_html_header();
    if(error) {
      print_error_msg(err_msg.c_str());
      print_html_footer();
    } else {
      print_sbox_header();
      print_sbox_footer();
      print_html_footer();
    }
  }

  return 0;
}


