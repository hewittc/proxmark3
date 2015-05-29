proxmark3lcd
============

This repository contains working code for a Proxmark3 LCD device to read/write/emulate RFID tags. I have made modifications for building with GCC 5.1.

Currently, only 125kHz HID ProxCard II tags have been successfully read and emulated.
Additionally, a 13.56MHz HID iClass tag has also been successfully read.

This project incorporates code from two sources:

* ~~[Proxmark3 @ Google Code](http://proxmark3.googlecode.com/svn/trunk)~~
* [Proxmark3 @ Github](https://github.com/proxmark/proxmark3)
* [Null Space Labs](http://www.032.la/svn/listing.php?repname=032&path=%2FProxmark3_LCD%2Ftrunk%2Fsource%2F&#a5f33ddfcd9ad27f6841dd37aa0812211)

Other resources: 
* [Null Space Labs' build notes](http://wiki.032.la/nsl/Proxmark3_LCD)
* [Original Proxmark3](http://cq.cx/proxmark3.pl)

I had a lot of trouble with flashing the device initially but ended up having a lot of success using a [Bus Pirate](http://dangerousprototypes.com/docs/Bus_Pirate) and OpenOCD. An example configuration file is present in tools/. Once bootrom.elf and fullimage.elf are built, it is possible to use the following commands in the telnet interface to flash the device:

\> halt  
\> flash erase\_sector 0 0 15  
\> flash erase\_sector 1 0 15  
\> flash write\_image ../armsrc/obj/fullimage.elf  
\> flash write\_image ../bootrom/obj/bootrom.elf  

Now I'm using a Segger J-Link and things are working pretty well!
