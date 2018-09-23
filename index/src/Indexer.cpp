/* -*- C++ -*- */

// $Id: Indexer.cpp,v 1.6 2009/10/27 20:55:34 akotov2 Exp $
// Alexander Kotov (alex.s.kotov@gmail.com) for UIUC TIMan group, 2009

// Main file of the indexing utility

#include <stdio.h>
#include <except.h>
#include <string.h>
#include <exception>
#include <string>
#include <IndexWriter.hpp>

void print_usage() {
  printf("Usage:\n indexer -c conf_file -d data_dir -t temp_dir [-v]\n");
  printf("Arguments:\n");
  printf("  -c - path to the system configuration file\n");
  printf("  -d - path to the directory with data files\n");
  printf("  -t - directory for storing temporary information during indexing\n");
}

int main(int argc, char* argv[])
{
  bool verbose = false;
  const char *conf_file = NULL, *data_dir = NULL, *temp_dir = NULL;

  if(argc == 1) {
    print_usage();
    return 1;
  }

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-c") == 0) {
      if((i+1) < argc)
        conf_file = argv[i+1];
      else {
        fprintf(stderr, "Missing configuration file name after -c\n");
        print_usage();
        return 1;
      }
    }
    else if(strcmp(argv[i], "-d") == 0) {
      if((i+1) < argc)
        data_dir = argv[i+1];
      else {
        fprintf(stderr, "Missing data directory name after -d\n");
        print_usage();
        return 1;
      }
    }
    else if(strcmp(argv[i], "-t") == 0) {
      if((i+1) < argc)
        temp_dir = argv[i+1];
      else {
        fprintf(stderr, "Missing temporary directory name after -t\n");
        print_usage();
        return 1;
      }
    }
    else if(strcmp(argv[i], "-v") == 0) {
      verbose = true;
    }
  }

  if(conf_file == NULL || data_dir == NULL || temp_dir == NULL) {
    printf("Missing required parameters\n");
    print_usage();
    return 1;
  }

  IndexWriter index(conf_file, temp_dir, verbose);

  try {
    if(verbose)
      printf("Initializing...\n");
    index.init();
    if(verbose) {
      printf("Initialization complete!\n");
      printf("Indexing directory...\n");
    }
    index.index_dir(data_dir);
    if(verbose) {
      printf("Indexing complete!\n");
    }
  } catch(std::exception& e) {
    fprintf(stderr, "Error while indexing: %s...\n", e.what());
    return 1;
  } catch(...) {
    fprintf(stderr, "Unknown error when indexing!\n");
    return 1;
  }

  return 0;
}
