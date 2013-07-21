#!/usr/bin/env python2.7
#-*- coding: utf-8 -*-

import random

## CONFIG
letters = [chr(i) for i in range(97, 97+26)]
son_domain = ['www', 'mail', 'crawl', 'edu', 'group', 'm']
suff = ['com', 'cn', 'net', 'org']

crawl_list = [
    '10.20.150.43',
    '8.9.9.9',
    '200.181.182.22',
    '123.123.22.0',
    '20.10.123.2',
]

Probability = [0, 1.0]    # [target, miss + target] => target: 0.3, miss: 0.7
MAX = 100000                # Test IP amount, 100k default

### FUNCTIONS

def rd_elem(ls):
    r = int(random.random() * len(ls) * len(ls))
    return ls[r % len(ls)]

def rd(n = 100):
    r = int(random.random() * 100000)
    return r % n

def domainname():
    length = 0
    while length == 0:
        length = rd(12)
    st = []

    for i in xrange(length):
        st.append(rd_elem(letters))

    return ''.join(st)

def ipname():
    a = rd(255)
    b = rd(255)
    c = rd(255)
    d = rd(255)
    return str(a) + '.' + str(b) + '.' +str(c) + '.' + str(d)

def net_address_miss():
    return 'http://' + rd_elem(son_domain) + '.' + domainname() + '.' + rd_elem(suff) + '/search?ip=' + ipname()

def net_address_target():
    return 'http://' + rd_elem(son_domain) + '.' + domainname() + '.' + rd_elem(suff) + '/search?ip=' + rd_elem(crawl_list)

def main():
    fp = open('fakedata', 'w')

    for i in xrange(MAX):
        pr = rd(101) / 100.0

        na = None
        if pr < Probability[0]:
            na = net_address_target()
        else:
            na = net_address_miss()
        
        fp.write(na + '\n')

    fp.close()

if __name__ == '__main__':
    main()
