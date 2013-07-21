# Word Frequency Statistic

> author: Lhfcws Wu (宸风)


### Description

Get a piece of English document, calculate the frequency of different words.

### Pre-conditions

+ 不用考虑停用词。
+ 统计完需要按词频排序。
+ 可能有多个文档, 文档假设不存在同名的文件文档。
+ English 词间分隔符包括： ' ' , ',' , '.' , '?' , '!' , '(' , ')' , ':' , ''' , '"'
+ 可能存在两个分隔符同时存在的情况。如 'I say: hello world! '
+ 会存在数字，会存在小数点 如4.3

### Post-conditions

+ 输出按{word: frequency}键值对格式，每行一个键值对。
+ 提供按文档查看和按全部查看词频。
