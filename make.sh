arm-none-eabi-gcc -I/opt/devkitpro/libgba/include -c main.c -mthumb-interwork -mthumb -O2 -o main.o
arm-none-eabi-gcc main.o -I/opt/devkitpro/libgba/include -mthumb-interwork -mthumb -specs=gba.specs -o main.elf
arm-none-eabi-objcopy -v -O binary main.elf main.gba
gbafix main.gba
