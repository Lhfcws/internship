#ifndef BMSSERVER_H
#define BMSSERVER_H

#include "BMS.h"
#include "BMSWorker.h"
#include "BMSWorkerFactory.h"
#include "BMSCommCenter.h"

BMS_BEGIN;

// Handler for pthread_create
static void* runWorker(void* args);

class BMSServer {
    public:
        BMSServer();
        ~BMSServer();

        void init();
        void start();
        void wait();
        void recvQuery(char* query);
        void setWorkerNum(int num);

        // init, register && subscribe
        ret_t initCMClient(clustermap::enodetype type, const char* cfg_path);

        /** 
        * Look up if there is any role that does not own at least one server.
        * I move it into BMSWorker, as I consider that the nodes allocated everytime should be decided by ClusterMap. 
        * CM may need to balance load, monitor heartbeats and so on, so it should be dynamic instead of allocating nodes ahead in the server.
        **/
        // void allocNodesFromCM();

        // void run();   Temporarily not use run(), use wait() instead.
        ServerLocator* _locator;
    private:
        BMSWorkerFactory *_w_factory;
        int _worker_num;

        // Global config, shared by the workers. Actually I'm thinking of creating a configure loader singleton class to manage all the configure.
        config_t* _root;
        BMSQueryRewrite _qrw;
        SORT::BlenderSort _sort;
        APP_DETAIL::ResultFormatFactory _rf_factory;

        clustermap::CMClient* _cm_client[clustermap::self + 1];
        anet::Transport _transport;
};

BMS_END;
#endif
