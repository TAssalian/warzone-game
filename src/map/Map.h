#pragma once
#include <iostream>
#include <vector>
using std::vector, std::string, std::ostream;

// continents and territories ids are 0 based and are modified from the text file.


class Continent {
public:
	// all properties are excepted to be defined

	int* id;
	string* name;
	int* bonusValue; // extra armies a player has all continents' territories
	vector<int*>* territoriesIds;

	Continent() = default;
	Continent(const Continent& other);
	~Continent();
	void swap(Continent& other);
	Continent& operator=(Continent other);

	friend ostream& operator<<(ostream& os, const Continent& continent);
};

class Territory {
public:
	// all properties are excepted to be defined

	int* id;
	string* name;
	int* continentId;
	int* playerId;
	int* armiesNum;
	vector<int*>* neighborsIds;

	Territory() = default;
	Territory(int id, string name, int continentId);
	Territory(const Territory& other);
	~Territory();
	void swap(Territory& other);
	Territory& operator=(const Territory other);

	string* getName() const;
	vector<int*>* getNeighborsIds() const;
	int* getArmiesNum() const;
	void setArmiesNum(const int armiesNum);

	friend ostream& operator<<(ostream& os, const Territory& territory);
};

class Map {
public:
	// all properties are excepted to be defined

	enum class Err {
		None,
		MapNotConnected,
		ContinentNotConnected,
		TerritoryContinentInvalid,
		InconsistentBorders,
		ZeroTerritories,
		ContinentZeroTerritories
	};

	vector<Continent*>* continents; // continents[i] -> continent with id i
	vector<Territory*>* territories; // territories[i] -> territory with id i
	vector<vector <bool*>*>* borders; // borders[i][j] -> is there a border between territory with id i and j
	Err* validateErr; // error type after validating - default: None

	Map() = default;
	Map(const Map& other);
	~Map();
	void swap(Map& other);
	Map& operator=(Map other);
	void validate();

	friend ostream& operator<<(ostream& os, const Map& map);
};

class MapLoader {
public:
	// all properties are excepted to be defined except map

	string* filename;
	bool* isFormatValid;
	Map* map;

	//
	// useful methods
	//

	// creates map for filename
	MapLoader(string filename);

	// true if format and sturcture is right
	bool isValid();

	// get error message if it is not valid, otherwise "no error"
	const char* getErrMsg();

	int getContinentsNum();

	int getTerritoriesNum();

	// are territories adjacent based on their ids
	bool areTerritoriesAdj(int territoryIdA, int territoryIdB);

	string* getContinentName(int continentId);

	int getContinentBonusValue(int continentId);

	// adds continent territories ids in given vector vc
	vector<int*>* getContinentTerritoriesIds(int continentId);

	// if one player owns continent, return playerId, otherwise -1
	int getContinentPlayerId(int continentId);

	string* getTerritoryName(int territoryId);

	int getTerritoryContinentId(int territoryId);

	// if a player owns territory, return player id, otherwise -1
	int getTerritoryPlayerId(int territoryId);

	void setTerritoryPlayerId(int territoryId, int playerId);

	// default 0
	int getTerritoryArmiesNum(int territoryId);

	void setTerritoryArmiesNum(int territoryId, int armiesNum);

	vector<int*>* getTerritoryNeighborsIds(int territoryId);

	//
	//
	//

	MapLoader(const MapLoader& other);
	~MapLoader();
	void swap(MapLoader& other);
	MapLoader& operator=(MapLoader other);

	friend ostream& operator<<(ostream& os, const MapLoader& mapLoader);

};
