#ifndef CRACK_H
#define CRACK_H

#include <pthread.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include "dictionary.h"
#include "bruteforce.h"
#include "options.h"
#include "configure.h"
#include "queue.h"

extern char * crackedPswd;

extern int found;

extern int test;

extern unsigned long int progress;

void doHash(EVP_MD_CTX ** mdctx, const EVP_MD * md, unsigned int * md_len,
            unsigned char ** md_value, const char * pswd);

void dictionaryAttack(pthread_t * threads, int ncores);

void bruteforceAttack(pthread_t * threads, int ncores);

void measureBruteRuntime(pthread_t * threads, int ncores);

void measureDictRuntime(pthread_t * threads, int ncores);

void printConvertedTime(double seconds);

#endif // CRACK_H
