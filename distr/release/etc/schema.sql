/* -*- SQL -*- */

/* $Id: schema.sql,v 1.10 2009/10/27 20:55:34 akotov2 Exp $ */

/* If database already exists, we delete it */
DROP DATABASE IF EXISTS quse;

/* Create the database */
CREATE DATABASE quse;

/* Deleting the user, if he existed */
DELETE FROM mysql.user WHERE User='quse';

DROP USER 'quse'@'localhost';

DROP USER 'quse'@'%';

/* Create a user with access to the DB from the local machine */
CREATE USER 'quse'@'localhost' IDENTIFIED BY 'TIManUIUC';

/* Create a user with access to the DB from the network */
CREATE USER 'quse'@'%' IDENTIFIED BY 'TIManUIUC';

/* Giving the newly created user all privileges for the database */
GRANT ALL ON quse.* TO 'quse';

/* Switching to the newly created database */
USE quse;

/*
   Dictionary of known terms:
     * id - identifier of a term;
     * term - the term itself.
*/
CREATE TABLE dictionary(
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  term VARCHAR(128) NOT NULL,
  PRIMARY KEY(id)
);

ALTER TABLE dictionary ADD INDEX term_ind (term);

/* 
   Repository of known URLs:
     * id - identifier of a URL;
     * word_count - number of words in a document (document length);
     * path - absolute path to the file.
*/ 
CREATE TABLE documents(
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  word_count INT UNSIGNED NOT NULL DEFAULT 0,
  path VARCHAR(512) NOT NULL,
  indexed ENUM('0','1') NOT NULL DEFAULT '0',
  PRIMARY KEY(id)
);

ALTER TABLE documents ADD INDEX path_ind (path);

/*
   Parsed document sentences:
     * sent_id - identifier of a sentence;
     * doc_id - identifier of a document containing sentence;
     * sentence - sentence itself.
*/
CREATE TABLE sentences(
  sent_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  doc_id INT UNSIGNED NOT NULL,
  sentence VARCHAR(1024) NOT NULL,
  PRIMARY KEY(sent_id),
  FOREIGN KEY(doc_id) REFERENCES documents(id) ON DELETE CASCADE
);

/*
   Index of instances of syntactic patterns:
     * inst_id - identifier of an instance;
     * doc_id - identifier of a document;
     * sent_id - identifier of a sentence;
     * term_id - identifier of a term;
     * stem_id - identifier of a stem;
     * pat_id - identifier of a pattern;
     * slot_id - identifier of a slot in a pattern;
     * slot_pos - position within a slot
*/
CREATE TABLE instances(
  inst_id INT UNSIGNED NOT NULL,
  doc_id INT UNSIGNED NOT NULL,
  sent_id INT UNSIGNED NOT NULL,
  term_id INT UNSIGNED NOT NULL,
  stem_id INT UNSIGNED NOT NULL,
  pat_id INT UNSIGNED NOT NULL,
  slot_id INT UNSIGNED NOT NULL,
  slot_pos INT UNSIGNED NOT NULL,
  FOREIGN KEY(doc_id) REFERENCES documents(id) ON DELETE CASCADE, 
  FOREIGN KEY(sent_id) REFERENCES corpus(sent_id) ON DELETE CASCADE,
  FOREIGN KEY(term_id) REFERENCES dictionary(id) ON DELETE CASCADE,
  FOREIGN KEY(stem_id) REFERENCES dictionary(id) ON DELETE CASCADE
);

ALTER TABLE instances ADD INDEX stem_ind (stem_id);
ALTER TABLE instances ADD INDEX inst_ind (inst_id);
ALTER TABLE instances ADD INDEX stem_doc_ind (doc_id,stem_id);

/* 
   Registry of search sessions:
     * id - identifier of a sessions;
     * qtype - query type:
               * 0 - no type (default);
               * 1 - short, under-specified (SQ);
               * 2 - long, over-specified (LQ);
               * 3 - queries without NE (NQ);
               * 4 - queries involving multiple NE (EQ).
     * fb - flag showing that a session is a feedback session
     * query - query submitted during the session
*/  
CREATE TABLE sessions(
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  type ENUM('0','1','2','3','4') NOT NULL DEFAULT '0',
  fb ENUM('0','1') NOT NULL DEFAULT '0',
  query VARCHAR(256),
  PRIMARY KEY(id)
);

ALTER TABLE sessions ADD INDEX query_ind (query);

/*
   Registry of questions and their judgments
     * id - identifier of a question;
     * orig_id - ID of the original question (if it is a feedback question, 0 - otherwise)
     * inst_id - ID of instance corresponding to a question;
     * sess_id - ID of a session during the question was presented;
     * rank - rank of a question during the session;
     * qterms - how many query terms does the question include; 
     * matches - how many query terms did an instance match;
     * score - score of a document corresponding to a question;
     * quest - the question itself;
     * shown - flag showing that a question had been shown;
     * wellf - flag showing that a question had been judged as well-formed;
     * inter - flag showing that a question had been judged as interesting;
     * relev - flag showing that a question had been judged as relevant;
     * clicked - flag showing that a question had been clicked.
*/  

CREATE TABLE questions(
  id INT UNSIGNED NOT NULL AUTO_INCREMENT,
  inst_id INT UNSIGNED NOT NULL,
  sess_id INT UNSIGNED NOT NULL,
  rank INT UNSIGNED NOT NULL,
  qterms INT UNSIGNED NOT NULL,
  matches INT UNSIGNED NOT NULL,
  score DOUBLE NOT NULL,
  quest VARCHAR(512) NOT NULL,
  shown ENUM('0','1') NOT NULL DEFAULT '0',
  wellf ENUM('0','1') NOT NULL DEFAULT '0',
  inter ENUM('0','1') NOT NULL DEFAULT '0',
  relev ENUM('0','1') NOT NULL DEFAULT '0',
  clicked ENUM('0','1') NOT NULL DEFAULT '0',
  origq_id INT UNSIGNED NOT NULL,
  PRIMARY KEY(id),
  FOREIGN KEY(origq_id) REFERENCES questions(id) ON DELETE CASCADE,
  FOREIGN KEY(sess_id) REFERENCES sessions(id) ON DELETE CASCADE,
  FOREIGN KEY(inst_id) REFERENCES instances(inst_id) ON DELETE CASCADE
);

ALTER TABLE questions ADD INDEX sess_quest_ind (sess_id,rank);

/* 

/* 
   Suggestions for queries:
     * query - suggested query;
          
*/

CREATE TABLE suggestions(
  type ENUM('0','1','2','3','4') NOT NULL DEFAULT '0',
  query VARCHAR(256)
);
