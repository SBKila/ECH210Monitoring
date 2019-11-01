# ECH210Monitoring
L'objectif de ce projet est de remonter sur ThingSpeak les valeurs des sondes de température d'un ECH210BDT ainsi que le % d'humidité ambiant a partir d'un ESP8266.

Aim of the project is to broadcast on Thingspeak ECH210BD temperature probes and humidity level using a ESP8266.

# Releases

## 0.1
Première version fonctionnelle. Les reglages sont codés dans le code source.
2 configurations doivent etre défini,
  La connection WIFI
  La connection ThingSpeak
Ces 2 configuration se trouve dans le fichier secrets.h

first full fonctional release. Setting are hardcoded
2 settings must be defined
  The WIFI connection
  The Thingspeak account

# Used library
## NtpClientLib by German Martin version 2.5.1
## DHT sensor library for ESPx by beegee_tokyo version 1.17.0
## ThingSpeak by MathWorks Version 1.5.0
## ESP8266 by ESP8266 community version 2.5.2

# Hardware
The present source code was validated on a NodeMcu v3 board and a DHT022 sensor.

# Installation
## ESP8266
Update secrets.h with your values,

* **ech210BAddress** is the address of your ECH module modbus configuration
* **ssid** is your WIFI network name
* **password** is the password to be used to connect your WIFI
* **myChannelNumber** is the thinkspeak Channel ID
* **myWriteAPIKey** is the thingspeak "Write API Key" of your channel

## Thingspeak
Create an acount on https://thingspeak.com/users/sign_up

Setup a channel with following 8 fields
*    Field 1 RSSI
*    Field 2 Humidity
*    Field 3 Temperature
*    Field 4 SD1
*    Field 5 SD2
*    Field 6 SD3
*    Field 7 SD4
*    Field 8 D0

Setup 24h graph

```
% Channel ID to read data from 
readChannelID = XXXXXXXX; 
SD3FieldID = 6; 
SD4FieldID = 7; 
TemperatureFieldID = 3; 
SD1FieldID = 4; 
SD2FieldID = 5; 
HumidityFieldID = 2;

% Read Data. 
[data, timeStamps] = thingSpeakRead(readChannelID, 'Fields',[SD1FieldID, SD2FieldID, SD3FieldID, SD4FieldID,TemperatureFieldID,HumidityFieldID], ...
                                                           'NumDays', 1);
% Format data.
temperatureData = data(:, 5)/10;
humidityData = data(:, 6)/10;
SD4Data = data(:, 4)/10;
SD3Data = data(:, 3)/10;
SD2Data = data(:, 2)/10;
SD1Data = data(:, 1)/10;

% Visualize Data
yyaxis left
plot(timeStamps, temperatureData,'-k',timeStamps, SD3Data,'-g',timeStamps, SD4Data,'-c',timeStamps, SD1Data, '-b',timeStamps, SD2Data, '-r');
ylabel('T °c');

yyaxis right
plot(timeStamps, humidityData,':r');
ylabel('%');

legend({'Text'; 'SD3'; 'SD4'; 'SD1'; 'SD2';'Hext'},'Location','southwest','NumColumns',1);
```
