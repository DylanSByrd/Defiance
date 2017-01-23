#pragma once

#include <vector>
#include "Engine/Math/Vector2i.hpp"
#include "Game/Map/MapProxy.hpp"

//-----------------------------------------------------------------------------------------------
// #TODO - templatize positions
// template <typename PositionType>


//-----------------------------------------------------------------------------------------------
enum PathfinderResult
{
   INCOMPLETE_RESULT,
   NO_PATH,
   PATH_FOUND,
   NUM_PATHFINDER_RESULTS,
};


//-----------------------------------------------------------------------------------------------
struct PathNode
{
   virtual ~PathNode() {}
   Vector2i position;
   PathNode* parent;
};


// #TODO - copy constructor
//-----------------------------------------------------------------------------------------------
struct Path
{
   Path(const Vector2i& start, const Vector2i& end);
   ~Path();

   const Vector2i& GetStepTowardsPathEnd() const;
   void UpdatePathPositions();

   Vector2i pathStart;
   Vector2i pathEnd;
   std::vector<Vector2i> pathPositions;
   std::vector<PathNode*> openList;
   std::vector<PathNode*> closedList;
   PathNode* activeNode;
};


//-----------------------------------------------------------------------------------------------
class Pathfinder
{
public:
   Pathfinder(const Vector2i& start, const Vector2i& goal, const MapProxy& map);
   virtual ~Pathfinder() {}

   virtual PathfinderResult FindPath() = 0;
   virtual PathfinderResult TakeStep() = 0;
   
   void AddNodeToOpenList(PathNode* node);
   void AddNodeToClosedList(PathNode* node);
   bool IsNodePositionOnClosedList(PathNode* node) const;
   PathNode* FindOpenNodeWithPosOnOpenList(const Vector2i& pos);

   PathfinderResult GetResult() const { return m_result; }
   const Path& GetPath() const { return m_path; }

protected:
   Path m_path;
   PathfinderResult m_result;
   MapProxy m_map;
};