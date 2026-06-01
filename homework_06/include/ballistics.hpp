#pragma once

#include <cstdint>

using namespace std;

enum AmmunitionMask {
  VOG17 = 0b00000001,
  M67 = 0b00000010,
  RKG3 = 0b00000100,
  GLIDINGVOG = 0b00001000,
  GLIDINGRKG = 0b00010000,
};

struct Data {
  float xd;
  float yd;
  float zd;
  float targetX;
  float targetY;
  float attackSpeed;
  float accelerationPath;
  char ammoName[32];
  uint8_t ammoMask;
};

struct AmmunitionDef {
  double m;
  double d;
  double l;
  bool type;
};

struct Point {
  double x, y;
};

Data getData(const char* fileName);
double getHorizontalFlightDistance(const Data& data);
double getTargetDistance(const Data& data);
void setIntermediatePoint(Data& data, double horDis, double D);
double getTargetDistance(const Data& data);
Point getPoint(const Data& data, double h, double D);
