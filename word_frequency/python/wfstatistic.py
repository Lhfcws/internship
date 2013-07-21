#!/usr/bin/env python 2.7
#-*- coding:utf-8 -*-

'''
@description Read English text file and calculate word frequenct. 
@author Lhfcws Wu (宸风)
@time 2013-07-12
'''

import copy

# CONFIG
config = {
    'separators': [' ', ',', '.', ':', '\'', '"', '?', '!', '(', ')', '/', '&'],
    'path': '../resource/'
}


class WFStatistic(object):
    def __init__(self):
        
        pass

    # Read a list of document name.
    def read_list(self, doc_list):
        assert type(doc_list) == type([])

        result = {}
        for doc_name in doc_list:
            result[doc_name] = self.read_doc(doc_name)
        
        return result
    
    # Read a document and split it!
    def read_doc(self, doc_name):
        fname = config['path'] + doc_name

        fs = open(fname, 'r')
        seps = config['separators']
        numbers = [str(j) for j in xrange(10)]
        result = {}

        for line in fs.readlines():
            letters = list(line)
            prev = []
            for i, l in enumerate(letters):
                # Judge situations like 'I have 13.5 dollars'
                if len(prev) > 0 and l == '.' and prev[-1] in numbers and len(letters) > i + 1 and prev[i+1] in numbers:
                    continue
                # Judge situations like 'Hello world!really?'
                if l in seps and len(prev) > 0:
                    new_word = ''.join(prev).lower()
                    prev = []
                    result = self.stat(result, new_word)            
                
                if not l in seps:
                    prev.append(l)
            
        return result

        fs.close()

    # Stat english word
    def stat(self, dct, word):
        try:
            num = float(word)
            dct.setdefault('~number', 0)
            dct['~number'] += 1
        except ValueError:
            try:
                dct[word] += 1
            except KeyError:
                dct[word] = 1
        finally:
            return dct
    ##
    ## Output

    def output_by_key_order(self, dct):
        print 'Sort by key:'
        def output_key(dct_):
            keys = dct_.keys()
            keys.sort()
            for key in keys:
                value = dct_[key]
                if type(value) == type({}):
                    print key,': '
                    output_key(value)
                else:
                    print key + ': ' + str(value)
            print ''

        output_key(dct)
        print '========================'

    # Assume that dct.deep == 1 
    def output_by_value_order(self, dct):
        print 'Sort by value:'
        def output_value(dct_):
            ls = []
            for key, value in dct_.iteritems():
                ls.append((value, key))
            #if type(dct_[0]) != type({}):
            ls.sort()

            for value, key in ls:
                if type(value) == type({}):
                    print key, ': '
                    output_value(value)
                else:
                    print key + ': ' + str(value)

            print ''

        output_value(dct)
        print '========================'
            

    def output_by_key_order_sum(self, dct):
        print 'Totally Sort by key:'
        dct_ = {}
        for key in dct.iterkeys():
            dct_ = self.merge_dict(dct_, dct[key])

        self.output_by_key_order(dct_)

    def output_by_value_order_sum(self, dct):
        print 'Totally Sort by value:'
        dct_ = {}
        for key in dct.iterkeys():
            dct_ = self.merge_dict(dct_, dct[key])

        self.output_by_value_order(dct_)

    def merge_dict(self, dct1, dct2):
        d = copy.deepcopy(dct2)

        for key in dct1.iterkeys():
            if not d.has_key(key):
                d[key] = dct1[key]
            else:
                d[key] += dct1[key]
            
        return d

## MAIN
def main():
    filelist = ['test1.txt', 'test2.txt', 'test3.txt']
    Wfs = WFStatistic()
    dct = Wfs.read_list(filelist)

    Wfs.output_by_key_order(dct)
    Wfs.output_by_key_order_sum(dct)

    Wfs.output_by_value_order(dct)
    Wfs.output_by_value_order_sum(dct)

if __name__ == '__main__':
    main()
