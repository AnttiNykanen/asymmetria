#!/bin/sh -

export SDK_PATH=/opt/espressif/ESP8266_RTOS_SDK
export BIN_PATH=./build

case $1 in
build)
	make BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=6
	;;
flash)
	esptool.py write_flash 0x01000 build/upgrade/user1.4096.new.6.bin 
	;;
clean)
	make clean
	make -C libesphttpd clean
	;;
*)
	echo "${0##*/} [build|flash|clean]"
	;;
esac
