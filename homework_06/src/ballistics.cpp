#include "ballistics.hpp"

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>

#include <cmath>
#include <bitset>

// Debugging exercise notes:
// this file intentionally contains four runtime defects.
// The defects are related to malformed input shape, invalid numeric values,
// unsafe time deltas, and empty logs. Exact locations are not marked on purpose.

const AmmunitionDef VOG17Def{.m = 0.35, .d = 0.07, .l = 0, .type = false};
const AmmunitionDef M67Def{.m = 0.6, .d = 0.10, .l = 0, .type = false};
const AmmunitionDef RKG3Def{.m = 1.2, .d = 0.10, .l = 0, .type = false};
const AmmunitionDef GLIDINGVOGDef{.m = 0.45, .d = 0.10, .l = 1, .type = true};
const AmmunitionDef GLIDINGRKGDef{.m = 1.4, .d = 0.10, .l = 1, .type = true};

const char* AMMUNITION_NAMES[] = {"VOG-17", "M67", "RKG-3", "GLIDING-VOG", "GLIDING-RKG"};
const uint8_t AMMUNITION_LIST_SIZE = 5;
const double G = 9.81;

inline void printDelimiter()
{
  cout << "===============================================" << '\n' << '\n';
};

uint8_t getAmmoMask(char* ammoName)
{
  for (size_t i = 0; i < AMMUNITION_LIST_SIZE; i++) {
    if (strcmp(AMMUNITION_NAMES[i], ammoName) == 0) {
      return (1 << i);
    }
  }

  return 0;
}

void printData(const Data& data)
{
  cout << "Data successfully parsed: \n";
  cout << "xd:" << data.xd << '\n';
  cout << "yd:" << data.yd << '\n';
  cout << "zd:" << data.zd << '\n';
  cout << "targetX:" << data.targetX << '\n';
  cout << "targetY:" << data.targetY << '\n';
  cout << "accelerationPath:" << data.accelerationPath << '\n';
  cout << "attackSpeed:" << data.attackSpeed << '\n';
  cout << "ammoName:" << data.ammoName << '\n';
  cout << "ammoMask:" << bitset<8>(data.ammoMask) << '\n';
  printDelimiter();
}

Data getData(const char* fileName)
{
  Data data;

  ifstream file(fileName);
  if (!file) {
    throw runtime_error("Could not open file: " + string(fileName));
  }

  string ammoNameStr;

  try {
    file >> data.xd >> data.yd >> data.zd >> data.targetX >> data.targetY >> data.attackSpeed >> data.accelerationPath >> data.ammoName;
  }
  catch (...) {
    throw runtime_error("Error");
  }

  data.ammoMask = getAmmoMask(data.ammoName);

  if (data.ammoMask == 0) {
    throw runtime_error("Not supported ammunition: " + string(data.ammoName));
  }

  cout << "File successfully read \n";

  printData(data);

  return data;
}

double calculateTimeOfFlight(const AmmunitionDef& aD, const Data& data)
{
  double m = aD.m;
  double m2 = pow(m, 2);
  double l = aD.l;
  double d = aD.d;
  double d2 = pow(d, 2);
  const double V0 = data.attackSpeed;

  double a = d * G * m - 2 * d2 * l * V0;
  double b = -3 * G * m2 + 3 * d * l * m * V0;
  double c = 6 * m2 * data.zd;

  double p = -pow(b, 2) / (3 * pow(a, 2));
  double q = 2 * pow(b, 3) / (27 * pow(a, 3)) + c / a;

  double phi = acos(3 * q / (2 * p) * sqrt(-3 / p));

  double t = 2 * sqrt(-p / 3) * cos((phi + 4 * M_PI) / 3) - b / (3 * a);

  return t;
}

AmmunitionDef getAmmoDef(uint8_t ammoMask)
{
  switch (ammoMask) {
    case AmmunitionMask::VOG17:
      return VOG17Def;
    case AmmunitionMask::M67:
      return M67Def;
    case AmmunitionMask::RKG3:
      return RKG3Def;
    case AmmunitionMask::GLIDINGRKG:
      return GLIDINGRKGDef;
    case AmmunitionMask::GLIDINGVOG:
      return GLIDINGVOGDef;
  }
  throw runtime_error("Not supported ammunition mask: " + bitset<8>(ammoMask).to_string());
}

double getHorizontalFlightDistance(const Data& data)
{
  AmmunitionDef aD = getAmmoDef(data.ammoMask);
  const double V0 = data.attackSpeed;
  const double d = aD.d;
  const double l = aD.l;
  const double m = aD.m;
  const double d2 = pow(d, 2);
  const double d3 = pow(d, 3);
  const double d4 = pow(d, 4);
  const double l2 = pow(l, 2);
  const double l3 = pow(l, 3);
  const double l4 = pow(l, 4);
  const double m2 = pow(m, 2);
  const double m3 = pow(m, 3);
  const double m4 = pow(m, 4);
  double t = calculateTimeOfFlight(aD, data);
  if (t <= 0 || isnan(t)) {
    string error = "Not able to calculate Time of Flight correctly for current input";
    throw runtime_error(error);
  }
  const double t2 = pow(t, 2);
  const double t3 = pow(t, 3);
  const double t4 = pow(t, 4);
  const double t5 = pow(t, 5);

  cout << "Time of Flight: " << t << endl;
  printDelimiter();

  double D =
    V0 * t - t2 * d * V0 / (2 * m) + t3 * (6 * d * G * l * m - 6 * d2 * (l2 - 1) * V0) / (36 * m2) +
    t4 * (-6 * d2 * G * l * (1 + l2 + l4) * m + 3 * d3 * l2 * (1 + l2) * V0 + 6 * d3 * l4 * (1 + l2) * V0) / (36 * pow((1 + l2), 2) * m3) +
    t5 * (3 * d3 * G * l3 * m - 3 * d4 * l2 * (1 + l2) * V0) / (36 * (1 + l2) * m4);

  if (D <= 0 || isnan(t)) {
    string error = "Not able to calculate Horizontal Flight Distance correctly for current input";
    throw runtime_error(error);
  }

  return D;
}

double getTargetDistance(const Data& data)
{
  return sqrt(pow(data.targetX - data.xd, 2) + pow(data.targetY - data.yd, 2));
}

Point getPoint(const Data& data, double h, double D)
{
  double targetX = data.targetX;
  double targetY = data.targetY;
  double xd = data.xd;
  double yd = data.yd;

  double ratio = (D - h) / D;
  Point point = {.x = xd + (targetX - xd) * ratio, .y = yd + (targetY - yd) * ratio};

  return point;
};

void xMoveFromTarget(Data& data, double h)
{
  data.xd = data.targetX - (h + data.accelerationPath);
}

void yMoveFromTarget(Data& data, double h)
{
  data.yd = data.targetY - (h + data.accelerationPath);
}

void diagonalMoveFromTarget(Data& data, double h)
{
  data.xd = data.targetX - (h + data.accelerationPath) / sqrt(2);
  data.yd = data.targetY - (h + data.accelerationPath) / sqrt(2);
}

void setIntermediatePoint(Data& data, double horDis, double D)
{
  if (D == 0) {
    xMoveFromTarget(data, horDis);
  }
  else {
    double c = (horDis + data.accelerationPath) / D;
    data.xd = data.targetX - (data.targetX - data.xd) * c;
    data.yd = data.targetY - (data.targetY - data.yd) * c;
  }

  cout << "Intermediate point required: x:" << data.xd << " y: " << data.yd << endl;
  printDelimiter();
}
