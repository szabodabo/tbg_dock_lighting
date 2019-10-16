#!/bin/bash

set -e

ARDUINO_BUILD="/tmp/arduino_build"
ARDUINO="/opt/arduino/arduino-1.8.9"
AVRDUDE="$ARDUINO/hardware/tools/avr/bin/avrdude"

rm -rf $ARDUINO_BUILD /tmp/arduino_cache
mkdir $ARDUINO_BUILD
mkdir /tmp/arduino_cache

/opt/arduino/arduino-1.8.9/arduino-builder \
	-compile \
	-logger=machine \
	-hardware /opt/arduino/arduino-1.8.9/hardware \
	-hardware /home/dakota/.arduino15/packages \
	-tools /opt/arduino/arduino-1.8.9/tools-builder \
	-tools /opt/arduino/arduino-1.8.9/hardware/tools/avr \
	-built-in-libraries /opt/arduino/arduino-1.8.9/libraries \
	-libraries /home/dakota/Arduino/libraries \
	-fqbn=attiny:avr:ATtinyX4:cpu=attiny44,clock=internal1 \
	-ide-version=10809 \
	-build-path $ARDUINO_BUILD \
	-warnings=default \
	-build-cache /tmp/arduino_cache \
	-prefs=build.warn_data_percentage=75 \
	-prefs=runtime.tools.arduinoOTA.path=/opt/arduino/arduino-1.8.9/hardware/tools/avr \
	-prefs=runtime.tools.arduinoOTA-1.2.1.path=/opt/arduino/arduino-1.8.9/hardware/tools/avr \
	-prefs=runtime.tools.avrdude.path=/opt/arduino/arduino-1.8.9/hardware/tools/avr \
	-prefs=runtime.tools.avrdude-6.3.0-arduino14.path=/opt/arduino/arduino-1.8.9/hardware/tools/avr \
	-prefs=runtime.tools.avr-gcc.path=/opt/arduino/arduino-1.8.9/hardware/tools/avr \
	-prefs=runtime.tools.avr-gcc-5.4.0-atmel3.6.1-arduino2.path=/opt/arduino/arduino-1.8.9/hardware/tools/avr \
	-verbose \
	/home/dakota/GoogleDrive/Train/dock_lighting/dock_lighting.ino

$AVRDUDE -p ATtiny44 -c usbtiny -C "$ARDUINO/hardware/tools/avr/etc/avrdude.conf" \
	-U flash:w:$ARDUINO_BUILD/dock_lighting.ino.elf


