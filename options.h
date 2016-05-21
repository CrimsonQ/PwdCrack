#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include <openssl/evp.h>
#include <getopt.h>

//-----Console options parsing-----//
typedef struct{
    _Bool bruteMode;
    _Bool dictionaryMode;
    int verbose;
    char hash[EVP_MAX_MD_SIZE * 2 + 1];
    char ** dictionaries;
    int bruteMaxLen;
    int dictCounter;
} options;

static const struct option longOpts[] = {
    { "dictionary", required_argument, NULL, 'd' },
    { "bruteforce", required_argument, NULL, 'b' },
    { "verbose", required_argument, NULL, 'v' },
    { "help", no_argument, NULL, 'h' },
    { NULL, no_argument, NULL, 0 }
};

options opt;

void help();

void setDefaults();

int add(char ** list, char * str, int * counter, const int maxNum);

void getOptions(int argc, char ** argv);

void getHashStdin();

char * typecheck(char * hash);

void printVerbose(int verbosity, const char * format, ...);
//---------------------------------//

#endif // OPTIONSPARSER_H
