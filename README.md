# DEF CON 29 Badge
by MK Factor

![alt text](https://github.com/compukidmike/Defcon29/blob/main/Hardware/Defcon29BadgeTypes.jpg "Badge Types")

Here's the code and some design files for the DEF CON 29 badge. Feel free to modify and share to your heart's content.

Compiled .uf2 files can be found under Firmware/Compiled. These are the firmware files that were released during the con that fix the 31-character issue. To put them on your badge, hold the bottom-right button when plugging it into your computer. The badge will start up in bootloader mode and will appear as a mass storage device. Drag/Drop or Copy/Paste the new .uf2 file onto the badge. It will apply the new firmware and automatcially restart.

NOTE: Flashing new firmware should not reset your challenge/game stats. They are stored in a separate EEPROM location in flash.

To generate your own .uf2 files, take the compiled .hex file and use the UF2 converter uf2conv.py available from https://github.com/microsoft/uf2/ (in the utils directory).

I used ~~Atmel~~Microchip Studio 7.0 and the ASF Framework. The install files for the exact version I used is available on the DEF CON Media Server https://media.defcon.org/DEF%20CON%2029/DEF%20CON%2029badge/

The main microcontroller is an ATSAMD21G16B. It has 64KB of flash and 8KB of RAM. The first 8KB of flash is used for the bootloader, leaving 56KB for the main program. The bootloader is write protected by internal fuses in the microcontroller. If you want to remove it, you'll need to use an SWD programmer or write your own program that unlocks the booloader section and erases/overwrites it. Do this at your own risk.

Pull requests *may* be merged if they add functionality while retaining all original functionality. If you make something cool that doesn't fit this description, let me know and I'll try to add it to a list here.
