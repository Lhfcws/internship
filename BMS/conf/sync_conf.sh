# source config files
BLENDER_CFG='/home/lhfcws/ISEARCH/etc/isearch/blender_server.cfg'
SEARCHER_CFG='/home/lhfcws/ISEARCH/etc/isearch/searcher_server.cfg'
DETAILER_CFG='/home/lhfcws/ISEARCH/etc/detail/detail_server.cfg'

# If old-version ISEARCH does not have `detail`, please refer to admin@10.125.224.53:~/search_lau/etc .

cp $BLENDER_CFG ./blender_server.cfg
cp $SEARCHER_CFG ./searcher_server.cfg
cp $DETAILER_CFG ./detail_server.cfg
