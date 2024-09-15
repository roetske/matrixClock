# matrixClock
 clock with 2 matrix led one for time 2nd scrolling date and weekdate<br />
# hardware
esp32 38 pins-<br />
2 matrix 8x8 leds spi connected in series-<br />
levelconverter 3v3 to 5v for spi(play safe esp32 3v3 matrix 5v)-<br />
powerlink 220Vac to 5v 3watt-fuse ptc 0.5A 5V side -fuse 1A 220v side- powerswitch-<br />
old spotlight (yes again but my last one lol)<br />
# description
Clock update via pool.org.<br />
No wifi choose and do setup wifi credentials via accesspoint<br />
matrix 4 modules 8x8 is 2 modules in series spi<br />
1) first matrix time 24H<br />
2) 2nd matrix scrolling date and weekdate<br />
# remark
Scrolling text was a challenge
What makes this clock great is the visibilty. Matrix clock you can see from far.<br />
I had issues with spi could not figure it out.(reaction time and scrolling speed were not very reactive).<br />
The level converter i used is not capable of high speeds. I found out doing some test with wled which uses also spi.<br />
There the same issue setup worked but responsiveness control nok. <br />
Posted this at forum wled and got the answer levelconverter good for i2c but not fast enough for spi.<br />
A better alternative is TXS0108E chip for highspeed bidirectional com.<br />
Only downside standard 8 ports which you only use 4 at max.<br />
# Updates and libraries
Updating is dangerous for projects , nothing more annoying than running code no longer compiling.<br />
One year later you try your project and you get compile trouble. It makes ruins your day:(<br />
I forget but should enter it in the code in comment versions used of the libraries.<br />
arduino ide 2.3.2<br />
board esp32 dev<br />
autoconnect 1.4.2<br />
board library esp32 systems 2.0.17 <br />
later lib versions board esp32 have important commands that changed are conflicting with autoconnect<br />
Did compile test with these version and compiled as it should :)<br />
