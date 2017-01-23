#include "Game/Pathfinding/Pathfinder.hpp"


//-----------------------------------------------------------------------------------------------
Path::Path(const Vector2i& start, const Vector2i& end)
   : pathStart(start)
   , pathEnd(end)
   , activeNode(nullptr)
{
}


//-----------------------------------------------------------------------------------------------
Path::~Path()
{
   for (PathNode* node : openList)
   {
      delete node;
      node = nullptr;
   }

   for (PathNode* node : closedList)
   {
      delete node;
      node = nullptr;
   }
}


//-----------------------------------------------------------------------------------------------
const Vector2i& Path::GetStepTowardsPathEnd() const
{
   return pathPositions[pathPositions.size() - 2];
}


//-----------------------------------------------------------------------------------------------
void Path::UpdatePathPositions()
{
   pathPositions.clear();

   PathNode* currentNode = activeNode;
   while (currentNode)
   {
      pathPositions.push_back(currentNode->position);
      currentNode = currentNode->parent;
   }
}


//-----------------------------------------------------------------------------------------------
Pathfinder::Pathfinder(const Vector2i& start, const Vector2i& goal, const MapProxy& map)
   : m_path(start, goal)
   , m_result(INCOMPLETE_RESULT)
   , m_map(map)
{
}


//-----------------------------------------------------------------------------------------------
void Pathfinder::AddNodeToOpenList(PathNode* node)
{
   m_path.openList.push_back(node);
}


//-----------------------------------------------------------------------------------------------
void Pathfinder::AddNodeToClosedList(PathNode* node)
{
   m_path.closedList.push_back(node);
}


//-----------------------------------------------------------------------------------------------
bool Pathfinder::IsNodePositionOnClosedList(PathNode* node) const
{
   bool nodeIsOnList = true;

   for (const PathNode* const closedNode : m_path.closedList)
   {
      if (node->position == closedNode->position)
      {
         return nodeIsOnList;
      }
   }

   return !nodeIsOnList;
}


//-----------------------------------------------------------------------------------------------
PathNode* Pathfinder::FindOpenNodeWithPosOnOpenList(const Vector2i& pos)
{
   for (PathNode* openNode : m_path.openList)
   {
      if (openNode->position == pos)
      {
         return openNode;
      }
   }

   return nullptr;
}