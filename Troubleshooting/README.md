# RTC (Real-Time Clock) Calibration

## Requirements
- [USB to Arduino cable](https://www.amazon.com/Arduino-Data-Sync-Cable-Microcontroller/dp/B08RCJXY1Z/ref=sr_1_1_sspa?crid=1TM8BLWQNUJ8L&dib=eyJ2IjoiMSJ9.CqfucQoe_wHNE9W2Hq-ThX3FrNwT4JMJfevmzZNSyLv9sa-nbIdZphJC8AXYYUTxCwwT1F4njmNIfwhoSj6wkbXZoAaiEB-iC2gKzZfVsbq3iy8YsBFTRIkFkyKhjP9WMkW05tdIJ2UwkxMR98Ihm998sHHjbjKr_O4C_Fn2GLB1p05SBqW4-CsLn3-Heu-kobwuc3axw92Vfmrkmw-SRTWaqe7w14W2upAxn4PYWNo.eOpmyZIMwtnGRCISYHlSuaH1407X0aYSBPmQ5NuzwMk&dib_tag=se&keywords=arduino+to+usb+cable&qid=1717087440&sprefix=arduino+to+usb+ca%2Caps%2C423&sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1)
- Computer with a viable USB port

## Steps

1. Install the Arduino IDE from [here](https://www.arduino.cc/en/software).
2. Download the code from the main page of this GitHub repository under the "Code" folder and download the file labeled "PMdatalogger.ino".

   ![Download Code](Troubleshooting/DownloadCodeTutorial.png)

3. Connect the Arduino to the computer using the USB-Arduino cable.

   ![Connect Arduino](Troubleshooting/ArduinoPortTutorial.png)

4. Open the IDE and click on the library manager on the left-hand side.

   ![Library Manager](Troubleshooting/LibraryManagerTutorial.png)

5. Type `pm2008` and click install library.

   ![Install Library](Troubleshooting/PM2008Tutorial.png)

6. Open the code provided and change the number to the corresponding sensor box.

   ![Change Sensor Number](Troubleshooting/ChangeNameTutorial.png)

7. Click upload code.

   ![Upload Code](Troubleshooting/UploadCodeTutorial.png)

The RTC should now be set properly!
