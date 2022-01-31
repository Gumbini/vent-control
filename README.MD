# Vent Control
A simple room ventilation controller using my other project [ESP32 Temperature Sensor](https://github.com/Gumbini/esp32-temperature-sensor)

## Behavior

This program monitors the current room temperature and warns you when it gets too cold.

**SIP functionality is not implemented yet!**

## Build

- Download and extract [PJLIB](https://github.com/pjsip/pjproject/releases)

- Move the folder to the root of the project or change the PJPATH variable in *Makefile*


```
make init
```
After successful initialization you will be prompted to install the library. Please do as prompted.
```
make
```

## Usage

    ./vent-control

You may want to start the program with ```screen``` so you can detach the console.

## Licensing

Any files not written by me, I release under the license under which they were originally released. If the licenses of the third-party code are not explicitly stated in the file *[CONTRIBUTORS.md](CONTRIBUTORS.md)* or the third-party files themselves, please contact the original author for more information on the license used.

My code is released under the global project license in *[LICENSE](LICENSE)*.