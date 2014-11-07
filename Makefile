 ##########################################################################
 # Copyright University of Warwick 2014.                                  #
 # Distributed under the Boost Software License, Version 1.0.             #
 # (See accompanying file LICENSE_1_0.txt or copy at                      #
 # http://www.boost.org/LICENSE_1_0.txt)                                  #
 #                                                                        #
 # Author:                                                                #
 # John Back                                                              #
 #                                                                        #
 # -------------------------------                                        #
 # Standalone Makefile for ACTIVIA                                        #
 # -------------------------------                                        #
 #                                                                        #
 # Instructions                                                           #
 #     - Run the configure file to set various compiler options           #
 #     - run 'make <target>'                                              #
 #                                                                        #
 # Build targets                                                          #
 #   lib     - make libActivia.a                                          #
 #   shlib   - make libActivia.so (default)                               #
 #   bin     - make bin/Activia program using libActivia.so (default)     #
 #   install - install the include and lib directories in $PREFIX         #
 #   clean   - delete all intermediate and final build objects            #
 #                                                                        #
 ##########################################################################

include ./config.mk

INCLUDES = -I$(INCDIR)
LIBS = -L${LIBDIR} -l${PACKAGE}
SHLIBS =

# Include ROOT if required
ifeq ($(ACT_USE_ROOT),1)
	INCLUDES += -I$(ROOTINCDIR)
	LIBS += $(ROOTLIBS)
	SHLIBS += $(ROOTLIBS)
endif

# If Qt is required
ifeq ($(ACT_USE_QT), 1)
	LIBS += $(QTLIBS)
	SHLIBS += $(QTLIBS)
endif

CXXFLAGS += $(INCLUDES)
LIBFILE   = $(LIBDIR)/lib$(PACKAGE).a
SHLIBFILE = $(LIBDIR)/lib$(PACKAGE).so

default: shlib bin

# Need to remove qt files if required...

# List of all source files that contain main functions
BINCCLIST:=$(BINCCLIST)

# List of all object files to build
OLIST:=$(patsubst %.cc,%.o,$(addprefix $(OBJDIR)/,$(notdir $(CCLIST))))

# List of all dependency files to make
DLIST:=$(patsubst %.cc,%.d,$(addprefix $(DEPDIR)/,$(notdir $(CCLIST))))

# List of all binary files to make
BINLIST:=$(patsubst %.cc,%,$(addprefix $(BINDIR)/,$(notdir $(BINCCLIST))))

# List of all object files to build for the binaries
BINOLIST:=$(patsubst %.cc,%.o,$(addprefix $(OBJDIR)/,$(notdir $(BINCCLIST))))

# Rule for creating the moc source files for Qt
$(SRCDIR)/moc_%.cc: ${FULLINCDIR}/%.hh
	@echo "Making $@"
	$(ACT_MOC) $(QTDEFINE) $(EXTRAFLAGS) $(QTINCLUDE) $< -o $@

# Implicit rule making all dependency files
$(DEPDIR)/%.d: $(SRCDIR)/%.cc
	@echo "Making $@"
	@mkdir -p $(DEPDIR)
	@set -e; $(CXX) $(MFLAGS) $(EXTRAFLAGS) $(CXXFLAGS) $< \
	          | sed 's#\($(notdir $*)\)\.o[ :]*#$(OBJDIR)/\1.o $@ : #g' > $@; \
	        [ -s $@ ] || rm -f $@

# Implicit rule to compile all classes
$(OBJDIR)/%.o : $(SRCDIR)/%.cc
	@echo "Compiling $<"
	@mkdir -p $(OBJDIR)
	@$(CXX) $(EXTRAFLAGS) $(CXXFLAGS) -c $< -o $@


# Rule to combine objects into a library
$(LIBFILE): $(OLIST)
	@echo "Making $(LIBFILE)"
	@mkdir -p $(LIBDIR)
	@rm -f $(LIBFILE)
	@ar rcs $(LIBFILE) $(OLIST)

# Rule to combine objects into a shared library
$(SHLIBFILE): $(OLIST)
	@echo "Making $(SHLIBFILE)"
	@mkdir -p $(LIBDIR)
	@rm -f $(SHLIBFILE)
	@$(CXX) $(OLIST) $(SOFLAGS) $(SHLIBS) -o $(SHLIBFILE)

# Rule to make binaries from their corresponding object files
$(BINLIST): $(BINOLIST)
	@echo "Creating $@"
	@mkdir -p $(BINDIR)
	@$(CXX) $(CXXFLAGS) $(EXTRAFLAGS) -o $@ $< $(LIBS)

# Rule to make objects from binary main files
$(BINOLIST): $(BINCCLIST)
	@echo "Making $@"
	@mkdir -p $(LIBDIR)
	@$(CXX) $(CXXFLAGS) $(EXTRAFLAGS) -c -o $@ $< -I$(INCLUDES)

bin: $(BINLIST)

# Useful build targets
lib: $(LIBFILE) 
shlib: $(SHLIBFILE)

install:
	if test "${PREFIX}" != "." ; then \
	  mkdir -p ${PREFIX}/lib ${PREFIX}/include && \
	  cp -rf lib/* ${PREFIX}/lib/ && \
	  cp -rf include/${PACKAGE} ${PREFIX}/include ; \
        fi

clean:
	rm -rf $(TMPDIR)
	rm -f $(LIBFILE)
	rm -f $(SHLIBFILE)
	rm -f $(BINLIST)

.PHONY: bin shlib lib default clean install

-include $(DLIST)
