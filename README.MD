# Vent Control
A simple room ventilation controller using my other project [ESP32 DS18B20 Web API](https://github.com/Gumbini/esp32-ds18b20-webapi)

## Behavior

This program monitors the current room temperature and warns you when it gets too cold.

## Build

**SIP functionality is not implemented yet!**

Buzzing will do for now. Use ```make dirs``` to create necessary directories which would have been automatically created by the striked command.

<strike>
- Download and extract [PJLIB](https://github.com/pjsip/pjproject/releases)

- Move the folder to the root of the project or change the PJPATH variable in *Makefile*

```
make init
```
After successful initialization you will be prompted to install the library. Please do as prompted.
</strike>
```
make
```

## Usage

```
./vent-control
```

You may want to start the program with ```screen``` so you can detach the console.

## Licensing

Any code not written by me, I release under the license under which it was originally released. If the license of any third-party code is not explicitly stated in the file *[CONTRIBUTORS.md](CONTRIBUTORS.md)* or with the third-party code itself, please contact the original author for more information on the license used.

My code is released under the global project license in *[LICENSE](LICENSE)*.
