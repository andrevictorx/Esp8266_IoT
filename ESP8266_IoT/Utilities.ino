
const char* getSensorValuesJSON(const char* clientID, char* sensor, float value, char* uom){

    const char* timeStamp = NTP.getTimeDateString().c_str();
    char buf[255];

    sprintf(buf,"{\"deviceID\": \"%s\", \"sensor\": \"%s\", \"value\": \"%.2f\", \"uom\": \"%s\"}",clientID, sensor, value, uom);
    return buf;
}

const char* getJSON(float temperature, float humidity, float luminosity){

    char buf[255];

    sprintf(buf,"{\"temperature\": \"%.2f\", \"humidity\": \"%.2f\", \"luminosity\": \"%.2f\"}",temperature, humidity, luminosity);
    return buf;
}
