# M18ST05 #

This is a small commandline utility for Medion's VFD Displays
built into some of their computers.

The linux tool is below 15k in size.
Future development will bring a DOS utility, too.

It seems, there are different models, M18ST05A and M18ST05B.
I don't know about differences yet.

# Pinout / Hardware connections #
![Pinout](pinout.jpg)

# Build it #
Go to linux-c folder and simply type make. 
It just needs gcc on your system.

# Usage #
Just run it without arguments, it will print a help.
```
./m18st05a
```
Some simple examples:
```
$ ./m18st05a clear
$ ./m18st05a textl0 "This is Line 0"
$ ./m18st05a textl1 "This is the second Line"
```

# Early development version #
This program is in a very early stage of development and may be buggy.
It does not cover all the functions provided by this display.

# Pictures #
![TwoDisplays](2displays.jpg)


# Links #
TODO
