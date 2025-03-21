#include "minikame.h"


void MiniKame::init() {
  // Map between servos and board pins
  board_pins[0] = 12; // Servo S0
  board_pins[1] = 16; // Servo S1
  board_pins[2] = 25; // Servo S2
  board_pins[3] = 18; // Servo S3
  board_pins[4] = 13; // Servo S4
  board_pins[5] = 17; // Servo S5
  board_pins[6] = 26; // Servo S6
  board_pins[7] = 19; // Servo S7

  // Trim values for zero position calibration.

    trim[0] = 0; // 前左大腿
    trim[1] = 0; // 前右大腿
    trim[2] = 0; // 前左小腿
    trim[3] = 0; // 前右小腿
    trim[4] = 0; // 后左大腿
    trim[5] = 0; // 后右大腿
    trim[6] = 0; // 后左小腿
    trim[7] = 0; // 后右小腿
    
  // Set reverse movement
  for (int i = 0; i < 8; i++) reverse[i] = false;

  // Init an oscillator for each servo
  for (int i = 0; i < 8; i++) {
    oscillator[i].start();
    servo[i].attach(board_pins[i]);
  }
  home();
}

void MiniKame::reverseServo(int id) {
  if (reverse[id])
    reverse[id] = 0;
  else
    reverse[id] = 1;
}


void MiniKame::setServo(int id, float target) {
  if (!reverse[id])
    servo[id].writeMicroseconds(angToUsec(target + trim[id]));
  else
    servo[id].writeMicroseconds(angToUsec(180 - (target + trim[id])));
  _servo_position[id] = target;
}

float MiniKame::getServo(int id) {
  return _servo_position[id];
}


void MiniKame::moveServos(int time, float target[8]) {
  if (time > 10) {
    for (int i = 0; i < 8; i++)	_increment[i] = (target[i] - _servo_position[i]) / (time / 10.0);
    _final_time =  millis() + time;

    while (millis() < _final_time) {
      _partial_time = millis() + 10;
      for (int i = 0; i < 8; i++) setServo(i, _servo_position[i] + _increment[i]);
      while (millis() < _partial_time); //pause
    }
  }
  else {
    for (int i = 0; i < 8; i++) setServo(i, target[i]);
  }
  for (int i = 0; i < 8; i++) _servo_position[i] = target[i];
}

void MiniKame::execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]) {

  for (int i = 0; i < 8; i++) {
    oscillator[i].setPeriod(period[i]);
    oscillator[i].setAmplitude(amplitude[i]);
    oscillator[i].setPhase(phase[i]);
    oscillator[i].setOffset(offset[i]);
  }

  unsigned long global_time = millis();

  for (int i = 0; i < 8; i++) oscillator[i].setTime(global_time);

  _final_time = millis() + period[0] * steps;
  while (millis() < _final_time) {
    for (int i = 0; i < 8; i++) {
      setServo(i, oscillator[i].refresh());
    }
    yield();
  }
}

int MiniKame::angToUsec(float value) {
  return value / 180 * (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) + MIN_PULSE_WIDTH;
}

