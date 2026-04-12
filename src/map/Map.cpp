#include "Map.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <utility>
#include <vector>

using std::vector, std::string, std::ifstream, std::stringstream, std::getline,
    std::stack;

Map::Map(const Map &other) {
  continents = new vector<Continent *>();
  for (auto continent : *other.continents) {
    continents->push_back(new Continent{*continent});
  }

  territories = new vector<Territory *>();
  for (auto territory : *other.territories) {
    territories->push_back(new Territory(*territory));
  }

  borders = new vector<vector<bool *> *>();
  for (int i = 0; i < other.borders->size(); i++) {
    borders->push_back(new vector<bool *>());
    for (int j = 0; j < other.borders->size(); j++) {
      (*borders)[i]->push_back(new bool(*(*(*other.borders)[i])[j]));
    }
  }

  validateErr = new Err(*other.validateErr);
}

Map::~Map() {
  for (auto continent : *continents) {
    delete continent;
  }
  delete continents;

  for (auto territory : *territories) {
    delete territory;
  }
  delete territories;

  for (auto vc : *borders) {
    for (auto item : *vc) {
      delete item;
    }
    delete vc;
  }
  delete borders;

  delete validateErr;
}

void Map::swap(Map &other) {
  using std::swap;
  swap(this->continents, other.continents);
  swap(this->territories, other.territories);
  swap(this->borders, other.borders);
  swap(this->validateErr, other.validateErr);
}

Map &Map::operator=(Map other) {
  this->swap(other);
  return *this;
}

std::ostream &operator<<(std::ostream &os, const Map &m) {
  os << "Map Details: { \n";

  os << "continents: [\n";
  for (int i = 0; i < m.continents->size(); i++) {
    os << *(*m.continents)[i];
  }
  os << "]\n";

  os << ", territories: [\n";
  for (int i = 0; i < m.territories->size(); i++) {
    os << *(*m.territories)[i];
  }
  os << "]\n";

  os << ", borders: [\n";
  for (int i = 0; i < m.borders->size(); i++) {
    os << i << ": [";
    for (int j = 0; j < (*m.borders)[i]->size(); j++) {
      if (j > 0)
        os << ", ";
      os << j << ": " << *(*(*m.borders)[i])[j];
    }
    os << "]\n";
  }
  os << "]\n";

  os << ", validate error: ";
  string msg;
  switch (*m.validateErr) {
  case Map::Err::MapNotConnected:
    msg = "map not connected";
    break;
  case Map::Err::ContinentNotConnected:
    msg = "continent not connected";
    break;
  case Map::Err::TerritoryContinentInvalid:
    msg = "territory continent invalid";
    break;
  case Map::Err::InconsistentBorders:
    msg = "inconsistent borders";
    break;
  case Map::Err::ZeroTerritories:
    msg = "zero territories";
    break;
  case Map::Err::ContinentZeroTerritories:
    msg = "continent zero territories";
    break;
  case Map::Err::None:
    msg = "none";
    break;
  }
  os << msg << "\n";

  os << " }\n";
  return os;
}

Continent::Continent(const Continent &other) {
  id = new int(*other.id);
  name = new string(*other.name);
  bonusValue = new int(*other.bonusValue);

  territoriesIds = new vector<int *>();
  for (auto territoryId : *other.territoriesIds) {
    territoriesIds->push_back(new int(*territoryId));
  }
}

Continent::~Continent() {
  delete id;
  delete name;
  delete bonusValue;

  for (auto territoryId : *territoriesIds) {
    delete territoryId;
  }
  delete territoriesIds;
}

void Continent::swap(Continent &other) {
  using std::swap;
  swap(this->id, other.id);
  swap(this->name, other.name);
  swap(this->bonusValue, other.bonusValue);
  swap(this->territoriesIds, other.territoriesIds);
}

Continent &Continent::operator=(Continent other) {
  this->swap(other);
  return *this;
}

std::ostream &operator<<(std::ostream &os, const Continent &c) {
  os << "Continent Details: { ";

  os << "id: " << *c.id << ", name: " << *c.name
     << ", bonus valus: " << *c.bonusValue << ", territories ids: [";

  for (int i = 0; i < c.territoriesIds->size(); i++) {
    if (i > 0)
      os << ", ";
    os << *(*c.territoriesIds)[i];
  }

  os << "] }\n";
  return os;
}

Territory::Territory(int id, string name, int continentId) {
  this->id = new int(id);
  this->name = new string(name);
  this->continentId = new int(continentId);
  this->neighborsIds = new vector<int *>();
  this->playerId = new int(-1);
  this->armiesNum = new int(0);
}

