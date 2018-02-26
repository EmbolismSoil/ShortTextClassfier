
APP := ShortTextClassfier
SRC := $(wildcard ./*.cpp)
OBJ := $(SRC:%.cpp=%.o)
DEP := $(SRC:%.cpp=%.d)

CXX := g++
CXXFLAGS := -I./ -g
LIBS += -ljsoncpp
LIBS += -lleveldb

all: $(APP)

$(APP): $(DEP) $(OBJ)
	echo $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIBS)

sinclude rules.mk
sinclude $(DEP)

.PHONY : clean

clean:
	-rm *.o
	-rm *.d
	-rm $(APP)
