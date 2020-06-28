.PHONY: all clean load

#TARGET= src/main.c
TARGET= src/main.cpp
INC= -Iinc/
INC+= -Ilib/
MCU= cortex-m3
OBJCOPY=arm-none-eabi-objcopy
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
SIZE=arm-none-eabi-size
CFLAGS=-mthumb -mcpu=$(MCU) $(INC) -g -O0 -Wall -ffreestanding -fno-common -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostartfiles 
#-nostdlib   
#-fno-common -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables #for C++ чтобы избежать появления дополнительных секций, связанных с обработкой исключительных ситуаций.
#--gc-sections    #Компоновщик в состоянии избавиться от секций, на которые никто не ссылается и которые не были явно указаны как необходимые в сценарии компоновки.
#-ffunction-sections -fdata-sections   #флаги компилятора, которые разбивают функции и данные в разные секции: когда каждая функция и объект помещены в независимые секции, компоновщик может от них избавиться:
#-ffreestanding #приложение у нас самостоятельное и ни от каких ОС не зависит.
ASFLAGS=-mthumb -mcpu=$(MCU) -g -Wall

all: main.bin main.lst main.elf

main.bin: main.elf
	$(OBJCOPY) main.elf main.bin -O binary

load: main.bin
	st-flash write main.bin 0x08000000

main.lst: main.elf
	arm-none-eabi-objdump -D main.elf > main.lst

main.elf: startup.o  main.o
	$(CC) -o main.elf -T lib/stm32f103.ld startup.o  main.o -mthumb -march=armv7-m -nostartfiles
#--gc-sections
	$(SIZE) main.elf  
#--specs=nosys.specs -g3 -Wl, -Xlinker --gc-sections
#system_stm32f10x.o
#$(LDFL) 
#	--data-sections
#	--specs=nosys.specs

startup.o: lib/startup.s
	$(CC) -c  lib/startup.s -o startup.o $(ASFLAGS) 
	
#system_stm32f10x.o: lib/system_stm32f10x.c
	#$(CC) -c lib/system_stm32f10x.c -o system_stm32f10x.o -g $(CFLAGS)
	#$(CFL)

main.o: $(TARGET) inc/main.h inc/modbus.h inc/adc.h
	$(CC)  -c $(TARGET) -o main.o -I$(LIB) -I$(INC) $(CFLAGS) 
#	arm-none-eabi-objdump main.o -h
	
clean:
	rm -rf *.o *.elf *.lst *.bin #*.map
