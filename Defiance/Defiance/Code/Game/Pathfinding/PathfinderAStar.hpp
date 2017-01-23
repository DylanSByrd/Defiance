#pragma once

#include <vector>
#include "Game/Pathfinding/Pathfinder.hpp"


//-----------------------------------------------------------------------------------------------
struct PathNodeAStar
   : public PathNode
{
   float GetTotalG();
   void UpdateNodeValues(float inLocalg, float inParentg, PathNodeAStar* inParent);

   float localg; // cost to enter
   float parentg; // parent total cost
   float h; // heuristic
   float f; // length of shortest path
};
inline float PathNodeAStar::GetTotalG() { return localg + parentg; }


//-----------------------------------------------------------------------------------------------
class PathfinderAStar
   : public Pathfinder
{
public:
   PathfinderAStar(const Vector2i& start, const Vector2i& goal, const MapProxy& map);
   ~PathfinderAStar() {}

   virtual PathfinderResult FindPath() override;
   virtual PathfinderResult TakeStep() override;

private:
   PathNodeAStar* CreateNode(const Vector2i& position, PathNodeAStar* parent, float localg, float parentg, float h);
   std::vector<PathNodeAStar*> FindValidAdjacentNodes();
   PathNodeAStar* FindLowestFNodeOnOpenListAndRemoveIt();
};