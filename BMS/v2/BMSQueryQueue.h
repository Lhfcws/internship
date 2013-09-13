#ifndef BMSQUERYQUEUE_H
#define BMSQUERYQUEUE_H

#include "BMS.h"
#include <deque>
#include <string>
#include <semaphore.h>

BMS_BEGIN;


class BMSQueryQueue {
    private:
        static std::deque<std::string> _queue;
        static BMSQueryQueue* self;
        static sem_t count;

    public:
        static void init();

        static BMSQueryQueue* getInstance();

        ~BMSQueryQueue();

        static void destroy();

        void push(char* query);

        void pop(char* &query);

};


BMS_END;

#endif
