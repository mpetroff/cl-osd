# Basic makefile for cl-osd originally contributed by davico(rcgroups).
# Updated by Matthew Petroff

CC=avr-gcc
CFLAGS=-funsigned-char -funsigned-bitfields -Os -fno-split-wide-types -g0 -Wall -std=gnu99 -Wno-main -mmcu=atmega88

all: e-osd e-osd-gps g-osd

e-osd:
	$(CC) $(CFLAGS) -DE_OSD -c -o cl-osd_e-osd.o cl-osd.c
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature "cl-osd_e-osd.o" "cl-osd_e-osd.hex"
#	avr-objdump -h -S "cl-osd_e-osd.o" > "cl-osd_e-osd.lss"
	avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex "cl-osd_e-osd.o" "cl-osd_e-osd.eep"

e-osd-gps:
	$(CC) $(CFLAGS) -DE_OSD_GPS -c -o cl-osd_e-osd-gps.o cl-osd.c
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature "cl-osd_e-osd-gps.o" "cl-osd_e-osd-gps.hex"
#	avr-objdump -h -S "cl-osd_e-osd-gps.o" > "cl-osd_e-osd-gps.lss"
	avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex "cl-osd_e-osd-gps.o" "cl-osd_e-osd-gps.eep"

g-osd:
	$(CC) $(CFLAGS) -DG_OSD -c -o cl-osd_g-osd.o cl-osd.c
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature "cl-osd_g-osd.o" "cl-osd_g-osd.hex"
#	avr-objdump -h -S "cl-osd_g-osd.o" > "cl-osd_g-osd.lss"
	avr-objcopy -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex "cl-osd_g-osd.o" "cl-osd_g-osd.eep"

clean:
	-rm -f *.o *.lss *.hex *.eep
