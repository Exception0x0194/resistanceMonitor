#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>

sensor::sensor()
{
    adcHandler = 0;
    retv = 0;
    resistance = 9;
}

bool sensor::openADC()
{
    if ((adcHandler = open("/dev/adc", O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
    {
        printf("Error occured when opening ADC.\n");
        return false;
    }
    printf("ADC successfully connected.\n");
    return true;
}

bool sensor::closeADC()
{
    close(adcHandler);
    return true;
}

int sensor::getResistance()
{
    retv = read(adcHandler, buffer, 10);
    if (retv == -1 || retv == 0)
    {
        printf("Error occored when reading ADC.\n");
        return -1;
    }
    resistance = atoi(buffer);
    resistance = (int)(resistance * 10000) / 4096;
    return resistance;
}
