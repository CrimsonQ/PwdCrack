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

void doHash(EVP_MD_CTX ** mdctx, const EVP_MD * md, unsigned int * md_len,
            unsigned char ** md_value, const char * pswd);

void dictionaryAttack(pthread_t * threads, int ncores);

void bruteforceAttack(pthread_t * threads, int ncores);

void measureRuntime(pthread_t * threads, int ncores);

double convertTime(double seconds);

#endif // CRACK_H
