#ifndef __PHOTO_H__
#define __PHOTO_H__

#include <Arduino.h>
#include <Wire.h>

class Photo{
    
private:

    bool startSensor();
public:

    void begin();
    bool getSensor(float *l);
    bool getLum(float *l);
};

#endif