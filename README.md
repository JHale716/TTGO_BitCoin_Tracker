# TTGO BitCoin Tracker

An adaption of the [TTGO T4 Demo](https://github.com/LilyGO/TTGO-T4-DEMO)
to run on the [TTGO T Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)

  I liked the idea and have the TTGO T Display so adapted it to suit.

  Remember to update the wifi stuff before uploading ;) Happy coding!
  
  You will also need libraries in addition to the 'normal' inbuilt ones:
  - [TFT eSPI](https://github.com/Bodmer/TFT_eSPI)
  - [Button2](https://github.com/LennartHennigs/Button2)
  
  Additional info:
  - Button 2 is defined, as a carry over from the TTGO_T_Display code, may or may not do anything with these, but there are two buttons to do something with.  
  - The TFT_eSPI User_Setup_Select.h file needs to be edited from the default to compile correctly, as per the instructions it the related git repo readme.md

## A few issues since discovered that may trip new users:

### Ardunio Json:

  If you have Arduino Json installed it is likely version 6, this sketch needs version 5, and I haven't the inclination to pull it across. 

  However, there is a work around, while still having Arduino Json 6 installed.

  All of this is from the Arduino Json v5 site. Any linking is for convenience purposes and no code changes are intended or made to the ArduinoJson5xxx.h files. Original source https://arduinojson.org/v5/doc/installation/ (i.e. you could achieve the same thing with a download and manual placement)

  To execute:

    Break out you favourite terminal editor
    cd into you sketechbook libraries folder.
    From there:
  ```
  $ git clone https://github.com/JHale716/ArduinoJson5.git ArduinoJson-v5.13.5
  ```
  This will place the repo and the ArduinoJson-v5.13.5.h file for ArduinoJson5 into the ArduinoJson-v5.13.5 folder.

  In your sketch, substitute ArduinoJson.h for ArduinoJson-v5.13.5.h and you'll be good to go!

### Python:

  Python 2 & Python 3... the esptools.py scripts can do some interesting things.

  If you are getting Serial errors, then this issue log may be useful. [ESPTool Issue 324](https://github.com/espressif/esptool/issues/324)

  Specifically for a reasonably clean ubuntu install the following was helpful
  ```
  sudo apt-get update
  sudo apt-get upgrade
  sudo pip install esptool
  sudo pip install --ignore-installed pyserial
  ```
  There is a good chance your system is looking at Python3 or you have pyserial for python3 installed...
  The above commands install for python2, as I'm writing this, v2.7.

From here, it should be working...
