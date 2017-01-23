#pragma once

#include "Game/FieldOfView/FieldOfView.hpp"


//-----------------------------------------------------------------------------------------------
class FieldOfViewBasic
   : public FieldOfView
{
public:
   virtual void CalculateFieldOfViewForAgent(Agent* agent, int viewDistance, Map* map, bool isPlayer) override;
};