
#include <SoftwareSerial9.h>

class Wheel
{
public:
    enum class Direction
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
    long GetRpm();
    void RisingIsr();
    void FallingIsr();
    long CalculateRpm();
private:
    int16_t currentSpeed;
    bool inverted;
    uint32_t Baudrate;
    SoftwareSerial9 *mySerial;
    void SendSpeedOverUart(int16_t sp);
    int directionPin;
    
    volatile Direction direction;
    const double wheelDiameter = 25.0;
    volatile long currentRpm;
    volatile unsigned long currentTime;

    volatile unsigned long pulsewidth;
    volatile unsigned long timeOld;
};
