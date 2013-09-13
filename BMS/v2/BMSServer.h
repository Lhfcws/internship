#ifndef BMSSERVER_H
#define BMSSERVER_H

#include "BMS.h"
#include "BMSWorker.h"
#include "BMSWorkerFactory.h"
#include "BMSCommCenter.h"

BMS_BEGIN;

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

    // void run();   Temporarily not use run().
    ServerLocator* _locator;
private:
    BMSWorkerFactory *_w_factory;
    int _worker_num;

    anet::Transport _transport;
    SCHE::ConnectionPool *_conn_pool;
};

BMS_END;
#endif
