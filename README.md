## SimpleWeek (dutch)

![Action shot](https://raw.github.com/atkaper/pebble-simpleweek-nl/master/releases/simpleweek.png)

Note: converted to firmware/sdk 2.0, as of March 9. 2014.
Note: small update, to compile with https://cloudpebble.net/, as of September 26. 2014. Version changed from 2.0.0 to 2.1.

Based on Simplicity & Complexity.


SimpleWeek is a quick and dirty mod of the Simplicity & Complexity watch face for Pebble which adds Week Day and Week Number to the main screen.
It has been translated to Dutch, and items slightly repositioned / reformatted.
The text for week/date/weekday is only redrawn if changed to conserve battery.

Added period numbers also. A period is the week number divided by 4 (e.g. 13 periods in a year).


Binary release:
https://github.com/atkaper/pebble-simpleweek-nl/raw/master/releases/simpleweek.pbw


### Building and installing the watch face

Got easier with SDK 2.0 ;-)

    pebble build
    pebble install


