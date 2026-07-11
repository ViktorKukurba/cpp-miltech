#pragma once

#include <cstring>
#include <stdexcept>
#include <fstream>
#include "json.hpp"

#include "config/FileConfigLoader.hpp"

using json = nlohmann::json;

class FileConfigLoader : public IFileConfigLoader {
public:
  void load() override
  {
    ammoCount = loadAmmo();
    loadConfig();
  };
  DroneConfig getConfig() override { return config; };
  AmmoParams getAmmoParams() override
  {
    for (AmmoParams ammo : ammos) {
      if (ammo.name == config.ammoName) {
        return ammo;
      }
    }
    throw runtime_error("Ammo not found!");
  };

private:
  const string CONFIG_FILE = "homework_07/data/config.json";
  const string AMMO_FILE = "homework_07/data/ammo.json";
  vector<AmmoParams> ammos;
  DroneConfig config;
  u_int16_t ammoCount;

  u_int16_t loadAmmo()
  {
    ifstream fin(AMMO_FILE);
    if (!fin) {
      throw runtime_error("Could not open file: " + string(AMMO_FILE));
    }
    json j;
    fin >> j;
    u_int16_t count = j.size();
    ammos.resize(count);
    for (int i = 0; i < count; i++) {
      ammos.at(i).name = j[i]["name"].get<string>().c_str();
      ammos.at(i).mass = j[i]["mass"];
      ammos.at(i).drag = j[i]["drag"];
      ammos.at(i).lift = j[i]["lift"];
    }
    return count;
  }

  void loadConfig()
  {
    ifstream fin(CONFIG_FILE);
    if (!fin) {
      throw runtime_error("Could not open file: " + string(CONFIG_FILE));
    }
    json j;
    fin >> j;

    // Доступ до полів
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

    config.ammoName = j["ammo"];
  }
};