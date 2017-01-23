#pragma once

#include <map>


//-----------------------------------------------------------------------------------------------
class Vector2i;
class Agent;
class Item;
class Feature;


//-----------------------------------------------------------------------------------------------
typedef Vector2i TileCoords;
typedef unsigned int TileIndex;
typedef std::multimap<float, Agent*> TurnOrderMap;
typedef std::pair<float, Agent*> TurnOrderMapPair;
typedef TurnOrderMap::iterator TurnOrderMapIter;
typedef int EntityID;
typedef int FactionID;
typedef std::multimap<float, Agent*> DistanceToAgentMap;
typedef std::pair<float, Agent*> DistanceToAgentPair;
typedef DistanceToAgentMap::iterator DistanceToAgentIter;
typedef DistanceToAgentMap::const_iterator DistanceToAgentConstIter;
typedef std::multimap<float, Item*> DistanceToItemMap;
typedef std::pair<float, Item*> DistanceToItemPair;
typedef DistanceToItemMap::iterator DistanceToItemIter;
typedef std::multimap<float, Feature*> DistanceToFeatureMap;
typedef std::pair<float, Feature*> DistanceToFeaturePair;
typedef DistanceToFeatureMap::iterator DistanceToFeatureIter;


//-----------------------------------------------------------------------------------------------
enum TileDirection
{
   INVALID_DIRECTION = -1,
   NORTH,
   SOUTH,
   EAST,
   WEST,
   NORTH_WEST,
   NORTH_EAST,
   SOUTH_WEST,
   SOUTH_EAST,
   NUM_TILE_DIRECTIONS,
};
const int NUM_CARDINAL_DIRECTIONS = NUM_TILE_DIRECTIONS / 2;


//-----------------------------------------------------------------------------------------------
TileDirection GetRandomTileDirection();