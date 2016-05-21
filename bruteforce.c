#include <openssl/evp.h>
#include <string.h>
#include "options.h"
#include "configure.h"
#include "bruteforce.h"
#include "queue.h"
#include "crack.h"

void * brute_thread (void * arg)
{
  struct producer_consumer_queue *q = (typeof(q))arg;
  char pswd[opt.bruteMaxLen];
  char currentHash[EVP_MAX_MD_SIZE * 2 + 1];

  EVP_MD_CTX * mdctx = NULL;
  const EVP_MD * md = NULL;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;

  md = EVP_get_digestbyname(typecheck(opt.hash));

  while (1)
  {
    if (found == 1) break;

    char * buffer = dequeue(q);
    // это сигнал, что очередь окончена
    if (!buffer) break; // значит, пора закрывать поток

    int pswd_counter = 0;
    int bufLen = strlen(buffer);

    for (int i = 0; i < bufLen; i++)
    {
            if (buffer[i] != ' ')
            {
                pswd[pswd_counter] = buffer[i];
                pswd_counter++;
            }
            else
            {
                pswd[pswd_counter] = '\0';

                doHash(&mdctx, md, &md_len, &md_value, pswd);

                for (unsigned int i = 0; i < md_len; i++)
                {
                    sprintf(&currentHash[i * 2], "%02x", md_value[i]);
                }

                if (strcmp(currentHash, opt.hash) == 0)
                {
                    found = 1;
                    crackedPswd = malloc(strlen(pswd) + 1);
                    strcpy(crackedPswd, pswd);
                    break;
                }
                pswd_counter = 0;
            }
        }
        free(buffer);
      }
  return 0;
}

void generateWords(struct producer_consumer_queue ** q)
{
    int   alphaLen = strlen(conf.alphabet);

    int letters[opt.bruteMaxLen];

    char * buffer   = (char *)malloc((opt.bruteMaxLen + 1) * alphaLen * alphaLen);

    for (int len = 1;len <= opt.bruteMaxLen; len++)
    {
        if (found) break;

        int i;
        int stride = len + 1;
        int bufLen = stride * alphaLen * alphaLen;

        if (len == 1)
        {
            bufLen = (len + 1) * alphaLen;

            generateOne(&buffer);

            enqueue(buffer, *q);
            continue;
        }

        memset(buffer, conf.alphabet[0], bufLen);

        generateTwo(&buffer, stride, bufLen, len);

        if (len == 2)
        {
            enqueue(buffer, *q);
            continue;
        }

        for (i = 0; i < len; i++)
        {
            letters[i] = 0;
        }

        i = len-3;

        do {
            generateLong(i, &buffer, letters, stride, bufLen);

            enqueue(buffer, *q);

            if (letters[i] != 0)
            {
                i = len - 3;
                continue;
            }

            i--;

            if (i < 0) break;

        } while(!found);
    }
    free(buffer);
    return;
}

void generateOne(char ** buffer)
{
    int alphaLen = strlen(conf.alphabet);
    int j = 0;

    for (int i = 0; i < alphaLen; i++)
    {
        (*buffer)[j++] =conf.alphabet[i];
        (*buffer)[j++] = ' ';
    }
}

void generateTwo(char ** buffer, int stride, int bufLen, int len)
{
    int let0 = 0;
    int let1 = 0;
    int alphaLen = strlen(conf.alphabet);

    for (int i = len-2; i < bufLen; i += stride)
    {
        (*buffer)[i]   = conf.alphabet[let0];
        (*buffer)[i+1] = conf.alphabet[let1++];
        (*buffer)[i+2] = ' ';

        if (let1 == alphaLen)
        {
            let1 = 0;
            let0++;

            if (let0 == alphaLen)
            {
                let0 = 0;
            }
        }
    }
}

void generateLong(int i, char ** buffer, int letters[],
                  int stride, int bufLen)
{
    char c;
    int  j;
    int alphaLen = strlen(conf.alphabet);

    letters[i]++;

    if (letters[i] >= alphaLen){
        letters[i] = 0;
    }

    c = conf.alphabet[letters[i]];

    for (j = i; j < bufLen; j += stride)
    {
        (*buffer)[j] = c;
    }
}
