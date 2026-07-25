#pragma once

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>
#include "interfaces/IConfigLoader.hpp"
#include "types.hpp"

class IFileConfigLoader : public IConfigLoader {};

class FileConfigLoader : public IFileConfigLoader {
public:
  void load() override;
  DroneConfig getConfig() override;
  AmmoParams getAmmoParams() override;

private:
  const std::string CONFIG_FILE = "homework_07/data/config.json";
  const std::string AMMO_FILE = "homework_07/data/ammo.json";
  std::vector<AmmoParams> ammos;
  DroneConfig config;
  uint16_t ammoCount{0};

  uint16_t loadAmmo();
  void loadConfig();
};