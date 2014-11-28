LIB = libSTM32F4xx_drivers.a

CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib

CFLAGS = -Wall -O2 -mthumb
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16
CFLAGS += -mcpu=cortex-m4 -fsingle-precision-constant -funsigned-char -Wundef -Wsign-compare -Wunreachable-code -Wstrict-prototypes
CFLAGS += -I../inc -I../../core -DARM_MATH_CM4 -DUSE_STDPERIPH_DRIVER -DSTM32F40_41xxx -D__FPU_USED=1 -DHSE_VALUE=8000000 -DSTM32F407VG

SRCS = \
../src/misc.c \
../src/stm32f4xx_dac.c \
../src/stm32f4xx_dma.c \
../src/stm32f4xx_exti.c \
../src/stm32f4xx_gpio.c \
../src/stm32f4xx_i2c.c \
../src/stm32f4xx_rcc.c \
../src/stm32f4xx_rng.c \
../src/stm32f4xx_spi.c \
../src/stm32f4xx_syscfg.c \
../src/stm32f4xx_tim.c 

OBJS = $(SRCS:.c=.o)

all: $(LIB)

$(LIB): $(OBJS)
	@$(AR) -r $(LIB) $(OBJS)
	@$(RANLIB) $(LIB)

%.o : %.c
	@echo "  GCC $^"
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	-rm -f $(OBJS)
	-rm -f $(LIB)

.PHONY: all clean
