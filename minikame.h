#ifndef minikame_h
#define minikame_h
#define MIN_PULSE_WIDTH 544 // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH 2400 // the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH 1500
#include <ESP32Servo.h>
#include <Octosnake.h>

class MiniKame{
public:
    void init();
    void setServo(int id, float target);
    void reverseServo(int id);
    float getServo(int id);
    void moveServos(int time, float target[8]);

    // 定义动作
    // 1. 方向
    void forward(float steps, int period);
    void turnL(float steps, int period);
    void home();
    void turnR(float steps, int period);
    void backward(float steps, int period);

    // 2. 手势
    void hello();
    void omniWalk(float steps, int T, bool side, float turn_factor);
    void moonwalkL(float steps, int period);
    void dance(float steps, int period);
    void upDown(float steps, int period);
    void pushUp(float steps, int period);
    void frontBack(float steps, int period);
    void waveHand(float steps, int period);
    void hide(float steps, int t);
    void scared();

    void run(float steps, int t);
    void walk(float steps, int t);

private:
    Oscillator oscillator[8];
    Servo servo[8];
    int board_pins[8];
    int trim[8];
    bool reverse[8];
    unsigned long _init_time;
    unsigned long _final_time;
    unsigned long _partial_time;
    float _increment[8];
    float _servo_position[8];

    int angToUsec(float value);
    void execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]);
};

#endif
