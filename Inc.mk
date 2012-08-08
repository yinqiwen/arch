CXX=g++
CC=gcc

CXXFLAGS=-Wall -Wno-format -g -O2 -D_FILE_OFFSET_BITS=64 -fPIC
CCFLAGS=-Wall -std=gnu99 -pedantic -Wno-format -g -O2 -D_FILE_OFFSET_BITS=64 -fPIC
LDFLAGS=-g 

ifndef LIB_PATH
   LIB_PATH=$(PWD)
endif

3RD_PATH=$(LIB_PATH)/3rd

ifndef 3RD_LIB_DIST_PATH
   3RD_LIB_DIST_PATH=$(3RD_PATH)
endif

CMAKE_PATH=$(3RD_LIB_DIST_PATH)/cmake-2.8.8
CMAKE_FILE=$(3RD_PATH)/cmake-2.8.8.tar.gz 

MYSQL_PATH=$(3RD_LIB_DIST_PATH)/mysql-connector-c-6.0.2
MYSQL_FILE=$(3RD_PATH)/mysql-connector-c-6.0.2.tar.gz
MYSQL_LIB=$(MYSQL_PATH)/libmysql/libmysqlclient.a 

SQLITE3_PATH=$(3RD_LIB_DIST_PATH)/sqlite-autoconf-3071300
SQLITE3_FILE=$(3RD_PATH)/sqlite-autoconf-3071300.tar.gz
SQLITE3_LIB=$(SQLITE3_PATH)/.libs/libsqlite3.a 

SPARSEHASH_PATH=$(3RD_LIB_DIST_PATH)/sparsehash-2.0.2
SPARSEHASH_FILE=$(3RD_PATH)/sparsehash-2.0.2.tar.gz

GTEST_PATH=$(3RD_LIB_DIST_PATH)/gtest-1.6.0
GTEST_FILE=$(3RD_PATH)/gtest-1.6.0.zip
GTEST_LIB=$(GTEST_PATH)/lib/.libs/libgtest.a $(GTEST_PATH)/lib/.libs/libgtest_main.a

INCS=-I$(SQLITE3_PATH) -I$(MYSQL_PATH)/include \
     -I$(SPARSEHASH_PATH)/include  -I$(GTEST_PATH)/include -I$(LIB_PATH)/src 

INSTALL_DIR=$(LIB_PATH)

LIBS= -L$(INSTALL_DIR) $(SQLITE3_LIB) $(MYSQL_LIB) -lpthread -lrt -ldl
#LIBS= -L$(INSTALL_DIR) -lpthread -lrt -ltcmalloc -lprofiler

%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o $@

%.o : %.c
	${CC} -c ${CCFLAGS} ${INCS} $< -o $@
