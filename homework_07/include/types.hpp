#pragma once

#include <cmath>
#include <optional>
#include <string>

using namespace std;

struct Coord {
  float x;
  float y;
  optional<double> z;

  // Додавання координат
  Coord operator+(const Coord& other) const
  {
    Coord result;
    result.x = x + other.x;
    result.y = y + other.y;
    return result;
  }

  // Віднімання координат
  Coord operator-(const Coord& other) const
  {
    Coord result;
    result.x = x - other.x;
    result.y = y - other.y;
    return result;
  }

  // Множення на скаляр
  Coord operator*(float s) const
  {
    Coord result;
    result.x = x * s;
    result.y = y * s;
    return result;
  }

  static float length(const Coord& a) { return sqrt(pow(a.x, 2) + pow(a.y, 2)); }

  Coord move(float theta, float time, float speed) { return Coord{x + speed * cos(theta) * time, y + speed * sin(theta) * time}; }

  static float getDistance(const Coord& a, const Coord& b)
  {
    Coord delta = a - b;
    float dist = length(delta);
    return dist;
  }
};

struct AmmoParams {
  string name;
  float mass;  // маса (кг)
  float drag;  // коефіцієнт опору
  float lift;  // коефіцієнт підйому
};

struct SimStep {
  Coord pos;              // позиція дрона
  float direction;        // напрямок (рад)
  int state;              // стан автомата (0-4)
  int targetIdx;          // індекс поточної цілі
  Coord dropPoint;        // точка скиду (куди летить дрон)
  Coord aimPoint;         // куди впаде бомба (якщо скинути зараз)
  Coord predictedTarget;  // прогнозована позиція цілі
};

struct DroneConfig {
  Coord startPos;       // початкова позиція (x, y)
  float altitude;       // висота
  float initialDir;     // початковий напрямок (рад)
  float attackSpeed;    // швидкість атаки (м/с)
  float accelPath;      // шлях розгону (м)
  string ammoName;      // обрані боєприпаси
  float arrayTimeStep;  // крок часу масиву цілей
  float simTimeStep;    // крок симуляції
  float hitRadius;      // радіус влучення
  float angularSpeed;   // кутова швидкість (рад/с)
  float turnThreshold;  // поріг повороту (рад)
};

enum DroneState { STOPPED, ACCELERATING, DECELERATING, TURNING, MOVING };

struct DroneContext {
  float desiredDir;
  float direction;
  float targetDir;
  float turnRemaining;
  float speed;
  DroneConfig cfg;
};
