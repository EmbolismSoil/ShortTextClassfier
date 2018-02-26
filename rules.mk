%.d : %.cpp
	$(Q) set -e;rm -f $@;\
	$(CC) -MM $(CXXFLAGS) $< > $@.$$$$;\
		sed 's/\($(notdir $*)\)\.o[ :]*/\1.o $(notdir $@) : /g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
