RM := rm -rf

CPP_SRC += \
Decompression.cpp \
WritePPM.cpp

OBJS += \
Decompression.o \
WritePPM.o 

COMP = g++

LIBS := -ljpeg

LIB_SEARCH_PATH = ./../../jpeg-9/src

INCLUDE_PATH = ./../../jpeg-9/src

DYLD_LIBRARY_PATH = ./../../jpeg-9/src

SUBDIRS := src 

DEFINED_SYMBOLS = \
-DPPM_SUPPORTED\
-DQUANT_2PASS_SUPPORTED\

OPTIMIZATION_LEVEL = -O

DEBUG_LEVEL = -g3

WARNINGS = -Wall

OTHER_FLAGS = -c -fmessage-length=0 -std=c++11

DEPENDS = $(patsubst %.o,%.d,$(OBJS))

ifeq ($(shell uname),Darwin)
	LIB = libjpeg.dylib
	LIB_FLAG = -dynamiclib
	COMP = /opt/local/bin/g++
endif
ifeq ($(shell uname),Linux)
	LIB = libjpeg.so
	LIB_FLAG = -shared
endif

# All Target
all: decompression

# Tool invocations
decompression: $(OBJS)
#	@echo 'Building target: $@'
#	@echo 'Invoking: C++ Linker'
	g++ -L"$(LIB_SEARCH_PATH)" -o "decompression" $(OBJS) $(LIBS)
#	@echo 'Finished building target: $@'
#	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(DEPENDS) decompression
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

Decompression.o: Decompression.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) $(DEFINED_SYMBOLS) -I$(INCLUDE_PATH) $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

WritePPM.o: WritePPM.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) $(DEFINED_SYMBOLS) -I$(INCLUDE_PATH) $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '
	































