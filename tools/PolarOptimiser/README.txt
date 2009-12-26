====================
 PolarOptimiser 1.0
====================

http://sf.net/projects/condor2nav

1. Description
--------------
PolarOptimiser is intended to help with gliders speed polar files tunning.
Not every 3 speed polar curve points make a good speed polar curve in gliding
navigation application. Not every glider mass makes curve "move" correctly
depending of water ballast volume. Thanks to PolarOptimiser application user
is able to verify and tune its speed polar file until he is happy with results.

2. Usage
--------

2.1. Application execution
--------------------------
Run polarOptimiser.exe with WinPilot like polar file provided as an argument.
That polar file must have at least 3 points from speed polar curve but may
provide many more of speed-sink pairs. The only rule here is that the speed must
increase for each next point.

If more that 3 polar curve points are provided in input file, than
polarOptimiser will try to choose the best 3 for you. If succeeded it will
print a table with calculated sink values and LDs for all speeds from the file
and will provide gross sink error (the sum of sink errors for all provided speeds).
Final polar file should be a copy-paste from Actions 4 (Dump WinPilot polar
file) option.

PolarOptimiser application implements XCSoar polar curve equation for now. More
implementations may be provided in future.

2.2. Speed polar curve
----------------------
Application will print speed-sink-LD values for current speed polar curve. The
values will be calculated for provided water ballast volume and lower and higher
speed boundaries values. It will also provide the "Best LD" speed and sink.

2.3. Sink for specific speed
----------------------------
Application will calculate a sink and LD for provided speed and water ballast
volume.

2.4. Best MassDryGross calculation based on full water ballast polar curve
--------------------------------------------------------------------------
That option is useful for glider with water ballast only. It will try to estimate
the best MassDryGross value to make speed polar curve "move" properly with water
ballast volume.

User will have to provide 3 speed-sink pairs for full water ballast
speed polar curve. Based on that PolarOptimiser will search for best weight and
will provide calculated sink and LD values for provided speeds. Gross sink error
that is the sum of all 3 sink value errors will be also provided.

2.5. Dump WinPilot polar file
-----------------------------
Application dumps on the screen actual polar curve in WinPilot format. It may be
easily copy-pasted to user's polar file. That option may be also useful to check
which speed values were used to create the best polar curve if more than 3 speed-sink
pairs were provided during the startup.
