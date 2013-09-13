#ifndef VALIDATION_H
#define VALIDATION_H

#include "BMS.h"
#include <arpa/inet.h>

#define VALIDATION_BEGIN namespace VALD {
#define VALIDATION_END }

BMS_BEGIN;

// check arguments completion
bool checkArgvCompletion(char* query, char* searcher_protocol, char* detailer_protocol) {
    if (query == NULL || searcher_protocol == NULL  || detailer_protocol == NULL )
        return false;

    return true;
}

// check IP valid
bool checkIP(char* ip_addr) {
    if (!ip_addr || inet_addr(ip_addr) == -1)
        return false;
    return true;
}

// check Input Protocol completion
bool checkInputProtocol(const BMS::InputProtocol& iprtcl_) {
    bool bo = true;
    bo &= (iprtcl_.type < clustermap::self);
    bo &= checkIP(iprtcl_.ip);
    bo &= (iprtcl_.port > 1024);
//    bo &= (strcmp(iprtcl_.protocol, "tcp") == 0 || strcmp(iprtcl_.protocol, "http") == 0 || strcmp(iprtcl_.protocol, "udp") == 0);
    bo &= (iprtcl_.protocol >= 0 && iprtcl_.protocol <= 2);
    bo &= (iprtcl_.orig != NULL);

    return bo;
}

ret_t formatInputProtocol(char type_, char* orig_, InputProtocol& iprtcl) {

    // Assign protocol
    iprtcl.orig = orig_;
    char* ptr = strchr(orig_, ':');
    char* tp_protocol = (char *) ::malloc(sizeof(char) * (ptr - orig_ + 1));
    if (!tp_protocol) {
        return r_enomem;
    }

    strncpy(tp_protocol, orig_, ptr - orig_);
    tp_protocol[ptr - orig_] = 0x00;

    if (strcmp(tp_protocol, "tcp") == 0)
        iprtcl.protocol = clustermap::tcp;
    else if (strcmp(tp_protocol, "http") == 0)
        iprtcl.protocol = clustermap::http;
    else if (strcmp(tp_protocol, "udp") == 0)
        iprtcl.protocol = clustermap::udp;
    else
        return r_failed;

    // Assign ip
    orig_ = ptr + 1;
    ptr = strchr(orig_, ':');
    iprtcl.ip = (char *) ::malloc(sizeof(char) * (ptr - orig_ + 1));
    if (!iprtcl.ip) {
        return r_enomem;
    }

    strncpy(iprtcl.ip, orig_, ptr - orig_);
    iprtcl.ip[ptr - orig_] = 0x00;

    // Assign port
    orig_ = ptr + 1;
    iprtcl.port = atoi(orig_);
    if (iprtcl.port == 0) {
        ::free(iprtcl.ip);
        return r_failed;
    }

    // Assign nodetype
    switch (type_) {
        case 's': iprtcl.type = clustermap::search; break;
        case 'd': iprtcl.type = clustermap::search_doc; break;
        case 'b': iprtcl.type = clustermap::blender; break;
        default: 
                 ::free(iprtcl.ip);
                 return r_failed;
                 break;
    };


    if (checkInputProtocol(iprtcl)) {
        return r_succeed;
    }
    else
        return r_failed;
}


BMS_END;
#endif
