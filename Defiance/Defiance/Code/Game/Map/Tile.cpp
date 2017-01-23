#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Debug/ErrorWarningAssert.hpp"
#include "Game/Entities/Features/Feature.hpp"
#include "Game/Map/Tile.hpp"


//-----------------------------------------------------------------------------------------------
STATIC Tile Tile::INVALID_TILE(INVALID_TYLE_TYPE);


//-----------------------------------------------------------------------------------------------
Tile::Tile()
   : type(INVALID_TYLE_TYPE)
   , typeToBecome(INVALID_TYLE_TYPE)
   , occupyingAgent(nullptr)
   , occupyingFeature(nullptr)
   , isHidden(false)
   , isVisible(true)
   , isKnown(true) 
{}


//-----------------------------------------------------------------------------------------------
Tile::Tile(TileType initType)
   : type(initType)
   , typeToBecome(initType)
   , occupyingAgent(nullptr)
   , occupyingFeature(nullptr)
   , isHidden(false)
   , isVisible(true)
   , isKnown(true) 
{}


//-----------------------------------------------------------------------------------------------
char Tile::GetItemGlyph() const
{
   int itemCount = GetNumItemsOnTile();
   ASSERT_OR_DIE(itemCount > 0, "ERROR: Tile has no items!");

   if (itemCount > 1)
   {
      return '*';
   }

   return inventory.GetItemGlyph();
}


//-----------------------------------------------------------------------------------------------
void Tile::ClearAgent()
{
   occupyingAgent = nullptr;
}


//-----------------------------------------------------------------------------------------------
bool Tile::HasItems() const
{
   return (GetNumItemsOnTile() > 0);
}


//-----------------------------------------------------------------------------------------------
bool Tile::ToggleFeature()
{
   bool didToggleFeature = true;

   if (!HasAFeature())
   {
      return !didToggleFeature;
   }

   occupyingFeature->TogggleState();
   return didToggleFeature;
}


// #TODO - handle all cases here
//-----------------------------------------------------------------------------------------------
bool Tile::DoesBlockLineOfSight() const
{
   bool doesBlockLineOfSight = true;

   if (!HasAFeature())
   {
      return !doesBlockLineOfSight;
   }

   return occupyingFeature->DoesBlockLineOfSight();
}


//-----------------------------------------------------------------------------------------------
bool Tile::DoesBlockPathing() const
{
   bool doesBlockPathing = true;

   if (!HasAFeature())
   {
      return !doesBlockPathing;
   }

   return occupyingFeature->DoesBlockPathing();
}