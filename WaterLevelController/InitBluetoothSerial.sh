#!/bin/bash
echo "***Bluetooth Serial Port***"
echo "Initializing..."
if [ $# == 0 ] 
then
    echo "E:Target device address argument missing!"
    echo "  (Ex: ./InitBluetoothSerial.sh <bdaddr> )"
    echo "List of paired devices"
    echo $(bluetoothctl paired-devices)
    exit 1
fi
if [ `expr length "$1"` != 17 ]
then 
    echo "E:Address format incorrect!"
    echo "  (Ex: 00:FA:BD:E6:7B:20 )"
    exit 1  
fi

echo "Target device: $1"
echo "Checking the paired devices..."
dev_name=$(bluetoothctl paired-devices | grep $1)

if [[ `expr length "$dev_name"` != 0 ]]
then
    echo "Found $dev_name"
else
    echo "E:Device $1 not found!"
    exit 1
fi

if [[ $(ls /dev | grep rfcomm0) ]]
then
    echo "E:Device \"rfcomm0\" already exists!"
    exit 1
fi

echo "Creating RFCOMM device..."
sudo modprobe rfcomm
sudo rfcomm bind rfcomm0 $1

if [[ $(ls /dev | grep rfcomm0) ]]
then
    sudo chmod +x /dev/rfcomm0
    echo "Device setup completed."
    echo "Info:$(ls -l /dev | grep rfcomm0)"
else
    echo "E:Device not created"
    exit 1
fi

#user=$(whoami)
#echo "Opening cutecom as $user"
#sudo -su $user cutecom

#echo "Releasing rfcomm0..."
#sudo rfcomm release rfcomm0

echo "Program exits successfully"
exit 0