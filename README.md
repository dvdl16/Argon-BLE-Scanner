# Argon-BLE-Scanner

A simple script to scan for Ruuvi temperature sensors and print the MAC address and readings to Serial.

Tested on an Argon.


### Example usage

Open up a Serial Monitor, this is the expected result:

```bash
0000209015 [app] INFO: Scanning...
0000213968 [app] INFO: Ruuvi tag is of type v05
0000213968 [app] INFO: Found sensor : rssi=-43 address=CC:15:5A:B8:A2:CD (204)
0000213969 [app] INFO: Readings for sensor 1: Humidity = 52.26% | Temperature = 25.38'C | Pressure = 87086.00 Pa
0000216470 [app] INFO: Scanning...
```