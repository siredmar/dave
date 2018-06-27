
#include <SoftwareSerial9.h>

class Wheel
{
public:
    enum
    {
        FORWARD = 0,
        BACKWARD
    };
    Wheel(SoftwareSerial9 *serial, uint32_t Baud, bool inv, int directionPin);
    ~Wheel();
    void SetSpeed(int16_t sp);
    void Stop();
    void IncreaseSpeed(int delta);
    void DecreaseSpeed(int delta);
    int GetRpm();
    void RisingIsr();
    void FallingIsr();
    int CalculateRpm();
private:
    int direction;
    int16_t currentSpeed;
    bool inverted;
    uint32_t Baudrate;
    SoftwareSerial9 *mySerial;
    void SendSpeedOverUart(int16_t sp);
    int directionPin;
    int pulsewidth;
    uint32_t timeOld;
    const double wheelDiameter = 25.0;
    int currentRpm;
};
