CXX = g++
CXXFLAGS = -g -std=c++20 -Wall -pedantic -fsanitize=address,undefined
LD = g++
LDFLAGS =

SRC = CPos.cpp CItem.cpp CDouble.cpp CString.cpp CReference.cpp COperator.cpp CAdd.cpp CSub.cpp CMul.cpp CDiv.cpp CPow.cpp CNeg.cpp CEq.cpp CNe.cpp CLt.cpp CLe.cpp CGt.cpp CGe.cpp CMyBuilder.cpp CSpreadsheet.cpp test.cpp
HDR = definitions.h CPos.h CSpreadsheet.h CItem.h CDouble.h CString.h CReference.h COperator.h CAdd.h CSub.h CMul.h CDiv.h CPow.h CNeg.h CEq.h CNe.h CLt.h CLe.h CGt.h CGe.h CMyBuilder.h
OBJ = $(SRC:.cpp=.o)

all: excel

excel: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -L./x86_64-linux-gnu -lexpression_parser -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f test *.o

.PHONY: all clean