// 定义动作
void MiniKame::home() {
  int ap = 2;
  int hi = 2;
  int position[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  for (int i = 0; i < 8; i++) setServo(i, position[i]);
}

void MiniKame::walk(float steps = 3, int t = 5000) {
  int x_amp = 15;
  int z_amp = 20;
  int ap = 20;
  int hi = 10;
  int front_x = 12;
  int period[] = {t, t, t / 2, t / 2, t, t, t / 2, t / 2};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {   90 + ap - front_x,
                     90 - ap + front_x,
                     90 - hi,
                     90 + hi,
                     90 - ap - front_x,
                     90 + ap + front_x,
                     90 + hi,
                     90 - hi
                 };
  int  phase[] = {90, 90, 270, 90, 270, 270, 90, 270};

  for (int i = 0; i < 8; i++) {
    oscillator[i].reset();
    oscillator[i].setPeriod(period[i]);
    oscillator[i].setAmplitude(amplitude[i]);
    oscillator[i].setPhase(phase[i]);
    oscillator[i].setOffset(offset[i]);
  }

  _final_time = millis() + period[0] * steps;
  _init_time = millis();
  bool side;
  while (millis() < _final_time) {
    side = (int)((millis() - _init_time) / (period[0] / 2)) % 2;
    setServo(0, oscillator[0].refresh());
    setServo(1, oscillator[1].refresh());
    setServo(4, oscillator[4].refresh());
    setServo(5, oscillator[5].refresh());

    if (side == 0) {
      setServo(3, oscillator[3].refresh());
      setServo(6, oscillator[6].refresh());
    }
    else {
      setServo(2, oscillator[2].refresh());
      setServo(7, oscillator[7].refresh());
    }
    delay(1);
  }
}


void MiniKame::forward(float steps=3, int t=800) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 10;
  int hi = 15;
  int front_x = 6;
  int period[8];
  for (int i = 0; i < 8; i++) {
    period[i] = t;
  }
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[8] = {
    90 + ap - front_x,
    90 - ap + front_x,
    90 - hi,
    90 + hi,
    90 - ap - front_x,
    90 + ap + front_x,
    90 + hi,
    90 - hi
  };
  int phase[8] = {0, 0, 90, 90, 180, 180, 90, 90};
  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::backward(float steps = 3, int t = 800) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 10;
  int hi = 15;
  int front_x = 6;
  int period[8] = {t, t, t, t, t, t, t, t};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[8] = {
    90 + ap - front_x,
    90 - ap + front_x,
    90 - hi,
    90 + hi,
    90 - ap - front_x,
    90 + ap + front_x,
    90 + hi,
    90 - hi
  };
  int phase[8] = {180, 180, 90, 90, 0, 0, 90, 90};
  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::turnR(float steps, int t = 600) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 23;
  int period[] = {t, t, t, t, t, t, t, t};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi };
  int phase[] = {0, 180, 90, 90, 180, 0, 90, 90};

  execute(steps, period, amplitude, offset, phase);
}

void MiniKame::turnL(float steps, int t = 600) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 23;
  int period[] = {t, t, t, t, t, t, t, t};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi };
  int phase[] = {180, 0, 90, 90, 0, 180, 90, 90};

  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::hello() {

  int a = 50;
  int b = 30;
  int c = 20;
  int d = 70;
  float state1[8] = {90 - a, 90, 90 + c, 90 - c,
                     90 + c, 90 - c, 90 - d, 90 + d};

  float state2[8] = {90 - a, 90 + b, 90 + c, 90 + d,
                     90 + c, 90 - c, 90 - d, 90 + d};

  float state3[8] = {90 - a, 90 - b, 90 + c, 90 + d,
                     90 + c, 90 - c, 90 - d, 90 + d};

  float state4[8] = {90, 90, 90, 90, 90, 90, 90, 90};

  moveServos(300, state1);

  for (int i = 0; i < 3; i++) {
    moveServos(200, state2);
    moveServos(200, state3);
  }

  delay(300);
  moveServos(200, state4);
}


