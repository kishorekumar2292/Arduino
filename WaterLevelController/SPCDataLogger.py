#!/usr/bin/python3

import serial
import time

btdevice=serial.Serial(
    port='/dev/rfcomm0',\
        baudrate=9600,\
            parity=serial.PARITY_NONE,\
                stopbits=serial.STOPBITS_ONE,\
                    bytesize=serial.EIGHTBITS,\
                        timeout=0)

print('Connected to: ' + btdevice.portstr)
btdevice.reset_input_buffer()
btdevice.reset_output_buffer()

def executeQuery(device, query):
    device.write(query.encode('utf-8'))
    while device.in_waiting == 0:
        time.sleep(1)
    else:
        response=str(device.read(device.in_waiting))
        return str(response[2:-3])

def executeCommand(device, command):
    device.write(command.encode('utf-8'))
    while device.in_waiting == 0:
        time.sleep(1)
    else:
        response=str(device.read(device.in_waiting))
        response=str(response[2:-3])
        return True if response == "OK" else False

def isLastLineSame(file, data):
    with open(file) as f:
        for line in (f.readlines() [-1:]):
            if(line==data):
                print("Last line is same")
                return True
            else:
                return False

def writeData(file, data):
    with open(file, 'a+') as f:
        f.write(data)

#Collects data for PumpSet1 only if PING succeeds
print("Collecting data for Pump Set1...")
if executeCommand(btdevice, "?ping"):
    pumpset1File='PumpSet1.csv'
    pump1=executeQuery(btdevice, "?pump1")
    pump1TS=executeQuery(btdevice, "?pump1TS")
    sump=executeQuery(btdevice, "?sump")
    tank1=executeQuery(btdevice, "?tank1")
    pumpset1Data=str(pump1 + ',' + pump1TS + ',' + sump + ',' + tank1 + '\n')
    print(pumpset1Data)
    if(isLastLineSame(pumpset1File,pumpset1Data)):
        print("Skipping...")
    else:
        print("Writing data")
        writeData(pumpset1File,pumpset1Data)
else:
    print('Ping failed!')

#Collects data for PumpSet2 only if PING succeeds
print("Collecting data for Pump Set2...")
if executeCommand(btdevice, "?ping"):
    pumpset2File='PumpSet2.csv'
    pump2=executeQuery(btdevice, "?pump2")
    pump2TS=executeQuery(btdevice, "?pump2TS")
    tank2=executeQuery(btdevice, "?tank2")
    pumpset2Data=str(pump2 + ',' + pump2TS + ',' + tank2 + '\n')
    print(pumpset2Data)
    if(isLastLineSame(pumpset2File,pumpset2Data)):
        print("Skipping...")
    else:
        print("Writing data")
        writeData(pumpset2File,pumpset2Data)
else:
    print('Ping failed!')
