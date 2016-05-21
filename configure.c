#include "stdio.h"
#include "configure.h"
#include <hwloc.h>

void getNumCores(int * num)
{
    hwloc_topology_t topology;
    hwloc_topology_init(&topology);
    hwloc_topology_load(topology);

    int depth = hwloc_get_type_depth(topology, HWLOC_OBJ_CORE);

    if(depth == HWLOC_TYPE_DEPTH_UNKNOWN)
    {
      printf("The number of cores is unknown. Use one\n");
      num = 1;
    }
    else
    {
      *num = hwloc_get_nbobjs_by_depth(topology, depth);
    }

    hwloc_topology_destroy(topology);
}


int getConfig(config * confStruct, char * filename)
{
    FILE * file = fopen (filename, "r");

    if (file != NULL)
    {
        char line[MAXBUF];
        int i = 0;

        // считываем первую строку из конф. файла
        while(fgets(line, sizeof(line), file) != NULL)
        {
            char * cfline = NULL;
            char intStr[1];
            // ищем в полученной строке первое вхождение разделителя
            cfline = strstr((char *)line, DELIM);
            cfline = cfline + strlen(DELIM);

            switch(i){
            case 0:
                memcpy(confStruct->dictionaryPath, cfline, strlen(cfline));
                confStruct->dictionaryPath[strlen(cfline) - 1] = '\0';
                break;
            case 1:
                memcpy(confStruct->alphabet, cfline, strlen(cfline));
                confStruct->alphabet[strlen(cfline) - 1] = '\0';
                break;
            case 2:
                memcpy(intStr, cfline, strlen(cfline));
                confStruct->maxDict = atoi(intStr);
                break;
            case 3:
                memcpy(intStr, cfline, strlen(cfline));
                confStruct->maxPswdLenDict = atoi(intStr);
                break;
            case 4:
                memcpy(intStr, cfline, strlen(cfline));
                confStruct->testIterationsNumber = atoi(intStr);
                break;
            }
            i++;
        } // End while

        fclose(file);
    } // End if file
    else
    {
        return 1;
    }
    return 0;
}
