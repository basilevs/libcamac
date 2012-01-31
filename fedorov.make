CPPFLAGS+=-Iinclude

VPATH+=src

DFLIBS= camacdf dfc dfm dfcompat dfmxx 

DEPGEN=$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

UNITS=base FedorovServer CamacServer rc2test
OBJECTS=$(addsuffix .o,$(UNITS))
CLEAN+=$(OBJECTS)

libcamac.a:$(OBJECTS)
	ar -r $@ $^ 
CLEAN+=libcamac.a



base.o rc2test.o: CPPFLAGS:=$(CPPFLAGS) -IFedorovClient

%.cpp.d: %.cpp
	@$(DEPGEN)

%.cpp.preprocessed: %.cpp
	$(CC) -E $(CPPFLAGS) $<

DEP = $(addsuffix .cpp.d,$(UNITS))

ifeq "$(filter clean,$(MAKECMDGOALS))" ""
	-include $(DEP)
endif
CLEAN+=$(DEP)
	
clean:
	@rm $(CLEAN) 2>/dev/null||true
