# How to use:
# Add your cpp code to the list in UNITS.
#
# Unit test:
# If you want to add a test build target for that cpp file ONLY, please see 
# example "AGC_unittest". (no need to add explicit build rule)
#
# If you intend to test a class including instanciated members, you need to 
# make a explicit rule to link properly. See example "CARFAC_unittest".

SRC_HEADERDIR = include
SRC_DIR = src
SRC_TESTDIR = unittest

GTEST_DIR = ../../../googletest

SRC_HEADERS = $(SRC_HEADERDIR)/*.h

CPPFLAGS += -I$(SRC_HEADERDIR) -I$(GTEST_DIR)/include
CXXFLAGS += -g -Wall -Wextra -std=gnu++0x #IMPORTANT note gnu++0x

UNITS = AGC CAR CARFAC Ear IHC #Add build units here (without the .cpp)
SRC_OBJ = $(addprefix $(SRC_DIR)/, $(addsuffix .o, $(UNITS)))

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
                
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

all : CARFAC_unittest AGC_unittest #make some other "main" target

clean :
	rm -f *.o *.a src/*.o unittest/*.o *_unittest.exe

CARFAC_unittest : $(SRC_TESTDIR)/CARFAC_unittest.o $(SRC_OBJ)

# pattern magic
$(SRC_DIR)/%.o : %.cpp $(SRC_HEADERS) #normal source
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(.SOURCE)

$(SRC_TESTDIR)/%.o : %.cpp $(SRC_HEADERS) $(GTEST_HEADERS) # unittest
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(.SOURCE)

%_unittest : $(SRC_DIR)/%.o $(SRC_TESTDIR)/%_unittest.o gtest_main.a #unittest
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@
#	./$@.exe # launches the test - disabled, as failing tests stops the build
# end pattern magic

# gtest stuff
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc
            
gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^
# end gtest stuff