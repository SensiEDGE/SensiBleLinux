# SensiBLE-HummingBoard integration example 
This is example of integration SensiBLE with HummingBoard to IBM Watson IoT Platform

<img src="https://github.com/SensiEDGE/SensiBleLinux/blob/develop/resources/logo.png"
     alt="logo"/>
## Dependencies
git  
gcc  
g++  
cmake  
make  
pkg-config  
libbluetooth-dev  
libreadline-dev  
libgtk2.0-dev  
libperl-dev  
curl  
unzip
gattlib  
cmocka-1.1.0  
iot-embeddedc

## Installation   
**1.  Install dependency libraries. In Debian Linux you can do this by commands:**  

`sudo apt update`  
`sudo apt install git gcc g++ cmake make pkg-config libbluetooth-dev libreadline-dev libgtk2.0-dev libperl-dev curl unzip`  

**2.  Get and compile gattlib libraries. You can do this by following commands:**  
 
`git clone https://github.com/labapart/gattlib.git`  
`cd gattlib`  
`mkdir build`  
`cd build`  
`cmake -DGATTLIB_FORCE_DBUS=TRUE ..`  
`make`  
`cd ../..`  

**3.  Get and compile cmocka libraries. You can do this by following commands:**  

`curl -O https://git.cryptomilk.org/projects/cmocka.git/snapshot/cmocka-1.1.0.zip`  
`unzip cmocka-1.1.0.zip`  
`rm -f cmocka-1.1.0.zip`  
`cd cmocka-1.1.0`  
`mkdir build`
`cd build`  
`cmake ..`  
`make`  
`sudo make install`  
`cd ../..`  

**4.  Get and compile ibm-watson-iot libraries. You can do this by following commands:**  

`git clone https://github.com/ibm-watson-iot/iot-embeddedc.git`  
`cd iot-embeddedc`  
`./setup.sh`  
`mkdir build`  
`cd build`  
`cmake ..`  
`make`  
`cd ../..`  

**5.  Build all in one executable. You can do this by following commands:**  

`chmod +x make.sh`  
`./make.sh`  
`chmod +x run.sh`  

##  Run

Execute next command to run the example:  
`sudo ./run.sh`  
After example start will be displayed devices list with their MAC addresses and names:

*Device Client Connected to quickstart.messaging.internetofthings.ibmcloud.com Platform in QuickStart Mode  
Connection Successful. Press Ctrl+C to quit  
Discovered D0:CF:5E:0A:23:D8 - 'BleButton_23D8'  
Discovered 02:80:E1:00:00:AA - 'AM1V300'  
Discovered 51:84:DB:28:95:62*

<img src="https://github.com/SensiEDGE/SensiBleLinux/blob/develop/resources/working_process.png"
     alt="Click to see screenshot"/>

In next prompt:  
*Scan completed. Now please type your BLE MAC-adress:*  
you should insert one of the device addresses, belong to SensiBLE device, for example *02:80:E1:00:00:AA* and press Enter. After this open browser and go to next URL to see the SensiBle data.  
https://quickstart.internetofthings.ibmcloud.com/#/device/HB_SE_1/sensor/


<img src="https://github.com/SensiEDGE/SensiBleLinux/blob/develop/resources/ibm_watson_iot.png"
     alt="Click to see screenshot"/>