Territory::Territory(const Territory &other) {
  id = new int(*other.id);
  name = new string(*other.name);
  continentId = new int(*other.continentId);
  neighborsIds = new vector<int *>();
  for (auto neighborId : *other.neighborsIds) {
    neighborsIds->push_back(new int(*neighborId));
  }
  playerId = new int(*other.playerId);
  armiesNum = new int(*other.armiesNum);
}

Territory::~Territory() {
  delete id;
  delete name;
  delete continentId;

  for (auto neighborId : *neighborsIds) {
    delete neighborId;
  }
  delete neighborsIds;

  delete playerId;
  delete armiesNum;
}

void Territory::swap(Territory &other) {
  using std::swap;
  swap(this->id, other.id);
  swap(this->name, other.name);
  swap(this->continentId, other.continentId);
  swap(this->neighborsIds, other.neighborsIds);
  swap(this->playerId, other.playerId);
  swap(this->armiesNum, other.armiesNum);
}

Territory &Territory::operator=(Territory territory) {
  this->swap(territory);
  return *this;
}

string *Territory::getName() const { return name; }

vector<int *> *Territory::getNeighborsIds() const { return neighborsIds; }

int Territory::getArmiesNum() const { return *armiesNum; }

void Territory::setArmiesNum(const int armiesNum) {
  *this->armiesNum = armiesNum;
}

void Territory::setPlayerId(const int id) { *this->playerId = id; }

int Territory::getPlayerId() const { return *playerId; }

std::ostream &operator<<(std::ostream &os, const Territory &t) {
  os << "Territory Details: { ";

  os << "id: " << *t.id << ", name: " << *t.name
     << ", continent id: " << *t.continentId << ", player id: " << *t.playerId
     << ", armies number: " << *t.armiesNum << ", neighbors ids: [";

  for (int i = 0; i < t.neighborsIds->size(); i++) {
    if (i > 0)
      os << ", ";
    os << *(*t.neighborsIds)[i];
  }

  os << "] }\n";
  return os;
}

MapLoader::MapLoader(string filename) {
  this->filename = new string(filename);
  map = nullptr;

  ifstream file(filename);
  if (!file.is_open()) {
    isFormatValid = new bool(false);
    return;
  }

  string line;
  enum class Section { Invalid, Continents, Territories, Borders };

  Section section = Section::Invalid;
  bool isFormatValid = true;
  int territoriesCount = 0;

  // checking file format
  while (getline(file, line)) {
    // skip empty lines or comment lines starting with ';'
    if (line.empty() || line[0] == ';')
      continue;

    // changing sections
    if (line[0] == '[') {
      if (line == "[continents]")
        section = Section::Continents;
      else if (line == "[countries]") {
        if (section != Section::Continents) {
          isFormatValid = false;
          break;
        }
        section = Section::Territories;
      } else if (line == "[borders]") {
        if (section != Section::Territories) {
          isFormatValid = false;
          break;
        }
        section = Section::Borders;
      } else if (section != Section::Invalid) {
        isFormatValid = false;
        break;
      }
      continue;
    }

    // skip invalid section
    if (section == Section::Invalid)
      continue;

    // processing based on sections
    stringstream ss(line);

    if (section == Section::Continents) {
      string name;
      int bonusValue;
      ss >> name >> bonusValue;
    } else if (section == Section::Territories) {
      territoriesCount++;
      int id = -1, continentId;
      string name;
      ss >> id >> name >> continentId;
      if (id != territoriesCount) {
        isFormatValid = false;
        break;
      }
    } else if (section == Section::Borders) {
      int territoryId = -1, neighborId;
      ss >> territoryId;
      while (!ss.eof() && ss >> std::ws && !ss.eof() && ss >> neighborId)
        ;
      if (!ss.eof() || territoryId <= 0 || territoryId > territoriesCount) {
        isFormatValid = false;
        break;
      }
    }

    if (ss.fail()) {
      isFormatValid = false;
      break;
    }
  }

  if (!isFormatValid) {
    this->isFormatValid = new bool(false);
    return;
  }

  // format valid
  this->isFormatValid = new bool(true);

  file.clear();
  file.seekg(0, std::ios::beg);
  section = Section::Invalid;

  map = new Map();
  map->continents = new vector<Continent *>();
  map->territories = new vector<Territory *>();

  map->borders = new vector<vector<bool *> *>();
  for (int i = 0; i < territoriesCount; i++) {
    map->borders->push_back(new vector<bool *>());

    for (int j = 0; j < territoriesCount; j++) {
      (*map->borders)[i]->push_back(new bool(false));
    }
  }

  while (getline(file, line)) {
    // skip empty lines or comment lines starting with ';'
    if (line.empty() || line[0] == ';')
      continue;

    // changing sections
    if (line[0] == '[') {
      if (line == "[continents]") {
        section = Section::Continents;
      } else if (line == "[countries]") {
        section = Section::Territories;
      } else if (line == "[borders]") {
        section = Section::Borders;
      }
      continue;
    }

    // skip if section is invalid
    if (section == Section::Invalid) {
      continue;
    }

    // doing based on sections
    stringstream ss(line);
    if (section == Section::Continents) {
      int id, bonusValue;
      string name;

      id = (int)map->continents->size();
      ss >> name >> bonusValue;

      Continent *continent = new Continent();
      continent->id = new int(id);
      continent->name = new string(name);
      continent->bonusValue = new int(bonusValue);
      continent->territoriesIds = new vector<int *>();

      map->continents->push_back(continent);
    } else if (section == Section::Territories) {
      int id, continentId;
      string name;

      ss >> id >> name >> continentId;

      Territory *territory = new Territory();
      territory->id = new int(id - 1);
      territory->name = new string(name);
      territory->continentId = new int(continentId - 1);
      territory->neighborsIds = new vector<int *>();
      territory->playerId = new int(-1);
      territory->armiesNum = new int(0);

      map->territories->push_back(territory);
    } else if (section == Section::Borders) {
      int territoryId;
      int neighborId;

      ss >> territoryId;

      while (ss >> neighborId) {
        (*map->territories)[territoryId - 1]->neighborsIds->push_back(
            new int(neighborId - 1));
      }
    }
  }

  map->validate();
}

