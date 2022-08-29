override CXXFLAGS += -std=c++14 -I/usr/include/z3/ -fopenmp -pthread -O2
override LDFLAGS  += -std=c++14 -lz3 -fopenmp -pthread -O2

EXEC = eqv

CSRCS      = $(wildcard *.cpp */*.cpp)
CHDRS      = $(wildcard *.hpp */*.hpp)

NAMEOBJDIR = objs
OBJDIR = $(shell mkdir -p $(NAMEOBJDIR); echo $(NAMEOBJDIR))
NOM = $(basename $(notdir $(CSRCS)))
COBJS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(NOM)))

$(OBJDIR)/%.o: */%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(COBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(EXEC) "$(EXEC).sif"
	rm -rf objs

singularity:
	rm -f "$(EXEC).sif"
	singularity build --fakeroot "$(EXEC).sif" "$(EXEC).def"

