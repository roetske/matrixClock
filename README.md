# matrixClock
 clock with 2 matrix led one for time 2nd scrolling date and weekdate
# hardware
esp32 38 pins-
2 matrix 8x8 leds spi connected in series-
levelconverter 3v3 to 5v for spi(play safe esp32 3v3 matrix 5v)-
powerlink 220Vac to 5v 3watt-fuse ptc 0.5A 5V side -fuse 1A 220v side- powerswitch-
old spotlight (yes again but my last one lol)
# description
Clock update via pool.org.
No wifi choose and do setup wifi credentials via accesspoint
matrix 4 modules 8x8 is 2 modules in series spi
1) first matrix time 24H
2) 2nd matrix scrolling date and weekdate
# remark
scrolling text was a challenge
What makes this clock great is the visibilty. Matrix clock you can see from far.
I had issues with spi could not figure it out. The level converter i used is not capable of high speeds.<br />
A better alternative is TXS0108E chip for highspeed bidirectional com.<br />
Only downside standard 8 ports which you only use 4 at max.<br />
