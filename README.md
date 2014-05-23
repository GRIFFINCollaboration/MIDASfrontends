MIDASfrontends
==============

64 bit frontends for MIDAS and their accompanying makefiles and other dependencies, principally used for supporting griffin.js

##CAENHV

This frontend will populate the ODB with the information required by the Dashboard for one CAEN crate of either 6, 12, or 16 slots, populated with CAEN cards of either 12, 24 or 48+primary channels (either polarity, empty slots ok).  It uses the dd_sy2527.c device driver, and the hv.c class driver, with corresponding entries in midas.h - all these should be up to date for a sufficiently recent MIDAS (see Dashboard setup instruction in griffin.js docs).  A few things need to happen to set up the frontend before compilation:

1.  Set the correct IP address in the device driver.  Open dd_sy2527.c (currently in MIDAS under .../drivers/device/dd_sy2527.c), and set the line
    
        IP = STRING : [32] your.ip.address.here

2.  Set the correct number of channels in the frontend.  In the frontend, there's a definintion of a DEVICE_DRIVER varibale:

        DEVICE_DRIVER sy2527_driver[] = {
          {"sy2527", dd_sy2527, 24, NULL, DF_HW_RAMP |.....
          
that '24' should be the total number of channels in your crate.  Remember, 48 channel cards have a primary channel too, so they count as 49.

3.  Set the channel names on the crates according to spec.  You don't technically have to do this to make the frontend or the Dashboard work, but channel names on the crate must match their detector's 10-character name as detailed in the [Detector Nomenclature Page](https://www.triumf.info/wiki/tigwiki/index.php/Detector_Nomenclature) in order to map the HV from the HV tool to the detector views.  

That's it!  Now just compile with

    make
    
and launch with

    ./fesy2527-0 -D &
    
and you should be good to go.  If you have more than one crate, copy the directory as many times as you need, and launch a frontend via this procedure for each.

###Troubleshooting
 - `make` complains:
```
fesy2527.c:54: error: ‘DF_LABELS_FROM_DEVICE’ undeclared here (not in a function)
fesy2527.c:54: error: ‘DF_REPORT_TEMP’ undeclared here (not in a function)
fesy2527.c:54: error: ‘DF_REPORT_STATUS’ undeclared here (not in a function)
fesy2527.c:54: error: ‘DF_REPORT_CHSTATE’ undeclared here (not in a function)
fesy2527.c:54: error: ‘DF_REPORT_CRATEMAP’ undeclared here (not in a function)
```

These are all flags put in to manage the expanded HV driver functionality; look in `include/midas.h` in your MIDAS package for the block

```
#define DF_INPUT       (1<<0)         /**< channel is input           */
#define DF_OUTPUT      (1<<1)         /**< channel is output          */
#define DF_PRIO_DEVICE (1<<2)         /**< get demand values from device instead of ODB */
#define DF_READ_ONLY   (1<<3)         /**< never write demand values to device */
#define DF_MULTITHREAD (1<<4)         //*< access device with a dedicated thread */
#define DF_HW_RAMP     (1<<5)         //*< high voltage device can do hardware ramping */
#define DF_LABELS_FROM_DEVICE (1<<6)  //*< pull HV channel names from device */
#define DF_REPORT_TEMP        (1<<7)  //*< report temperature from HV cards */
#define DF_REPORT_STATUS      (1<<8)  //*< report status word from HV channels */
#define DF_REPORT_CHSTATE     (1<<9)  //*< report channel state word from HV channels */
#define DF_REPORT_CRATEMAP    (1<<10) //*< reports an integer encoding size and occupancy of HV crate */
```

If you got the error above, the last 5 lines are probably missing.  Add them in.

##GRIFClk

The GRIFClk frontend handles the MSCB connection to the chip-scale atomic clocks used in GRIFFIN.  To set up, just replace all the instances of 'mscb571.triumf.ca' with the addess of the clock you want; launch a separate frontend for each clock.  For those without convenient search and replace tools handy, try something like

    sed 's@mscb571.triumf.ca@computer.place.xx@g' mscb_fe.c > new_mscb_fe.c
