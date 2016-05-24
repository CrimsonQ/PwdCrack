#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include <openssl/evp.h>
#include "queue.h"
#include "options.h"

void * brute_thread (void * arg);

void doHash(EVP_MD_CTX ** mdctx, const EVP_MD * md, unsigned int * md_len,
            unsigned char ** md_value, const char * pswd);

void generateWords(struct producer_consumer_queue ** q);

void generateOne(char **buffer);

void generateTwo(char ** buffer, int stride, int bufLen, int len);

void generateLong(int i, char ** buffer, int letters[], int stride, int bufLen);

unsigned long int countWordsNum();

#endif // BRUTEFORCE_H
