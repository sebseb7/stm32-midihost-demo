PROJECT=template


STM32F=4
LSCRIPT=core/stm32f$(STM32F)xx_flash.ld

OPTIMIZATION = -O2

ifeq ($(STM32F),2)
CORTEXM=3
else
CORTEXM=4
endif


SRC=$(wildcard  core/*.c *.c usb/*.c midi/*.c mcugui/*.c libs/*.c disp/*.c STM32F4_drivers/src/*.c)

OBJECTS=$(patsubst %,.bin/%,$(SRC:.c=.o)) 
LSTFILES=$(patsubst %,.bin/%,$(SRC:.c=.lst)) 
DEPS   =$(patsubst %,.bin/%,$(SRC:.c=.d)) 



#  Compiler Options
GCFLAGS = -DSTM32F=$(STM32F) -ffreestanding -std=gnu99 -mcpu=cortex-m$(CORTEXM) -mthumb $(OPTIMIZATION) -Idisp -I. -Imidi -Icore -Iusb -DARM_MATH_CM$(CORTEXM) -DSTM32F40_41xxx -D__FPU_USED=1 -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -DSTM32F407VG -flto
ifeq ($(CORTEXM),4)
GCFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16 
endif
# Warnings
GCFLAGS += -Wstrict-prototypes -Wundef -Wall -Wextra -Wunreachable-code -Wno-strict-aliasing
# Optimizazions
GCFLAGS += -fstrict-aliasing -fsingle-precision-constant -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-builtin -ffunction-sections -fno-common -fdata-sections 
# Debug stuff
GCFLAGS += -Wa,-adhlns=.bin/$(<:.c=.lst),-gstabs -g 

GCFLAGS+= -ISTM32F$(STM32F)_drivers/inc


LDFLAGS = -mcpu=cortex-m$(CORTEXM) -mthumb $(OPTIMIZATION) -T$(LSCRIPT) 
ifeq ($(CORTEXM),4)
LDFLAGS+= -mfpu=fpv4-sp-d16 -mfloat-abi=hard -falign-functions=16
endif
#LDFLAGS+= -LSTM32F$(STM32F)_drivers/build -lSTM32F$(STM32F)xx_drivers -lm -lnosys -lc --specs=nano.specs -Wl,--gc-section 
#LDFLAGS+= -LSTM32F$(STM32F)_drivers/build -lSTM32F$(STM32F)xx_drivers 
LDFLAGS+= -lm -Wl,--gc-section 


#  Compiler/Assembler Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy
REMOVE = rm -f
SIZE = arm-none-eabi-size

#########################################################################

all: .bin/$(PROJECT).bin Makefile 

STM32F$(STM32F)_drivers/build/libSTM32F$(STM32F)_drivers.a:
	@make -C STM32F$(STM32F)_drivers/build

.bin/$(PROJECT).bin: .bin/$(PROJECT).elf Makefile
	@echo "  OBJCOPY $(PROJECT).bin"
	@$(OBJCOPY) --strip-unneeded -S -g -R .stack -O binary .bin/$(PROJECT).elf .bin/$(PROJECT).bin

.bin/$(PROJECT).elf: $(OBJECTS) Makefile $(LSCRIPT)
	@echo "  LD $(PROJECT).elf"
	@$(GCC) $(OBJECTS) $(LDFLAGS)  -o .bin/$(PROJECT).elf
	@$(SIZE) .bin/$(PROJECT).elf

clean:
	$(REMOVE) $(DEPS)
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(LSTFILES)
	$(REMOVE) .bin/$(PROJECT).bin
	$(REMOVE) .bin/$(PROJECT).elf
	$(REMOVE) -r .bin

tools/flash/st-flash:
	make -C tools/flash

#########################################################################

-include $(DEPS)

.bin/%.o: %.c Makefile
	@echo "  GCC $<"
	@mkdir -p $(dir $@)
	@$(GCC) $(GCFLAGS) -o $@ -c $<
	@$(GCC) $(GCFLAGS) -MM $< > $*.d.tmp
	@sed -e 's|.*:|.bin/$*.o:|' < $*.d.tmp > .bin/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
		sed -e 's/^ *//' -e 's/$$/:/' >> .bin/$*.d
	@rm -f $*.d.tmp

.bin/%.o: %.s Makefile 
	@echo "  AS $<"
	@$(AS) $(ASFLAGS) -o $@  $< 

#########################################################################

flash: tools/flash/st-flash all
	tools/flash/st-flash --reset write .bin/$(PROJECT).bin 0x08000000 

.PHONY : clean all flash debug
