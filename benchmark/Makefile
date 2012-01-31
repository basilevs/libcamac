BENCHMARK_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
BENCHMARK_BASE?=$(dir $(BENCHMARK_MAKEFILE))
CAMAC_ABSTRACT_BASE?=$(BENCHMARK_BASE)../

CPPFLAGS+=-I$(CAMAC_ABSTRACT_BASE)include

DEPGEN=$(CC) -M $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

.PHONY : libcamac.a clean

read: read.o libcamac.a
	g++ $^ $(LDFLAGS) -o $@
readtest: read
	./read 0 2 0
CLEAN+=read read.o

lam: lam.o libcamac.a
	g++ $^ $(LDFLAGS) -o $@
CLEAN+=lam lam.o

lamtest: lam
	./lam 0 2 13
    
%.cpp.d: %.cpp
	@$(DEPGEN)

libcamac.a:
	$(MAKE) -f $(CAMAC_ABSTRACT_BASE)cheblakov.make

clean:
	$(MAKE) -f $(CAMAC_ABSTRACT_BASE)cheblakov.make clean
	rm -f $(CLEAN)
	