void MiniKame::omniWalk(float steps=2, int t=1000, bool side=true, float turn_factor=2) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 23;
  int front_x = 6 * (1 - pow(turn_factor, 2));
  int period[] = {t, t, t, t, t, t, t, t};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {    90 + ap - front_x,
                      90 - ap + front_x,
                      90 - hi,
                      90 + hi,
                      90 - ap - front_x,
                      90 + ap + front_x,
                      90 + hi,
                      90 - hi
                 };

  int phase[8];
  if (side) {
    int phase1[] =  {0,   0,   90,  90,  180, 180, 90,  90};
    int phase2R[] = {0,   180, 90,  90,  180, 0,   90,  90};
    for (int i = 0; i < 8; i++)
      phase[i] = phase1[i] * (1 - turn_factor) + phase2R[i] * turn_factor;
  }
  else {
    int phase1[] =  {0,   0,   90,  90,  180, 180, 90,  90};
    int phase2L[] = {180, 0,   90,  90,  0,   180, 90,  90};
    for (int i = 0; i < 8; i++)
      phase[i] = phase1[i] * (1 - turn_factor) + phase2L[i] * turn_factor + oscillator[i].getPhaseProgress();
  }

  execute(steps, period, amplitude, offset, phase);
}

 
void MiniKame::moonwalkL(float steps=4, int t=2000) {

  int z_amp = 25;
  int hi = 25;
  int period[] = {t, t, t, t, t, t, t, t};
  int amplitude[] = {0, 0, z_amp, z_amp, 0, 0, z_amp, z_amp};
  int offset[] = {90, 90, 90 - hi, 90 + hi, 90, 90, 90 + hi, 90 - hi};
  int phase[] = {0, 0, 0, 80, 0, 0, 160, 290};

  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::dance(float steps=3, int t=2000) {
  int x_amp = 0;
  int z_amp = 30;
  int ap = 0;
  int hi = 20;
  int period[] = {t, t, t, t, t, t, t, t};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[] = {0, 0, 0, 270, 0, 0, 90, 180};

  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::upDown(float steps=2, int t=2000) {
  int x_amp = 0;
  int z_amp = 35;
  int ap = 10;
  int hi = 15;
  int front_x = 0;
  int period[8] = {t, t, t, t, t, t, t, t};
  int amplitude[8] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[8] = {
    90 + ap - front_x,
    90 - ap + front_x,
    90 - hi,
    90 + hi,
    90 - ap - front_x,
    90 + ap + front_x,
    90 + hi,
    90 - hi
  };
  int phase[8] = {0, 0, 90, 270, 180, 180, 270, 90};
  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::pushUp(float steps=2, int t=2000) {
  int z_amp = 40;
  int x_amp = 45;
  int hi = 0;
  int b = 35;
  int period[8];
  for (int i = 0; i < 8; i++) {
    period[i] = t;
  }
  int amplitude[8] = {0, 0, z_amp, z_amp, 0, 0, 0, 0};
  int offset[8] = {
    90,
    90,
    90 - hi,
    90 + hi,
    90 + x_amp,
    90 - x_amp,
    90 + b,
    90 - b
  };
  int phase[8] = {0, 0, 90, -90, 0, 0, 0, 0};
  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::frontBack(float steps=2, int t=1000) {
  int x_amp = 30;
  int z_amp = 25;
  int ap = 20;
  int hi = 30;
  int period[] = {t, t, t, t, t, t, t, t};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {90 + ap, 90 - ap, 90 - hi, 90 + hi, 90 - ap, 90 + ap, 90 + hi, 90 - hi};
  int phase[] = {0, 180, 270, 90, 0, 180, 90, 270};

  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::waveHand(float steps=3, int t=2000) {
  int period[8];
  for (int i = 0; i < 8; i++) {
    period[i] = t;
  }
  int amplitude[8] = {20, 0, 0, 30, 0, 0, 0, 0};
  int offset[8] = {
    90 - 50,
    90,
    90 + 20,
    90 + 60,
    90,
    90,
    90,
    90
  };
  int phase[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::hide(float steps=1.0, int t=2000) {
  int a = 60;
  int b = 70;
  int period[8] = {t, t, t, t, t, t, t, t};
  int amplitude[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int offset[8] = {
    90 - a,
    90 + a,
    90 + b,
    90 - b,
    90 + a,
    90 - a,
    90 - b,
    90 + b
  };
  int phase[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  execute(steps, period, amplitude, offset, phase);
}


void MiniKame::scared() {
  int ap = 10;
  int hi = 40;

  float sentado[8] = {90 - 15, 90 + 15, 90 - hi, 90 + hi, 90 - 20, 90 + 20, 90 + hi, 90 - hi};
  float salto[8] = {90 - ap, 90 + ap, 160, 20, 90 + ap * 3, 90 - ap * 3, 20, 160};

  moveServos(600, sentado);
  moveServos(1000, salto);
  delay(1000);
}

void MiniKame::run(float steps, int t = 5000) {
  int x_amp = 15;
  int z_amp = 15;
  int ap = 15;
  int hi = 15;
  int front_x = 6;
  int period[] = {t, t, t, t, t, t, t, t};
  int amplitude[] = {x_amp, x_amp, z_amp, z_amp, x_amp, x_amp, z_amp, z_amp};
  int offset[] = {    90 + ap - front_x,
                      90 - ap + front_x,
                      90 - hi,
                      90 + hi,
                      90 - ap - front_x,
                      90 + ap + front_x,
                      90 + hi,
                      90 - hi
                 };
  int phase[] = {0, 0, 90, 90, 180, 180, 90, 90};

  execute(steps, period, amplitude, offset, phase);
}

