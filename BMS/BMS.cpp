/**
 * @author: Lhfcws Wu
 **/


#include "BMSServer.h"
#include "validation.h"
#include "CLParser.h"


BMS::BMSServer server;

// ====================================================================
void help(const std::string& null_para) {
    printf("BLENDER MOCKER SHELL ");
    printf(BMS_VERSION);
    printf("\n");
    printf("Author:  Lhfcws Wu\n");
    printf("Time:    2013-08-28\n");

    printf("Example: ./BMS -s tcp:10.20.150.11:8080,tcp:10.20.150.12:8080 -d tcp:222.200.200.11:8008 -f querylist\n");
    printf("Options:\n");
    printf("  -h(--help)        Print help information.\n");
    printf("  -s(--searcher)    Assign searcher host(s).\n");
    printf("  -d(--detailer)    Assign detailer host(s).\n");
    printf("  -f(--file)        Assign a query file.\n");
    printf("  -w(--worker)      Assign parallel worker number. [1]\n");
    printf("\n");

    exit(0);
}

void load_searcher(const std::string& searcher_list) {
    size_t spos, epos;
    spos = epos = 0;
    char* searcher;
    ret_t ret;
        
    while (epos != std::string::npos) {
        epos = searcher_list.find(',');
        if (epos != std::string::npos) {
            searcher = strdup(searcher_list.substr(spos, epos - spos + 1).c_str());
        }
        else {
            searcher = strdup(searcher_list.substr(spos).c_str());
        }
        BMS::InputProtocol iprtcl;
        ret = BMS::formatInputProtocol('s', searcher, iprtcl);
        (server._locator)->setCMNode(iprtcl);
        ::free(searcher);
        spos = epos;
    }
}

void load_detailer(const std::string& detailer_list) {
    size_t spos, epos;
    spos = epos = 0;
    char* detailer;
    ret_t ret;
        
    while (epos != std::string::npos) {
        epos = detailer_list.find(',');
        if (epos != std::string::npos) {
            detailer = strdup(detailer_list.substr(spos, epos - spos + 1).c_str());
        }
        else {
            detailer = strdup(detailer_list.substr(spos).c_str());
        }
        BMS::InputProtocol iprtcl;
        ret = BMS::formatInputProtocol('d', detailer, iprtcl);
        (server._locator)->setCMNode(iprtcl);
        ::free(detailer);
        spos = epos;
    }
}

void load_queryfile(const std::string& filename) {
    FILE* fp;
    fp = fopen(filename.c_str(), "r");

    char* tp_query = (char*) ::malloc(sizeof(char) * 1600);
    while (NULL != fgets(tp_query, 1600, fp)) {

        char *input_query = strdup(tp_query);

        server.recvQuery(input_query);
    }
    ::free(tp_query);
    fclose(fp);
}

void set_worker_num(const std::string& s_num) {
    int num = atoi(s_num.c_str());
    server.setWorkerNum(num);
}


// ====================================================================
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

    CLP::CLParser parser;
    server.init();

    parser.add_parameter("-h", "--help", &help);
    parser.add_parameter("-s", "--searcher", &load_searcher);
    parser.add_parameter("-d", "--detailer", &load_detailer);
    parser.add_parameter("-f", "--file", &load_queryfile);
    parser.add_parameter("-w", "--worker", &set_worker_num);

    parser.parse(argc, argv);

    server.start();
    server.wait();

    printf("Exiting Blender Mocker Shell..\n");
    exit(0);
    return 0;
}
