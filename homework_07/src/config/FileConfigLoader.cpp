#include <cstring>
#include <stdexcept>
#include <fstream>
#include <vector>
#include "json.hpp"
#include "config/FileConfigLoader.hpp"

using json = nlohmann::json;
using namespace std;

void FileConfigLoader::load()
{
  ammoCount = loadAmmo();
  loadConfig();
}

DroneConfig FileConfigLoader::getConfig()
{
  return config;
}

AmmoParams FileConfigLoader::getAmmoParams()
{
  for (AmmoParams ammo : ammos) {
    if (ammo.name == config.ammoName) {
      return ammo;
    }
  }
  throw runtime_error("Ammo not found!");
}

uint16_t FileConfigLoader::loadAmmo()
{
  ifstream fin(AMMO_FILE);
  if (!fin) {
    throw runtime_error("Could not open file: " + string(AMMO_FILE));
  }
  json j;
  fin >> j;
  uint16_t count = j.size();
  ammos.resize(count);
  for (int i = 0; i < count; i++) {
    ammos.at(i).name = j[i]["name"].get<string>().c_str();
    ammos.at(i).mass = j[i]["mass"];
    ammos.at(i).drag = j[i]["drag"];
    ammos.at(i).lift = j[i]["lift"];
  }
  return count;
}

void FileConfigLoader::loadConfig()
{
  ifstream fin(CONFIG_FILE);
  if (!fin) {
    throw runtime_error("Could not open file: " + string(CONFIG_FILE));
  }
  json j;
  fin >> j;

  config.startPos.x = j["drone"]["position"]["x"];
  config.startPos.y = j["drone"]["position"]["y"];
  config.altitude = j["drone"]["altitude"];
  config.accelPath = j["drone"]["accelerationPath"];
  config.angularSpeed = j["drone"]["angularSpeed"];
  config.attackSpeed = j["drone"]["attackSpeed"];
  config.initialDir = j["drone"]["initialDirection"];
  config.turnThreshold = j["drone"]["turnThreshold"];

  config.simTimeStep = j["simulation"]["timeStep"];
  config.hitRadius = j["simulation"]["hitRadius"];
  config.arrayTimeStep = j["targetArrayTimeStep"];

  config.physicsTimeStep = j["simulation"]["physicsTimeStep"];
  config.targetTimeStep = j["simulation"]["targetTimeStep"];
  config.timeScale = j["simulation"]["timeScale"];

  config.ammoName = j["ammo"];
}