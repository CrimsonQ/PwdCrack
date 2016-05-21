#ifndef CONFIGURE_H
#define CONFIGURE_H

#include "options.h"

typedef enum
{
    MD5_LENGTH = 32
   ,SHA1_LENGTH = 40
   ,SHA256_LENGTH = 64
   ,SHA512_LENGTH = 128

} digestLengths;

//---------Conf file options--------//
#define CONF_FILENAME "config.conf"
#define MAXBUF 1024
#define DELIM "="

typedef struct{
    char dictionaryPath[MAXBUF];
    char alphabet[MAXBUF];
    int maxDict;
    int maxPswdLenDict;
    int testIterationsNumber;
} config;

int getConfig(config * confStruct, char * filename);

config conf;
//---------------------------------//

void getNumCores(int * num);

#endif // CONFIGURE_H
