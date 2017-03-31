
RM = rm -rf


C_SRCS += \
jaricom.c \
jcapimin.c \
jcapistd.c \
jcarith.c \
jccoefct.c \
jccolor.c \
jcdctmgr.c \
jchuff.c \
jcinit.c \
jcmainct.c \
jcmarker.c \
jcmaster.c \
jcomapi.c \
jcparam.c \
jcprepct.c \
jcsample.c \
jctrans.c \
jdapimin.c \
jdapistd.c \
jdarith.c \
jdatadst.c \
jdataqual.c\
jdatasrc.c \
jdcoefct.c \
jdcolor.c \
jddctmgr.c \
jdhuff.c \
jdinput.c \
jdmainct.c \
jdmarker.c \
jdmaster.c \
jdmerge.c \
jdpostct.c \
jdsample.c \
jdtrans.c \
jerror.c \
jfdctflt.c \
jfdctfst.c \
jfdctint.c \
jidctflt.c \
jidctfst.c \
jidctint.c \
jmemmgr.c \
jmemnobs.c \
jquant1.c \
jquant2.c \
jutils.c 


OBJS += \
jcapimin.o \
jaricom.o \
jcapistd.o \
jcarith.o \
jccoefct.o \
jccolor.o \
jcdctmgr.o \
jchuff.o \
jcinit.o \
jcmainct.o \
jcmarker.o \
jcmaster.o \
jcomapi.o \
jcparam.o \
jcprepct.o \
jcsample.o \
jctrans.o \
jdapimin.o \
jdapistd.o \
jdarith.o \
jdatadst.o \
jdataqual.o\
jdatasrc.o \
jdcoefct.o \
jdcolor.o \
jddctmgr.o \
jdhuff.o \
jdinput.o \
jdmainct.o \
jdmarker.o \
jdmaster.o \
jdmerge.o \
jdpostct.o \
jdsample.o \
jdtrans.o \
jerror.o \
jfdctflt.o \
jfdctfst.o \
jfdctint.o \
jidctflt.o \
jidctfst.o \
jidctint.o \
jmemmgr.o \
jmemnobs.o \
jquant1.o \
jquant2.o \
jutils.o 

DEPENDS = $(patsubst %.o,%.d,$(OBJS))

SUBDIRS = \
src \

COMP = g++

OPTIMIZATION_LEVEL = -O

DEBUG_LEVEL = -g3

WARNINGS = -Wall

OTHER_FLAGS = -c -fmessage-length=0 -std=c++11 -fPIC



ifeq ($(shell uname),Darwin)
	LIB = libjpeg.dylib
	LIB_FLAG = -dynamiclib
	COMP =/opt/local/bin/g++
endif
ifeq ($(shell uname),Linux)
	LIB = libjpeg.so
	LIB_FLAG = -shared
endif

all: $(LIB)

$(LIB): $(OBJS) $(USER_OBJS)
	@echo 'Building libjpeg'
#	@echo 'Building target: $@'
#	@echo 'Invoking:  C Linker'
	$(COMP) $(LIB_FLAG) -o "$(LIB)" $(OBJS) $(USER_OBJS) $(LIBS)
#	@echo 'Finished building target: $@'
#	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(LIBRARIES)$(DEPENDS) $(LIB)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

# Each subdirectory must supply rules for building sources it contributes
.c.o:
#	@echo 'Building file: $<'
#	@echo 'Invoking: GCC C Compiler'
	$(COMP) $(OPTIMIZATION_LEVEL) $(DEBUG_LEVEL) $(WARNINGS) $(OTHER_FLAGS) -MD -MF"$(@:%.o=%.d)" -o"$@" "$<"
#	@echo 'Finished building: $<'
#	@echo ' '
	

-include $(DEPENDS)
