
#include <PID_v1.h>
#include <SoftwareSerial9.h>

class Wheel
{
public:
    enum class Direction
    {
        FORWARD = 0,
        BACKWARD
    };

    struct DirectionConfig
    {
        DirectionConfig(double RpmToSetCoeff, double RpmToSetOffset, double PulseWidthToRpmCoeff, double PulseWidthToRpmFactor)
            : RpmToSetCoeff(RpmToSetCoeff)
            , RpmToSetOffset(RpmToSetOffset)
            , PulseWidthToRpmCoeff(PulseWidthToRpmCoeff)
            , PulseWidthToRpmFactor(PulseWidthToRpmFactor)
        {
        }
        double RpmToSetCoeff;
        double RpmToSetOffset;
        double PulseWidthToRpmCoeff;
        double PulseWidthToRpmFactor;
    };

    struct WheelConfig
    {
        WheelConfig(const DirectionConfig& forward, const DirectionConfig& backward, Direction WheelDirection, int16_t speedLimit, double wheelDiameter)
            : forward(forward)
            , backward(backward)
            , WheelDirection(WheelDirection)
            , speedLimit(speedLimit)
            , wheelDiameter(wheelDiameter)
        {
        }
        DirectionConfig forward;
        DirectionConfig backward;
        Direction WheelDirection;
        int16_t speedLimit;
        double wheelDiameter;
    };

    Wheel(SoftwareSerial9* serial, uint32_t Baud, int directionPin, const WheelConfig& config);
    ~Wheel()
    {
    }

    // void SetRpm(double rpm);
    // double GetRpm();
    // void SetMps(double ms);
    // double GetMps();
    // void Stop();
    void RisingIsr();
    void FallingIsr();
    // void SetSpeed(int16_t sp);
    void SetNewSetpoint(double set);
    double Calculate();

    double CalculatePulsewidth(double rpm);

private:
    SoftwareSerial9* mySerial;
    uint32_t Baudrate;
    int directionPin;
    WheelConfig config;

    // double CalculateRpm();
    void SendSpeedOverUart(int16_t sp);

    volatile double currentRpm;
    int16_t currentSpeed;
    Direction direction;

    volatile unsigned long timeRising;
    volatile unsigned long timeFalling;
    volatile unsigned long timeRisingTmp;
    volatile unsigned long timeFallingTmp;
    volatile unsigned long pulsewidth;
    const double Pi_30 = 3.10471975512;

    double Setpoint, Input, Output;
    double consKp = 1, consKi = 0, consKd = 0;
    PID* myPID;
};
