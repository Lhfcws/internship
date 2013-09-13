#ifndef BMS_H
#define BMS_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>  
#include <netinet/in.h>

#include "common/common.h"
#include "sche/Query.h"
#include "sche/ConnectionPool.h"
#include "blender/blender.h"
#include "blender/BlenderSort.h"
#include "blender/DocumentRewrite.h"
#include "blender/QueryRewrite.h"
#include "blender/MemPoolFactory.h"
#include "blender/ISearchResultProcessor.h"
#include "detail/rf/ResultFormatFactory.h"
#include "detail/detail.h"
#include "searcher/searcher.h"
#include "clustermap/CMNode.h"
#include "sort/IBaseSort.h"
#include "secore/search/Ors.h"
#include "secore/search/QueryParameter.h"
#include "secore/search/SearchResult.h"
#include "secore/search/SearchResultSerializer.h"
#include "util/vsa/VSAContainer.h"
#include "anet/anet.h"
#include "anet/tcpconnection.h"
#include <vector>

#include "util/configure.h"

#define BMS_BEGIN namespace BMS {
#define BMS_END }

#define QUERY_PREFIX "/bin/search?"

#define BMS_VERSION "v0.1"

static const char* DEFAULT_CLUSTERNAME = "s_0_0";

BMS_BEGIN;


// Input Protocol Definition
struct InputProtocol {
    clustermap::enodetype type;
    char* ip;
    int port;
    clustermap::eprotocol protocol;     // "tcp", "http", "udp"
    char* orig;
    InputProtocol():type(clustermap::self), ip(NULL), port(0), protocol(clustermap::tcp), orig(NULL){}
};

struct PLIST {
    clustermap::CMISNode* node;
    int count;
    struct PLIST* next;

    PLIST():count(1), next(NULL) {
        node = new clustermap::CMISNode();
    }

    ~PLIST() {
        if (node)
            delete node;
        if (next)
            delete next;
    }
};

BMS_END;
#endif
