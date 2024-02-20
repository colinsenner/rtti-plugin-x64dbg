# x64dbg RTTI Plugin

A plugin to quickly retrieve RTTI information (if present) at a memory address.

There's a wealth of information present in RTTI that's of use when reverse engineering.  This plugin aims to make this information easily available to the user.

## Installation
1. Compile the solution in Release mode for x86 and x64.
2. Copy 'Rtti.dp32' from the /bin to your \x64dbg\release\x32\plugins directory.
3. Copy 'Rtti.dp64' from the /bin to your \x64dbg\release\x64\plugins directory.

## Usage

![Rtti Usage image](images/Rtti.jpg)

## Tests
There is an included project called 'ClassRTTIPluginTest'.  You can compile these and use them to test the functionality.  On compilation these are copied to the /bin directory.

The tests have breakpoints after the creation of the class, so open the tests in x64dbg, and run until you hit the breakpoint.

## Reporting Crashes

If you find a crash please submit an issue on gitlab.

## References
There are references for finding Rtti information included in the /docs directory.  Thanks to the authors of the following papers:

[Reversing C++ by Paul Vincent Sabanal and Mark Vincent Yason](https://www.blackhat.com/presentations/bh-dc-07/Sabanal_Yason/Paper/bh-dc-07-Sabanal_Yason-WP.pdf)

[Visual C++ RTTI Inspection](https://blog.quarkslab.com/visual-c-rtti-inspection.html)

[Reversing Microsoft Visual C++ Part II: Classes, Methods and RTTI - by Igorsk](http://www.openrce.org/articles/full_view/23)


[Recovery of Object Oriented Features from C++ Binaries](https://www.ece.umd.edu/~barua/yoo-APSEC-2014.pdf)