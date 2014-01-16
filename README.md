MIDASfrontends
==============

64 bit frontends for MIDAS and their accompanying makefiles and other dependencies, principally used for supporting griffin.js

###CAENHV

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


###GRIFClk

The GRIFClk frontend handles the MSCB connection to the chip-scale atomic clocks used in GRIFFIN.  To set up, just replace all the instances of 'mscb571.triumf.ca' with the addess of the clock you want; launch a separate frontend for each clock.  For those without convenient search and replace tools handy, try something like

    sed 's@mscb571.triumf.ca@computer.place.xx@g' mscb_fe.c > new_mscb_fe.c