void Map::validate() {
  int territoriesNum = (int)territories->size();
  int continentsNum = (int)continents->size();

  // checking not zero territories
  if (!territoriesNum) {
    validateErr = new Err(Err::ZeroTerritories);
    return;
  }

  // processing and checking borders

  for (auto territory : *territories) {
    for (auto neighborId : *territory->neighborsIds) {
      if (*neighborId >= 0 && *neighborId < territoriesNum) {
        int territoryId = *territory->id;
        delete (*(*borders)[territoryId])[*neighborId];
        (*(*borders)[territoryId])[*neighborId] = new bool(true);
      } else {
        validateErr = new Err(Err::InconsistentBorders);
        return;
      }
    }
  }

  for (int i = 0; i < territoriesNum; i++) {
    if (*(*(*borders)[i])[i]) {
      validateErr = new Err(Err::InconsistentBorders);
      return;
    }

    for (int j = 0; j < territoriesNum; j++) {
      if (*(*(*borders)[i])[j] != *(*(*borders)[j])[i]) {
        validateErr = new Err(Err::InconsistentBorders);
        return;
      }
    }
  }

  // checking territories continents ids
  for (int i = 0; i < territoriesNum; i++) {
    int continentId = *(*territories)[i]->continentId;
    if (continentId >= 0 && continentId < continentsNum) {
      (*continents)[continentId]->territoriesIds->push_back(new int(i));
    } else {
      validateErr = new Err(Err::TerritoryContinentInvalid);
      return;
    }
  }

  // checking continent not having 0 territories
  for (int i = 0; i < continentsNum; i++) {
    if (!(*continents)[i]->territoriesIds->size()) {
      validateErr = new Err(Err::ContinentZeroTerritories);
      return;
    }
  }

  bool *visited = new bool[territoriesNum];
  stack<int> s;

  // checking map connectivity
  std::fill(visited, visited + territoriesNum, false);
  s.push(0);

  while (!s.empty()) {
    int u = s.top();
    s.pop();
    visited[u] = true;
    for (auto v : *(*territories)[u]->neighborsIds) {
      if (!visited[*v])
        s.push(*v);
    }
  }

  for (int i = 0; i < territoriesNum; i++) {
    if (!visited[i]) {
      validateErr = new Err(Err::MapNotConnected);
      delete[] visited;
      return;
    }
  }

  // checking continent connectivity
  std::fill(visited, visited + territoriesNum, false);

  for (int i = 0; i < continentsNum; i++) {
    s.push(*(*continents)[i]->territoriesIds->back());

    while (!s.empty()) {
      int u = s.top();
      s.pop();
      visited[u] = true;
      for (auto v : *(*territories)[u]->neighborsIds) {
        if (*(*territories)[*v]->continentId == i && !visited[*v])
          s.push(*v);
      }
    }

    for (auto territoryId : *(*continents)[i]->territoriesIds) {
      if (!visited[*territoryId]) {
        validateErr = new Err(Err::ContinentNotConnected);
        delete[] visited;
        return;
      }
    }
  }

  validateErr = new Err(Err::None);
  delete[] visited;
}

