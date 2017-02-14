ACLOCAL_AMFLAGS = -I m4

SRC_ALL=\
	src/Server.cpp \
	src/BaseCore.cpp \
	src/Config.cpp \
	src/Log.cpp \
	src/CgiService.cpp \
	src/Params.cpp \
	src/Cookie.cpp \
	src/Core.cpp \
	src/base64.cpp \
	src/UrlParser.cpp \
	src/main.cpp


CMN_CPPFLAGS=-std=c++11 -Iinclude
CMN_LDADD = $(BOOST_DATE_TIME_LIBS) $(BOOST_FILESYSTEM_LIBS) $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_REGEX_LIBS)

if DEBUG
bin_PROGRAMS = rgadtrackerd-dev
rgadtrackerd_dev_SOURCES=$(SRC_ALL)
rgadtrackerd_dev_CPPFLAGS=$(CMN_CPPFLAGS)
rgadtrackerd_dev_LDADD=$(CMN_LDADD)
else
bin_PROGRAMS = rgadtrackerd
rgadtrackerd_SOURCES=$(SRC_ALL)
rgadtrackerd_CPPFLAGS=$(CMN_CPPFLAGS)
rgadtrackerd_LDADD=$(CMN_LDADD)
endif

if DEBUG
AM_CXXFLAGS = -ggdb3 -DDEBUG -Wall -pipe
else
AM_CXXFLAGS = -m64 -Os -march=native -pipe
endif
