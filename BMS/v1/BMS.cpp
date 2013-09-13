/**
 * @author: Lhfcws Wu
 **/


#include "BMSHandler.h"
#include "validation.h"


BMS::BMSHandler bmsHandler;
char* input_query;

inline bool streq(char* s1, char* s2) {
    return strcmp(s1, s2) == 0;
}

void help() {
    printf("BLENDER MOCKER SHELL ");
    printf(BMS_VERSION);
    printf("\n");
    printf("Author:  Lhfcws Wu\n");
    printf("Time:    2013-08-12\n");

    printf("Example: ./BMS -s tcp:10.20.150.11:8080,tcp:10.20.150.12:8080 -d tcp:222.200.200.11:8008\n");
    printf("\n");

    exit(0);
}

void processArgv(int argc, char*argv[]) {
    int i = 1;
    // clustermap::enodetype ent;

    while (i < argc) {

        if (streq(argv[i], "-h"))
            help();

        else if (streq(argv[i], "-u")) {
            i++;
            if (i >= argc)
                help();

            input_query = argv[i];
        }

        else if (streq(argv[i], "-s") || streq(argv[i], "-d")) {
            /*
               ent = clustermap::search;       
               if (streq(argv[i], "-d")) 
               ent = clustermap::search_doc;
               */
            char type_ = argv[i][1];

            i++;
            ret_t ret = r_failed;
            while (i < argc) {
                BMS::InputProtocol iprtcl;
                ret = BMS::formatInputProtocol(type_, argv[i], iprtcl);
                if (ret != r_succeed)
                    break;

                bmsHandler._locator.setCMNode(iprtcl);
                i++;
            }
        }

        else i++;
    }
}

void get_a_query(const char* filename) {
    FILE* fp;
    fp = fopen(filename, "r");

    char* tp_query = (char*) ::malloc(sizeof(char) * 1600);
    fgets(tp_query, 1600, fp);

    input_query = strdup(tp_query);
    ::free(tp_query);

    while (input_query[strlen(input_query) - 1] == '\n')
        input_query[strlen(input_query) - 1] = 0x00;

    fclose(fp);
}

void logtime() {
    FILE* fp;
    fp = fopen("BMS.log", "a");

    time_t tt = time(NULL);
    tm* t= localtime(&tt);
    fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d\n",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
    fclose(fp);
}

int main(int argc, char* argv[]) {
    logtime();
    printf("Welcome to Blender Mocker Shell!!\n");

    processArgv(argc, argv);
    get_a_query("./querylist");
    printf("Command loaded!!\n");

    ret_t ret;
    ret = bmsHandler.init();
    printf("Init finished!!\n");

    if (ret == r_succeed) {
        ret = bmsHandler.handleNewRequest(input_query);
        printf("New Request handled!!\n");
    }

    if (ret == r_succeed) {
        ret = bmsHandler.handlePhase1Return();
        printf("Phase 1 Request handled!!\n");
    }

    if (ret == r_succeed) {
        ret = bmsHandler.handlePhase2Return();
        printf("Phase 2 Request handled!!\n");
    }

    printf("Exiting Blender Mocker Shell..\n");
    exit(0);
    return 0;
}
