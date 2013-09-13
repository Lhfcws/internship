#include "BMSQueryQueue.h"

BMS_BEGIN;

std::deque<std::string> BMSQueryQueue::_queue;
BMSQueryQueue* BMSQueryQueue::self = NULL;
sem_t BMSQueryQueue::count;

void BMSQueryQueue::init() {
    self = NULL;
    _queue.clear();
    sem_init(&count, 0, 0);
}

BMSQueryQueue* BMSQueryQueue::getInstance() {
    if (!self) {
        self = new BMSQueryQueue();
    }
    return self;
}

BMSQueryQueue::~BMSQueryQueue() {}

void BMSQueryQueue::destroy() {
    sem_destroy(&count);
    delete self;
    self = NULL;
    _queue.clear();
}

void BMSQueryQueue::push(char* query) {


    // Crtical Section
    std::string str = query;
    _queue.push_front(str);
    // Section end
    sem_post(&count);
}

void BMSQueryQueue::pop(char* &query) {
    sem_wait(&count);

    // Crtical Section
    std::string str = _queue.back();
    query = strdup(str.c_str());
    _queue.pop_back();
    // Section end

}
BMS_END;
