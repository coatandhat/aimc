# How to use:
# Add your cpp code to the list in UNITS.
#
# Unit test:
# If you want to add a test build target for that cpp file ONLY, please see 
# example "AGC_unittest". (no need to add explicit build rule)
#
# If you intend to test a class including instanciated members, you need to 
# make a explicit rule to link properly. See example "CARFAC_unittest".

# settings
GTEST_DIR = ../../../googletest
LIB_EXTENSION = .dll
EXE_EXTENSION = .exe
# end settings

SRC_HEADERDIR = include
SRC_DIR = src
SRC_TESTDIR = unittest

SRC_HEADERS = $(SRC_HEADERDIR)/*.h

CPPFLAGS += -I$(SRC_HEADERDIR) -I$(GTEST_DIR)/include
CXXFLAGS += -g -Wall -Wextra -std=gnu++0x #IMPORTANT note gnu++0x

UNITS = CARFAC AGC CAR Ear IHC unit_conversion #Add build units here (without the .cpp)
SRC_OBJ = $(addprefix $(SRC_DIR)/, $(addsuffix .o, $(UNITS)))

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
                
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

all : libcarfac unittest

libcarfac : $(SRC_DIR)/CARFAC.o $(SRC_OBJ)
	$(CXX) -shared $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@$(LIB_EXTENSION) -fPIC

example_program : libcarfac
	# likely some example executable that runs the whole thing...

unittest: $(foreach unit, $(UNITS), $(addsuffix _unittest, $(unit)))
	$(foreach binary, $^, ./$(addsuffix $(EXE_EXTENSION), $(binary));)
	
CARFAC_unittest : $(SRC_OBJ)
CAR_unittest : $(SRC_OBJ) #$(SRC_DIR)/unit_conversion.o
Ear_unittest : $(SRC_OBJ)

clean :
	rm -f *.o *.a src/*.o unittest/*.o *_unittest* *.dll *.exe

# pattern magic
$(SRC_DIR)/%.o : %.cpp $(SRC_HEADERS) #normal source
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(.SOURCE)

$(SRC_TESTDIR)/%.o : %.cpp $(SRC_HEADERS) $(GTEST_HEADERS) # unittest
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(.SOURCE)

%_unittest : $(SRC_DIR)/%.o $(SRC_TESTDIR)/%_unittest.o gtest_main.a #unittest
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@
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