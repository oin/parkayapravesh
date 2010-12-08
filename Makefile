# Makefile
# Projet RVIP 2010
# J. Aceituno, L. Letoupin, Y. Tisserand

# lib: rvip (local, pas vraiment une lib)
rvip_dir = ./librvip
rvip_cppflags = -I$(rvip_dir)

# lib: tuio
tuio_dir = libTUIO
tuio_cppflags = -I$(tuio_dir)/TUIO -I$(tuio_dir)/oscpack
tuio_ldflags = $(tuio_dir)/libTUIO.a

# lib: opensg
opensg_libtype ?= dbg
opensg_subsystems ?= Base System GLUT
opensg_cppflags = $(shell osg-config --cflags --$(opensg_libtype) $(opensg_subsystems))
opensg_ldflags = $(shell osg-config --libs --$(opensg_libtype) Base System GLUT)

# libs:
librairies = opensg rvip tuio

# Concaténation de toutes les choses nécessaires aux librairies
librairies_cppflags = $(foreach lib,$(librairies),$($(addsuffix _cppflags,$(lib))))
librairies_ldflags = $(foreach lib,$(librairies),$($(addsuffix _ldflags,$(lib))))

# Options à passer au compilateur
CPPFLAGS ?= -g -Wall
CPPFLAGS += $(addprefix -I,$(chercher_dans))
CPPFLAGS += $(librairies_cppflags)

# Options à passer à l'éditeur de liens
LDFLAGS ?= -lm
LDFLAGS += $(addprefix -L,$(chercher_dans))
LDFLAGS += $(librairies_ldflags)

# Tous les programmes sont pourvus de l'extension .src
programmes = $(patsubst %.src,%,$(wildcard *.src))

# Tous les programmes dépendent de librvip
vpath %.h $(rvip_dir)
rvip_objects = $(patsubst %.cpp,%.o,$(wildcard $(rvip_dir)/*.cpp))

##################

all: $(programmes)

%: %.src/*.cpp librvip.a
	$(LINK.cpp) $(OUTPUT_OPTION) $^

librvip.a: $(rvip_objects)
	$(AR) $(ARFLAGS)s $@ $^

%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $^

.PHONY: clean
clean:
	rm -rf *.o *.d $(programmes) $(addsuffix .dSYM,$(programmes)) $(addsuffix /*.o,$(programmes)) librvip.a