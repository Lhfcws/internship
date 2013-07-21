#!/usr/bin/env python2.7
#-*- coding: utf8 -*-

def generate(ipsr, dep, start, end):
    if (dep > 3):
        fp1.write(ipsr+'\n')
        return
    for i in xrange(start, end+1):
        generate(ipsr + '.' + str(i), dep+1, 0, 255)


fp = open("ip_blacklist", "r")
fp1 = open("ip_blacklist1", "w")

for line in fp.readlines():
    pos = line.find('/')
    if -1 == pos:
        fp1.write(line)
        continue

    [_ip, mask] = line.split('/')
    ip = [int(x) for x in _ip.split('.')]
    
    mask = int(mask)
    n = mask / 8
    mod = 8 - mask % 8
    start = ip[n] >> mod << mod
    end = start | 255 >> (8 - mod)

    ls = []
    for i in xrange(n):
        ls.append(str(ip[i]))

    generate('.'.join(ls), n, start, end)


fp.close()
fp1.close()
