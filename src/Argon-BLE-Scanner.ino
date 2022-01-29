/*
 * Project Argon-BLE-Scanner
 * Description: A simple script to scan for Ruuvi tags and get their addresses
 * Author: Dirk van der Laarse
 * Date: 2022-01-21
 */

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler;

const size_t SCAN_RESULT_MAX = 20;

BleScanResult scanResults[SCAN_RESULT_MAX];
BlePeerDevice peer;
uint16_t lastRate = 0;
char publishString[45];
char address[45];


void setup() {
  Log.info("Setup...");
}


void loop() {
    if (BLE.connected()) {
        // We're currently connected to a sensor
        Log.info("Currently connected to a sensor, doing nothing");
    }
    else {
        Log.info("Scanning...");

        // We are not connected to a sensor, scan for one
        int count = BLE.scan(scanResults, SCAN_RESULT_MAX);

        for (int ii = 0; ii < count; ii++) {
            uint8_t buf[BLE_MAX_ADV_DATA_LEN];
            size_t dataLength;
            
            // dataLength = scanResults[ii].advertisingData.get(BleAdvertisingDataType::MANUFACTURER_SPECIFIC_DATA, buf, BLE_MAX_ADV_DATA_LEN);
            dataLength = scanResults[ii].advertisingData().customData(buf, BLE_MAX_ADV_DATA_LEN);
            if(buf[0]==0x99 && buf[1]==0x04){
                if(buf[2]==0x3){
                    Log.info("Ruuvi tag is of type v03");
                    // RuuviTag is type v03 (https://github.com/ruuvi/ruuvi-sensor-protocols/blob/master/dataformat_03.md)
                    
                    // publish address
                    sprintf(address, "rssi=%d address=%02X:%02X:%02X:%02X:%02X:%02X (%d)",
                            scanResults[ii].rssi(),
                            scanResults[ii].address()[0], scanResults[ii].address()[1], scanResults[ii].address()[2],
                            scanResults[ii].address()[3], scanResults[ii].address()[4], scanResults[ii].address()[5], scanResults[ii].address()[0]);

                    Log.info("Found sensor #%s: %s", (const char*)(ii), (const char*)(address));
            
                    // process manufacturer data
                    sprintf(publishString, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", 
                                                        buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14]);
                    Log.info("Manuf data for sensor %s: %s", (const char*)(ii), (const char*)(publishString));
                    
                    // process reading
                    float humidity = int(buf[3]) * 0.5;
                    int temperature_base = int(buf[4]);
                    int temperature_fraction = int(buf[5]);
                    float temperature = float(temperature_base) + (float(temperature_fraction) / 100);
                    
                    char readings[40];
                    sprintf(readings, "Humidity = %.2f%% | Temperature = %.2f'C", humidity, temperature);
                    Log.info("Readings for sensor %s: %s", (const char*)(ii), (const char*)(readings));
            
                }
                if(buf[2]==0x5 && dataLength>19){
                    Log.info("Ruuvi tag is of type v05");
                    // RuuviTag is type v05 (https://github.com/ruuvi/ruuvi-sensor-protocols/blob/master/dataformat_05.md)
                    
                    // publish address
                    sprintf(address, "rssi=%d address=%02X:%02X:%02X:%02X:%02X:%02X (%d)",
                            scanResults[ii].rssi(),
                            scanResults[ii].address()[0], scanResults[ii].address()[1], scanResults[ii].address()[2],
                            scanResults[ii].address()[3], scanResults[ii].address()[4], scanResults[ii].address()[5], scanResults[ii].address()[0]);

                    Log.info("Found sensor %s: %s", (const char*)(ii), (const char*)(address));

                    // process reading
                    // Combine 2 bytes for the temperature as signed int in 0.005 degrees
                    signed long temperature_long = 0;
                    temperature_long = (buf[3]<<8) | (buf[4]);
                    float temperature_float = 0.0;
                    temperature_float = temperature_long * 0.005;

                    // Combine 2 bytes for the humidity as unsigned int in 0.0025%
                    unsigned int humidity_int;
                    humidity_int = (buf[5]<<8) | (buf[6]);
                    float humidity_float = 0.0;
                    humidity_float = humidity_int * 0.0025;

                    // Combine 2 bytes for the pressure as unsigned int in Pa units, with offset of -50 000 Pa
                    unsigned int pressure_int;
                    pressure_int = (buf[7]<<8) | (buf[8]);
                    float pressure_float = 0.0;
                    pressure_float = pressure_int + 50000;

                    char readings[70];
                    sprintf(readings, "Humidity = %.2f%% | Temperature = %.2f'C | Pressure = %.2f Pa", humidity_float, temperature_float, pressure_float);
                    Log.info("Readings for sensor %i: %s", ii, (const char*)(readings));
                }
            }

                
        }
    }
    delay(2500);

}