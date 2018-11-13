
#include <SoftwareSerial9.h>

class Wheel
{
public:
    enum class Direction
    {
        FORWARD = 0,
        BACKWARD
    };
    Wheel(SoftwareSerial9* serial, uint32_t Baud, bool inv, int directionPin, int16_t speedLimit, double wheelDiameter = 0.17);
    ~Wheel()
    {
    }
    void SetRpm(double rpm);
    double GetRpm();
    void SetMps(double ms);
    double GetMps();
    void Stop();
    void RisingIsr();
    void FallingIsr();
    void SetSpeed(int16_t sp);

private:
    double CalculateRpm();
    int16_t speedLimit;
    int16_t currentSpeed;
    bool inverted;
    uint32_t Baudrate;
    SoftwareSerial9* mySerial;
    void SendSpeedOverUart(int16_t sp);
    int directionPin;

    volatile Direction direction;
    const double wheelRadius;
    volatile double currentRpm;
    volatile unsigned long timeRising;
    volatile unsigned long timeFalling;
    volatile unsigned long timeRisingTmp;
    volatile unsigned long timeFallingTmp;
    volatile unsigned long pulsewidth;
    const double Pi_30 = 3.10471975512;
};