MapLoader::MapLoader(const MapLoader &other) {
  filename = new string(*other.filename);
  map = other.map == nullptr ? nullptr : new Map(*other.map);
  isFormatValid = new bool(*other.isFormatValid);
}

void MapLoader::swap(MapLoader &other) {
  using std::swap;
  swap(this->filename, other.filename);
  swap(this->map, other.map);
  swap(this->isFormatValid, other.isFormatValid);
}

MapLoader &MapLoader::operator=(MapLoader mapLoader) {
  this->swap(mapLoader);
  return *this;
}

MapLoader::~MapLoader() {
  delete filename;
  if (map != nullptr)
    delete map;
  delete isFormatValid;
}

std::ostream &operator<<(std::ostream &os, const MapLoader &m) {
  os << "Map Loader Details: { ";

  os << "filename: " << *m.filename << ", isFormatValid: " << *m.isFormatValid
     << ", map: ";

  if (m.map == nullptr)
    os << "null";
  else {
    os << "\n" << *m.map;
  }

  os << " }\n";
  return os;
}

bool MapLoader::isValid() {
  return *isFormatValid && map != nullptr &&
         *map->validateErr == Map::Err::None;
}

const char *MapLoader::getErrMsg() {
  if (!*isFormatValid) {
    return "text file format is wrong.";
  } else {
    switch (*map->validateErr) {
    case Map::Err::None:
      return "no error.";

    case Map::Err::MapNotConnected:
      return "map is not connected.";

    case Map::Err::ContinentNotConnected:
      return "at least one of the continents is not connected.";

    case Map::Err::TerritoryContinentInvalid:
      return "at least one of the territories belongs to an invalid continent.";

    case Map::Err::InconsistentBorders:
      return "borders are not consistent.";

    case Map::Err::ZeroTerritories:
      return "the map has 0 territories.";

    case Map::Err::ContinentZeroTerritories:
      return "at least one of the continents has 0 territories.";
    };
  }
}

int MapLoader::getContinentsNum() { return (int)map->continents->size(); }

int MapLoader::getTerritoriesNum() { return (int)map->territories->size(); }

bool MapLoader::areTerritoriesAdj(int territoryIdA, int territoryIdB) {
  return *(*(*map->borders)[territoryIdA])[territoryIdB];
}

string *MapLoader::getContinentName(int continentId) {
  return (*map->continents)[continentId]->name;
}

int MapLoader::getContinentBonusValue(int continentId) {
  return *(*map->continents)[continentId]->bonusValue;
}

vector<int *> *MapLoader::getContinentTerritoriesIds(int continentId) {
  return (*map->continents)[continentId]->territoriesIds;
}

int MapLoader::getContinentPlayerId(int continentId) {
  int playerId =
      *(*map->territories)[*(*map->continents)[0]->territoriesIds->front()]
           ->playerId;
  std::cout << "--debug " << playerId << "\n";
  for (auto territoryId : *(*map->continents)[continentId]->territoriesIds) {
    int p = *(*map->territories)[*territoryId]->playerId;
    std::cout << "--debug " << p << "\n";
    if (p != playerId)
      return -1;
  }
  return playerId;
}

string *MapLoader::getTerritoryName(int territoryId) {
  return (*map->territories)[territoryId]->name;
}

int MapLoader::getTerritoryContinentId(int territoryId) {
  return *(*map->territories)[territoryId]->continentId;
}

int MapLoader::getTerritoryPlayerId(int territoryId) {
  return *(*map->territories)[territoryId]->playerId;
}

void MapLoader::setTerritoryPlayerId(int territoryId, int playerId) {
  delete (*map->territories)[territoryId]->playerId;
  (*map->territories)[territoryId]->playerId = new int(playerId);
}

int MapLoader::getTerritoryArmiesNum(int territoryId) {
  return *(*map->territories)[territoryId]->armiesNum;
}

void MapLoader::setTerritoryArmiesNum(int territoryId, int armiesNum) {
  delete (*map->territories)[territoryId]->armiesNum;
  (*map->territories)[territoryId]->armiesNum = new int(armiesNum);
}

vector<int *> *MapLoader::getTerritoryNeighborsIds(int territoryId) {
  return (*map->territories)[territoryId]->neighborsIds;
}
