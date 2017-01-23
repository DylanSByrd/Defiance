#pragma once

#include "Game/Entities/Items/Inventory.hpp"

//-----------------------------------------------------------------------------------------------
class Agent;
class Feature;


//-----------------------------------------------------------------------------------------------
enum TileType
{
   INVALID_TYLE_TYPE = -1,
   AIR_TYPE,
   STONE_TYPE,
   WATER_TYPE,
   NUM_TILE_TYPES,
};


//-----------------------------------------------------------------------------------------------
struct Tile
{
   Tile(); 
   Tile(TileType initType);

   bool IsOccupiedByAgent() const { return (occupyingAgent != nullptr); }
   int GetNumItemsOnTile() const { return inventory.GetNumItemsInInventory(); }
   char GetItemGlyph() const;
   void RemoveAgent() { occupyingAgent = nullptr; }
   void ClearAgent();
   bool HasItems() const;
   Items GetItems() const { return inventory.GetAllItems(); }
   bool HasAFeature() const { return (occupyingFeature != nullptr); }
   bool ToggleFeature();
   bool DoesBlockLineOfSight() const;
   bool DoesBlockPathing() const;
   void PrintItemInfo() const { inventory.PrintItemInfo(); }

   static Tile INVALID_TILE;

   TileType type;
   TileType typeToBecome;
   Agent* occupyingAgent;
   Feature* occupyingFeature;
   bool isHidden;
   bool isVisible;
   bool isKnown;
   Inventory inventory;
};