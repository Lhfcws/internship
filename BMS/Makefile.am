#AM_LDFLAGS = -static-libtool-libs
rootdir=../../../../
INCLUDES=-I$(rootdir)/include -I$(rootdir)/src -I$(rootdir)/src/sche/application/blender/ -I$(rootdir)/src/sche/ \
		 -I$(rootdir)/src/secore/utility -I$(rootdir)/src/secore/basis -I$(rootdir)/src/document/ \
		 -I$(rootdir)/src/sche/application/detail -I$(rootdir)/src/sche/application/searcher -I$(rootdir)/src/sort \
		 -I$(rootdir)/src/sche/application

DEFS+=$(MACRO)

bin_PROGRAMS=BMS

include_HEADERS=BMS.h BMSServer.h BMSQueryRewrite.h BMSQueryQueue.h BMSWorker.h BMSWorkerFactory.h BMSCommCenter.h validation.h CLParser.h
BMS_SOURCES=BMS.cpp BMSServer.cpp BMSCommCenter.cpp BMSQueryRewrite.cpp BMSWorker.cpp BMSWorkerFactory.cpp BMSQueryQueue.cpp CLParser.cpp

#BMS_LDADD=-L$(top_srcdir)/src/util -lanet -lutil $(top_srcdir)/src/sche/libsche.la \
#		  $(top_srcdir)/src/clustermap/CMNode.o \
#		  $(top_srcdir)/src/sche/application/merger/ISearchResultProcessor.o \
#		  $(top_srcdir)/src/sche/application/merger/MemPoolFactory.o \
#		  $(top_srcdir)/src/sche/application/merger/MergerSort.o \
#		  $(top_srcdir)/src/sche/application/blender/DocumentRewrite.o \
#		  $(top_srcdir)/src/sche/application/blender/QueryRewrite.o \
#		  $(top_srcdir)/src/sche/application/searcher/ResultFormatFactory.o \
#		  $(top_srcdir)/src/secore/search/Ors.o \
#		  $(top_srcdir)/src/secore/search/QueryParameter.o \
#		  $(top_srcdir)/src/secore/search/SearchResult.o \
#		  $(top_srcdir)/src/secore/search/SearchResultSerializer.o

BMS_LDADD=-lanet \
		$(top_srcdir)/src/clustermap/libclustermap.la \
		$(top_srcdir)/src/sche/application/blender/libblender.la \
		$(top_srcdir)/src/sche/application/searcher/libsearcher.la \
		$(top_srcdir)/src/sche/application/detail/libdetail.la \
		$(top_srcdir)/src/sche/libsche.la \
		-L$(top_srcdir)/src/secore -L$(top_srcdir)/src/util $(isearchlib) -lutil $(special_lib) \
		$(top_srcdir)/src/clustermap/libclustermap.la \
		$(top_srcdir)/src/sort/libsort.la 
		






