# Our output name
BINARY = nrf24-send

# Using the STM32F1 series chip
LIBNAME		= opencm3_stm32f1
DEFS		+= -DSTM32F1

LIBEMPATH	+=../../libem

DEFS	+= -I$(LIBEMPATH)/libnrf24l01/src/include
LDFLAGS	+= -L$(LIBEMPATH)/libnrf24l01/lib/ -L$(LIBEMPATH)/libnrf24l01/src/
LDLIBS	+= -lnrf24l01
TGT_CFLAGS	+= -DNRF_REG_DEF_META

ifeq ($(WITH_CONIO),1)
DEFS	+= -I$(LIBEMPATH)/libconio/src/include -I$(LIBEMPATH)/libserial/src/include
LDFLAGS	+= -L$(LIBEMPATH)/libconio/lib/ -L$(LIBEMPATH)/libconio/src/ -L$(LIBEMPATH)/libserial/lib/ -L$(LIBEMPATH)/libserial/src/
LDLIBS	+= -lconio -lserial
TGT_CFLAGS	+= -DWITH_CONIO

endif

include ../stm32f103.mk
