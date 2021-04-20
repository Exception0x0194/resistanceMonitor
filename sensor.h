#ifndef SENSOR_H
#define SENSOR_H

class sensor
{
public:
    sensor();

    bool openADC();
    bool closeADC();
    bool refreshADC();
    bool changeRefreshRate(int refreshRateMs);
    int getResistance();

private:
    int adcHandler;
    int resistance;
    int retv ;
    char buffer[128];
};

#endif // SENSOR_H
