#include "Game/Pathfinding/PathfinderAStar.hpp"


//-----------------------------------------------------------------------------------------------
void PathNodeAStar::UpdateNodeValues(float inLocalg, float inParentg, PathNodeAStar* inParent)
{
   localg = inLocalg;
   parentg = inParentg;
   parent = inParent;
   f = GetTotalG() + h;
}


//-----------------------------------------------------------------------------------------------
PathfinderAStar::PathfinderAStar(const Vector2i& start, const Vector2i& end, const MapProxy& map)
   : Pathfinder(start, end, map)
{
   PathNode* activeNode = CreateNode(start, nullptr, 0.f, 0.f, 0.f);
   m_path.activeNode = activeNode;
   m_path.pathPositions.push_back(start);
   AddNodeToOpenList(activeNode);
}


//-----------------------------------------------------------------------------------------------
PathfinderResult PathfinderAStar::FindPath()
{
   while (m_result == INCOMPLETE_RESULT)
   {
      TakeStep();
   }

   return m_result;
}


//-----------------------------------------------------------------------------------------------
PathfinderResult PathfinderAStar::TakeStep()
{
   if (m_result != INCOMPLETE_RESULT)
   {
      return m_result;
   }

   if (m_path.openList.empty())
   {
      m_result = NO_PATH;
      return m_result;
   }

   PathNodeAStar* activeNode = FindLowestFNodeOnOpenListAndRemoveIt();
   AddNodeToClosedList(activeNode);
   m_path.activeNode = activeNode;

   if (activeNode->position == m_path.pathEnd)
   {
      m_path.UpdatePathPositions();
      m_result = PATH_FOUND;
      return m_result;
   }


   std::vector<PathNodeAStar*> validNodes = FindValidAdjacentNodes();
   for (PathNodeAStar* node : validNodes)
   {
      if (IsNodePositionOnClosedList(node))
      {
         delete node;
         continue;
      }

      PathNodeAStar* existingNode = dynamic_cast<PathNodeAStar*>(FindOpenNodeWithPosOnOpenList(node->position));
      if (existingNode != nullptr)
      {
         float activeTotalG = activeNode->GetTotalG();
         float existingTotalG = existingNode->GetTotalG();

         if (activeTotalG < existingTotalG)
         {
            existingNode->UpdateNodeValues(node->localg, node->parentg, dynamic_cast<PathNodeAStar*>(node->parent));
         }
         delete node;
         continue;
      }

      AddNodeToOpenList(node);
   }

   m_path.UpdatePathPositions();
   m_result = INCOMPLETE_RESULT;
   return m_result;
}


//-----------------------------------------------------------------------------------------------
PathNodeAStar* PathfinderAStar::CreateNode(const Vector2i& position, PathNodeAStar* parent, float localg, float parentg, float h)
{
   PathNodeAStar* newNode = new PathNodeAStar();
   newNode->position = position;
   newNode->parent = parent;
   newNode->localg = localg;
   newNode->parentg = parentg;
   newNode->h = h;
   newNode->f = localg + parentg + h;

   return newNode;
}


//-----------------------------------------------------------------------------------------------
std::vector<PathNodeAStar*> PathfinderAStar::FindValidAdjacentNodes()
{
   PathNodeAStar* parent = dynamic_cast<PathNodeAStar*>(m_path.activeNode);
   std::vector<Vector2i> validPositions = m_map.FindValidAdjacentPositions(parent->position);

   std::vector<PathNodeAStar*> newNodes;
   newNodes.reserve(validPositions.size());
   for (const Vector2i& position : validPositions)
   {
      float localg = m_map.ComputeLocalAStarG(parent->position, position);
      float h = m_map.ComputeEstimatedAStarH(position, m_path.pathEnd);
      PathNodeAStar* node = CreateNode(position, parent, localg, parent->GetTotalG(), h);
      newNodes.push_back(node);
   }

   return newNodes;
}


//-----------------------------------------------------------------------------------------------
PathNodeAStar* PathfinderAStar::FindLowestFNodeOnOpenListAndRemoveIt()
{
   std::vector<PathNode*>& openList = m_path.openList;

   PathNodeAStar* lowestNode = nullptr;
   int lowestIndex = 0;
   for (size_t nodeIndex = 0; nodeIndex< openList.size(); ++nodeIndex)
   {
      PathNodeAStar* currentNode = dynamic_cast<PathNodeAStar*>(openList[nodeIndex]);
      if (lowestNode == nullptr
         || currentNode->f < lowestNode->f)
      {
         lowestNode = currentNode;
         lowestIndex = nodeIndex;
      }
   }

   std::vector<PathNode*>::iterator nodeIter = openList.begin() + lowestIndex;
   openList.erase(nodeIter);
   return lowestNode;
}