Car front wiper relay
=====================

> *Firstly designed for my Lifan Smily (Lifan 320) car*

This relay places instead of the standart one in the same place. Brings more functionality and less irritation.

New functionality:
 - Delay between water switch is on and first wiper swing come;
 - Less swings for less water;
 - Remaining water removing action;
 - Adjustable delay in intermittent mode.


Component list
--------------
 - 1 x [L78L05ABUTR] voltage regulator;
 - 1 x [pic12F683] MCU;
 - 1 x [IRLML2402] N-channel MOSFETs;
 - 1 x [IRFR5305] P-channel MOSFET;
 - 1 x [VS-12CWQ10FN] shottky diode;
 - 2 x [BC847A] NPN transistors;
 - 2 x 0805 10nF capacitors;
 - 1 x 0805 100nF capacitor;
 - 1 x 0805 300nF capacitor;
 - 1 x 0805 4.7mF capacitor;
 - 2 x 0805 100Ohms resistors;
 - 4 x 0805 10kOhms resistors;
 - 2 x 0805 47kOhms resistors;
 - 1 x 0805 100kOhms resistor.

Directory tree
--------------
 - circuit — device schematic diagram designed in Splan 7;
 - layout — PCB layout designed in Sprint Layout 6;
 - firmware — MCU firmware, Hi-Tech C STD 9.60PL3;
 - model — simple imitation model designed in Proteus 7.

Code constants
--------------

You can change device behavior changing next constants. Note that all times measured in ticks, there are 30 ticks per second.

`ADJUSTABLE_INTERMITTENT_DELAY_ON`

Enables an adjustable delay in intermittent mode. Comment this row if you want a stable delay time mode.

`REMAINING_WATER_REMOVING_ON`

Enables a remaining water removement mode on. Comment this row if you want a stable delay time mode.

`SWITCH_DEBOUNCE_TICKS 3`

Switch debounce code work time *(max 255)*

`WATER_WIPER_WAIT_TIME 15`

A delay time before a wiper swing after a water switch is on *(max 65533)*

`WATER_WIPER_SWITCH_MODE_TIME 30`

Water switch work time dividing a 'small' and a 'big' wiper work time after it *(max 255)*

`WATER_WIPER_REMOVEMENT_WAIT_TIME 150`

Delay between last wiper swing after a water switch is off and a water removement swing.

`WATER_WIPER_WORK_TIME_SMALL 30`

'Small' wiper work time after a water switch is off *(max 65533)*

`WATER_WIPER_WORK_TIME_BIG 240`

'Big' wiper work time after a water switch is off *(max 65533)*

`INTERMITTENT_WIPER_WORK_TIME 30`

Enough time to start a wiper in intermittent mode *(max 65533)*

`INTERMITTENT_WIPER_WAIT_TIME 240`

Default (stable) intermittent delay time between wiper swings *(max 65533)*

`INTERMITTENT_WIPER_WAIT_TIME_MIN 150`

Minimal intermittent delay time *(max 65533)*

`INTERMITTENT_WIPER_WAIT_TIME_MAX 3600`

Maximal intermittent delay time *(max 65533)*

Photos, installation, etc
-------------------------
You can read about this device in my car blog later.






[L78L05ABUTR]:http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00000446.pdf
[pic12F683]:http://ww1.microchip.com/downloads/en/devicedoc/41211d_.pdf
[IRLML2402]:http://www.irf.com/product-info/datasheets/data/irlml2402.pdf
[IRML9301]:http://www.irf.com/product-info/datasheets/data/irlml9301pbf.pdf
[IRFR5305]:http://www.irf.com/product-info/datasheets/data/irfr5305pbf.pdf
[VS-12CWQ10FN]:http://www.vishay.com/docs/94135/12cwq10f.pdf
[BC847A]:http://www.nxp.com/documents/data_sheet/BC847_SER.pdf