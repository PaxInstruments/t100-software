#t100-software

Computer side software applications for t100 USB thermocouple adapter.

Currently we have only commandline applications but soon we'll have a QT based basic GUI program. 

We are using hidapi library from signal11 as a backend. It is an open source, cross platform, USB-HID interface library. For compilation, we don't need to download it externally. Latest stable version of the hidapi library currently resides at the _/commandline/hidapi/_ folder. 

Hidapi itself needs libusb package for Linux. On the other hand for Windows and OSX, hidapi uses the core libraries come with the operating system.

##Commandline applications

Please note that currently softwares are only tested with OSX and can't be compiled for other operating system as is.

For __OSX__, go to the application folder from the terminal and simply hit _make_ to compile the software. In order to run the generated software, hit _./{name_of_the_binary}_

###Which program does what?

__app_basicReadings__: Prints raw ADC voltage, cold junction temperature and the calculated K-type thermocouple temperature. 

__app_interactiveConnection__: Lists all the available t100 devies on the usb bus and tries to connect to the one you select from the commandline.

__app_random_serial__: Assigns random serial number to the currently attached t100 device. Normally each t100 starts with the serial number of "512" but we can use this program to assign random serial number to differentiate different devices. Also this program will be modified after we increase the length of the serial number from 3 to some other number.

__app_searchDevices__: Lists all the available t100 devices on the usb bus and exits.

##What is t100_lib?

It is a C++ class which abstracts the hidapi calls and does neccessary raw unit conversions. You can find the documentation for this library at the t100_lib.h file.

##How do we calculate the thermocouple temperature?

We are using 9th order polynomial for the conversion. The coefficients for that polynomial are coming from _ITS-90 Thermocouple Database_. Copy of that database file resides at the /docs/ folder. 

## T100 Projects

- [Electronics](https://github.com/PaxInstruments/t100-electronics)
- [Enclosure](https://github.com/PaxInstruments/t100-enclosure)
- [Firmware](https://github.com/PaxInstruments/t100-firmware)
- [Software](https://github.com/PaxInstruments/t100-software)
