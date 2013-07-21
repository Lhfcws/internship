echo Making...
cd ~/coding/internship/crawl_ip_filter/crawl_ip_filter_hash
make clean && make
cd ~/coding/internship/crawl_ip_filter/crawl_ip_filter_trie
make clean && make
echo
echo



cd ~/coding/internship/crawl_ip_filter/crawl_ip_filter_hash/bin
echo Hash_Verion:
time ./Crawl_IP_Filter >  ~/coding/internship/crawl_ip_filter/hash.result
echo
echo


cd ~/coding/internship/crawl_ip_filter/crawl_ip_filter_trie/bin
echo Trie_Verion:
time ./Crawl_IP_Filter >  ~/coding/internship/crawl_ip_filter/trie.result
echo
echo

