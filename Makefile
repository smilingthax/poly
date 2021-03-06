OBJECTS=test_poly.o
DEPENDS=$(OBJECTS:.o=.d)

CXXFLAGS+=-std=c++11 -Wall
#CXXFLAGS+=-std=c++17 -Wall

all: test_poly
ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDS)
endif

clean:
	$(RM) $(DEPENDS) $(OBJECTS) test_poly

%.d: %.cpp
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MT"$@" -MT"$*.o" -o $@ $<  2> /dev/null

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

test_poly: test_poly.o
	$(CXX) $(CXXFLAGS) -o $@ $^

