#include "BMSServer.h"

BMS_BEGIN;

BMSServer::BMSServer() {
    _w_factory = NULL;
    _worker_num = 1;
    _locator = NULL;
}

BMSServer::~BMSServer() {
    if (_w_factory) {
        delete _w_factory;
    }
}

void BMSServer::setWorkerNum(int num) {
    _worker_num = num;
}

void BMSServer::init() {
    // Init
    _w_factory = BMSWorkerFactory::getInstance();
    _locator = new ServerLocator();
    BMSQueryQueue::init();

}

void BMSServer::start() {
    // create workers
    for (int i = 0; i < _worker_num; i++) {
        BMSWorker* worker = _w_factory->makeWorker();
        worker->_locator = _locator;

        pthread_t pid;
        pthread_create(&pid, NULL, runWorker, (void *)worker);
    }
}

void BMSServer::wait() {
    while (true) {}
}

void BMSServer::recvQuery(char* query) {
    (BMSQueryQueue::getInstance())->push(query);
}

void* runWorker(void* args) {
    BMSWorker* worker = (BMSWorker *)args;
    worker->run();
}

BMS_END;
