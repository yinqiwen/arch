
include Inc.mk

SRC_DIR = src
TEST_DIRS := $(wildcard test/*)

DIRS = $(SRC_DIR) $(TEST_DIRS)

ifneq ("/usr/local/include/google", "$(wildcard /usr/local/include/google)")
  DEPENDENCY = sparsehash
endif

ifneq ("$(GTEST_PATH)/lib/.libs/libgtest.a", "$(wildcard $(GTEST_PATH)/lib/.libs/libgtest.a)")
  DEPENDENCY += gtest
endif

ifneq ("$(SQLITE3_PATH)/.libs/libsqlite3.a", "$(wildcard $(SQLITE3_PATH)/.libs/libsqlite3.a)")
  DEPENDENCY += sqlite3
endif

ifneq ("/usr/local/bin/cmake", "$(wildcard /usr/local/bin/cmake)")
  DEPENDENCY += cmake
endif

ifneq ("$(MYSQL_PATH)/libmysql/libmysqlclient.a", "$(wildcard $(MYSQL_PATH)/libmysql/libmysqlclient.a)")
  DEPENDENCY += mysql
endif

all: $(DEPENDENCY)
	@for dir in $(DIRS); do make -C $$dir; echo; done
	

sparsehash:
	cd $(3RD_LIB_DIST_PATH);tar zxvf $(SPARSEHASH_FILE); cd $(SPARSEHASH_PATH);chmod +x configure; \
	./configure --prefix=$(SPARSEHASH_PATH); make; sudo make install

gtest:
	cd $(3RD_LIB_DIST_PATH); unzip $(GTEST_FILE); cd $(GTEST_PATH);chmod +x configure; \
	./configure; make

sqlite3:
	cd $(3RD_LIB_DIST_PATH); tar zxvf $(SQLITE3_FILE); cd $(SQLITE3_PATH);chmod +x configure; \
	./configure; make

cmake:
	cd $(3RD_LIB_DIST_PATH); tar zxvf $(CMAKE_FILE); cd $(CMAKE_PATH); \
	./bootstrap; make; sudo make install

mysql:
	cd $(3RD_LIB_DIST_PATH); tar zxvf $(MYSQL_FILE); cd $(MYSQL_PATH); \
	cmake -G "Unix Makefiles"; make
	
baseclean:
	@for dir in $(SRC_DIR); do make -C $$dir clean; echo; done

testclean:
	@for dir in $(TEST_DIRS); do make -C $$dir clean; echo; done

clean:
	@for dir in $(DIRS); do make -C $$dir clean; echo; done

