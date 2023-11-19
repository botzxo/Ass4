#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Include the header files required here:
// #include "your_kwic.h"
// #include "your_multimap.h"
// and put these constants in there too:
#define DEFAULT_INDEX_SIZE 1000
#define DEFAULT_ENTRY_LENGTH 40
// and remove this one (it will come from your_multimap.h):
#define MAX_VALUE_LENGTH 81

int main(int argc, char *argv[]) {
  int result = 0;
  int opt;

  char *in_file = NULL, *out_file = NULL, *stop_file = NULL;
  int max_index = DEFAULT_INDEX_SIZE, entry_length = DEFAULT_ENTRY_LENGTH;
  double max_frequency = 1;
  char *end_p;

  while ((0 == result) && (opt = getopt(argc, argv, "f:l:m:o:s:")) != -1) {
    switch (opt) {
    case 'f':
      max_frequency = strtod(optarg, &end_p);
      if (end_p == optarg || max_frequency < 0 || max_frequency > 1) {
        fprintf(stderr, "Maximum frequency must be a number between 0 and 1\n");
        max_frequency = 1;
      }
      break;
    case 'l':
      entry_length = strtol(optarg, &end_p, 10);
      if (end_p == optarg || entry_length <= 0 || entry_length >= MAX_VALUE_LENGTH) {
        fprintf(stderr, "Entry length must be between 0 and %d\n", MAX_VALUE_LENGTH-1);
        entry_length = DEFAULT_ENTRY_LENGTH;
      }
      break;
    case 'm':
      max_index = strtol(optarg, &end_p, 10);
      if (end_p == optarg || max_index <= 0) {
        fprintf(stderr, "Maximum index length must be greater than zero\n");
        max_index = DEFAULT_INDEX_SIZE;
      }
      break;
    case 'o':
      out_file = optarg;
      break;
    case 's':
      stop_file = optarg;
      break;
    default:
      result = 1;
      break;
    }
  }
  
  if (result != 0 || argc > optind + 1) {
    fprintf(stderr, "Usage: %s [-f max_frequency] [-l entry_length] [-m max_index] [-o outfile.txt] [-s stopwords.txt] [infile.txt]\n", argv[0]);
  } else {
    if (argc > optind) {
      in_file = argv[optind];
    }
    
    // Replace this, calling a function and passing the parameters:
    printf("in_file = %s\n", in_file);
    printf("out_file = %s\n", out_file);
    printf("stop_file = %s\n", stop_file);
    printf("max_frequency = %f\n", max_frequency);
    printf("entry_length = %d\n", entry_length);
    printf("max_index = %d\n", max_index);
    
    // For example:
    // your_kwic(in_file, out_file, stop_file, max_frequency, entry_length, max_index);
  }
  
  return result;
}
