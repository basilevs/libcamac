CAMAC_ABSTRACT_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
CAMAC_ABSTRACT_BASE?=$(dir $(CAMAC_ABSTRACT_MAKEFILE))

INCLUDE = $(CAMAC_ABSTRACT_BASE)include $(CAMAC_ABSTRACT_BASE)FedorovClient $(CAMAC_ABSTRACT_BASE)../../cheblakov


CPPFLAGS+=$(addprefix -I,$(INCLUDE))

#The Cheblakov server requires lsi6camac.h that can be found in https://github.com/basilevs/lsi6
#The CHEBLAKOV variable point to the directory that contains this file
CHEBLAKOV?=$(CAMAC_ABSTRACT_BASE)../../cheblakov


VPATH+=$(CAMAC_ABSTRACT_BASE)src $(CAMAC_ABSTRACT_BASE)FedorovClient/camac/dfmodule/tools $(CAMAC_ABSTRACT_BASE)FedorovClient/camac/dfmodule $(CAMAC_ABSTRACT_BASE)FedorovClient/camac/df

DEPGEN=$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

UNITS=base MamkinServer lecroy2249 CamacServer rc2test std_lcm CamacAddressParser IntegralVoltmeter_C0309
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

cppcheck:
	cppcheck -f -q -v $(addprefix -I,$(INCLUDE)) $(CAMAC_ABSTRACT_BASE)

