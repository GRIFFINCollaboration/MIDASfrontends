/********************************************************************\

  Name:         mscb_fe.c
  Created by:   Stefan Ritt

  Contents:     Example Slow control frontend for the MSCB system

  $Id: sc_fe.c 20457 2012-12-03 09:50:35Z ritt $

\********************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "mscb.h"
#include "midas.h"
#include "msystem.h"
#include "class/multi.h"
#include "class/generic.h"
#include "device/mscbdev.h"
#include "device/mscbhvr.h"

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "GRIFClk Frontend";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms    */
INT display_period = 1000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 10 * 10000;

/*-- Equipment list ------------------------------------------------*/

/* device driver list */
DEVICE_DRIVER mscb_driver[] = {
   {"SCS2001", mscbdev, 0, NULL, DF_INPUT | DF_MULTITHREAD},
   {""}
};

EQUIPMENT equipment[] = {

   {"GRIFClk",              /* equipment name */
    {10, 0,                     /* event ID, trigger mask */
     "SYSTEM",                  /* event buffer */
     EQ_SLOW,                   /* equipment type */
     0,                         /* event source */
     "MIDAS",                   /* format */
     TRUE,                      /* enabled */
     RO_ALWAYS,
     60000,                     /* read every 60 sec */
     10000,                     /* read one value every 10 sec */
     0,                         /* number of sub events */
     60,                        /* log history every 60 second */
     "", "", ""} ,
    cd_multi_read,              /* readout routine */
    cd_multi,                   /* class driver main routine */
    mscb_driver,                /* device driver list */
    NULL,                       /* init string */
    },

  {""}
};

/*-- Dummy routines ------------------------------------------------*/

INT poll_event(INT source[], INT count, BOOL test)
{
   return 1;
};
INT interrupt_configure(INT cmd, INT source[], PTYPE adr)
{
   return 1;
};

/*-- Function to define MSCB variables in a convenient way ---------*/

void mscb_define(char *submaster, char *equipment, char *devname, DEVICE_DRIVER *driver, 
                 int address, unsigned char var_index, char *name, double threshold)
{
   int i, dev_index, chn_index, chn_total;
   char str[256];
   float f_threshold;
   HNDLE hDB;

   cm_get_experiment_database(&hDB, NULL);

   if (submaster && submaster[0]) {
      sprintf(str, "/Equipment/%s/Settings/Devices/%s/Device", equipment, devname);
      db_set_value(hDB, 0, str, submaster, 32, 1, TID_STRING);
      sprintf(str, "/Equipment/%s/Settings/Devices/%s/Pwd", equipment, devname);
      db_set_value(hDB, 0, str, "meg", 32, 1, TID_STRING);
   }

   /* find device in device driver */
   for (dev_index=0 ; driver[dev_index].name[0] ; dev_index++)
      if (equal_ustring(driver[dev_index].name, devname))
         break;

   if (!driver[dev_index].name[0]) {
      cm_msg(MERROR, "Device \"%s\" not present in device driver list", devname);
      return;
   }

   /* count total number of channels */
   for (i=chn_total=0 ; i<=dev_index ; i++)
      chn_total += driver[i].channels;

   chn_index = driver[dev_index].channels;
   sprintf(str, "/Equipment/%s/Settings/Devices/%s/MSCB Address", equipment, devname);
   db_set_value_index(hDB, 0, str, &address, sizeof(int), chn_index, TID_INT, TRUE);
   sprintf(str, "/Equipment/%s/Settings/Devices/%s/MSCB Index", equipment, devname);
   db_set_value_index(hDB, 0, str, &var_index, sizeof(char), chn_index, TID_BYTE, TRUE);

   if (threshold != -1) {
     sprintf(str, "/Equipment/%s/Settings/Update Threshold", equipment);
     f_threshold = (float) threshold;
     db_set_value_index(hDB, 0, str, &f_threshold, sizeof(float), chn_total, TID_FLOAT, TRUE);
   }

   if (name && name[0]) {
      sprintf(str, "/Equipment/%s/Settings/Names Input", equipment);
      db_set_value_index(hDB, 0, str, name, 32, chn_total, TID_STRING, TRUE);
   }

   /* increment number of channels for this driver */
   driver[dev_index].channels++;
}

/*-- Error dispatcher causing communiction alarm -------------------*/

void scfe_error(const char *error)
{
   char str[256];

   strlcpy(str, error, sizeof(str));
   cm_msg(MERROR, "scfe_error", str);
   al_trigger_alarm("MSCB", str, "MSCB Alarm", "Communication Problem", AT_INTERNAL);
}

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init()
{
   /* set error dispatcher for alarm functionality */
   mfe_set_error(scfe_error);

   /* set maximal retry count */
   mscb_set_max_retry(100);

   /*---- set correct ODB device addresses ----*/

   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 0, "ClockEnB", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 1, "Master", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 2, "SyncSel", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 3, "FanSel", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 4, "RefSel", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 5, "ExtClk", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 6, "ExtSync", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 7, "SATAClk", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 8, "SATASync", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 9, "ch0_high", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 10, "ch0_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 11, "ch0_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 12, "ch0_ph", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 13, "ch1_high", 0.1); 
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 14, "ch1_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 15, "ch1_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 16, "ch1_ph", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 17, "ch2_high", 0.1); 
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 18, "ch2_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 19, "ch2_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 20, "ch2_ph", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 21, "ch3_high", 0.1); 
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 22, "ch3_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 23, "ch3_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 24, "ch3_ph", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 25, "ch4_high", 0.1); 
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 26, "ch4_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 27, "ch4_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 28, "ch4_ph", 0.1);   
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 29, "ch5_high", 0.1); 
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 30, "ch5_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 31, "ch5_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 32, "ch5_ph", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 33, "ch6_high", 0.1); 
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 34, "ch6_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 35, "ch6_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 36, "ch6_ph", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 37, "ch7_high", 0.1); 
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 38, "ch7_low", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 39, "ch7_byp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 40, "ch7_ph", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 41, "CSAC_ena", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 42, "CSAC_sta", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 43, "CSAC_mod", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 44, "CSAC_air", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 45, "CSAC_pwr", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 46, "CSAC_tcx", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 47, "CSAC_lsr", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 48, "CSAC_htr", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 49, "CSAC_tmp", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 50, "CSAC_sn", 0.1);
   mscb_define("mscb571.triumf.ca", "GRIFclk", "SCS2001", mscb_driver, 1, 51, "CSAC_ver", 0.1);
   return CM_SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
   return CM_SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{
   return CM_SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{  
   /* don't eat up all CPU time in main thread */
   ss_sleep(100);

   return CM_SUCCESS;
}

/*------------------------------------------------------------------*/
