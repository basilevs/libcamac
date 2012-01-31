CAMAC_ABSTRACT_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
CAMAC_ABSTRACT_BASE?=$(dir $(CAMAC_ABSTRACT_MAKEFILE))

CPPFLAGS+=-I$(CAMAC_ABSTRACT_BASE)include
CPPFLAGS+=-I$(CAMAC_ABSTRACT_BASE)FedorovClient
CHEBLAKOV?=$(CAMAC_ABSTRACT_BASE)../../cheblakov
CPPFLAGS+=-I$(CHEBLAKOV)

VPATH+=$(CAMAC_ABSTRACT_BASE)src

DEPGEN=$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

UNITS=base MamkinServer CamacServer rc2test std_lcm
OBJECTS=$(addsuffix .o,$(UNITS))
CLEAN+=$(OBJECTS)

all: libcamac.a

libcamac.a:$(OBJECTS)
	ar -r $@ $^ 
CLEAN+=libcamac.a


%.cpp.d: %.cpp
	@$(DEPGEN)

%.cpp.preprocessed: %.cpp
	$(CC) -E $(CPPFLAGS) $<

DEP = $(addsuffix .cpp.d,$(UNITS))

ifeq "$(filter clean,$(MAKECMDGOALS))" ""
	-include $(DEP)
endif
CLEAN+=$(DEP)
	
.PHONY: clean
clean:
	@rm $(CLEAN) 2>/dev/null||true
