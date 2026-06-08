#include <optional>
#include <iostream>
#include "json.hpp"

#define ENABLE_LOG 1
#define ENABLE_DEBUG 0

#if ENABLE_LOG
#define LOG(msg) std::cout << "[LOG] " << msg << std::endl
#else
#define LOG(msg)
#endif

#if ENABLE_DEBUG
#define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
#define DEBUG(msg)
#endif

using json = nlohmann::json;

using namespace std;

const char* SIMULATION_FILE = "data/simulation.json";

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

struct SolverResult {
  Coord interpolatedTargetPos;
  Coord balisticPoint;
  float timeToPos;
};

struct AmmoParams {
  char name[32];
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
  char ammoName[32];    // обрані боєприпаси
  float arrayTimeStep;  // крок часу масиву цілей
  float simTimeStep;    // крок симуляції
  float hitRadius;      // радіус влучення
  float angularSpeed;   // кутова швидкість (рад/с)
  float turnThreshold;  // поріг повороту (рад)
};

enum DroneState { STOPPED, ACCELERATING, DECELERATING, TURNING, MOVING };

// const uint8_t TIME_STEP_SIZE = 60;
const uint16_t MAX_STEPS = 10000;
const double G = 9.81;
inline void printDelimiter()
{
  cout << "===============================================" << '\n' << '\n';
};

struct AmmunitionDef {
  double m;
  double d;
  double l;
  bool type;
};

class ITargetProvider {
public:
  virtual int getTargetCount() = 0;
  virtual int getTimeSteps() = 0;
  virtual Coord* getTarget(int index) = 0;
  virtual Coord getTargetCoord(int index, float) = 0;
  virtual ~ITargetProvider() {}
};

class IBallisticSolver {
public:
  virtual SolverResult solve(const DroneConfig& config,
                             const Coord& dronePos,
                             const Coord& targetPos,
                             const Coord& targetPrevPos,
                             AmmoParams ammo,
                             int timeStepSize) = 0;
  virtual ~IBallisticSolver() {}
};

class IConfigLoader {
public:
  virtual void load() = 0;
  virtual DroneConfig getConfig() = 0;
  virtual AmmoParams getAmmoParams() = 0;
};

enum class SolverType { ANALYTICAL };
enum class ProviderType { JSON };
enum class LoaderType { FILE };

IBallisticSolver* createSolver(SolverType type);
ITargetProvider* createProvider(ProviderType type, const char* param);
IConfigLoader* createLoader(LoaderType type);
