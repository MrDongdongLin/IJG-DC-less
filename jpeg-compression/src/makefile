RM := rm -rf

CPP_SRCS += \
Compression.cpp \
Decompression.cpp \
WritePPM.cpp \
Read_P2_PPM.cpp \
Read_P3_PPM.cpp 

OBJS += \
Compression.o \
Decompression.o \
WritePPM.o \
ReadPPM.o \
Read_P2_PPM.o \
Read_P3_PPM.o 

COMP = g++

LIBS := -ljpeg

LIB_SEARCH_PATH = ./../../jpeg-9/src

INCLUDE_PATH = ./../../jpeg-9/src

DYLD_LIBRARY_PATH = ./../../jpeg-9/src/

SUBDIRS := src 

DEFINED_SYMBOLS = PPM_SUPPORTED

OPTIMIZATION_LEVEL = -O3

DEBUG_LEVEL = -g3

WARNINGS = -Wall

OTHER_FLAGS = -c -fmessage-length=0 -std=c++11

DEPENDS = $(patsubst %.o,%.d,$(OBJS))

ifeq ($(shell uname),Darwin)
	LIB = libjpeg.dylib
	LIB_FLAG = -dynamiclib
	COMP =/opt/local/bin/g++
endif
ifeq ($(shell uname),Linux)
	LIB = libjpeg.so
	LIB_FLAG = -shared
endif

# All Target
all: compression

# Tool invocations
compression: $(OBJS)
#	@echo 'Building target: $@'
#	@echo 'Invoking: C++ Linker'
	g++ -L"$(LIB_SEARCH_PATH)" -o "compression" $(OBJS) $(LIBS)
#	@echo 'Finished building target: $@'
#	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(DEPENDS) compression
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

Compression.o: Compression.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) -D$(DEFINED_SYMBOLS) -I"$(INCLUDE_PATH)" $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

Decompression.o: Decompression.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) -D$(DEFINED_SYMBOLS) -I"$(INCLUDE_PATH)" $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

WritePPM.o: WritePPM.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) -D$(DEFINED_SYMBOLS) -I"$(INCLUDE_PATH)" $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

ReadPPM.o: ReadPPM.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) -D$(DEFINED_SYMBOLS) -I"$(INCLUDE_PATH)" $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

Read_P2_PPM.o: Read_P2_PPM.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) -D$(DEFINED_SYMBOLS) -I"$(INCLUDE_PATH)" $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

Read_P3_PPM.o: Read_P3_PPM.cpp
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C++ Compiler'
	$(COMP) -D$(DEFINED_SYMBOLS) -I"$(INCLUDE_PATH)" $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MP -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '

-include $(DEPENDS)
