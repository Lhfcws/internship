#ifndef BMSWORKERFACTORY_HPP
#define BMSWORKERFACTORY_HPP

#include "BMS.h"
#include "BMSWorker.h"


BMS_BEGIN;

// Singleton
class BMSWorkerFactory {
    private:
        BMSWorkerFactory(){}
        static BMSWorkerFactory* _factory;
        static int _count;
    public:
        ~BMSWorkerFactory();
        static void init();

        static BMSWorkerFactory* getInstance();

        int getWorkerNum() const;

        BMSWorker* makeWorker();

        void destroyWorker(BMSWorker* &worker);

        static void destroy();
};


BMS_END;
#endif
