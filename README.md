# Defiance
2D Roguelike written in C++

Defiance - A Simple 2D Roguelike, Minus Some Features - Made by Dylan Byrd - dsbyrd@smu.edu

About
//-----------------------------------------------------------------------------------------------
You wake up, unsure of where you are.
You remember the end of the world, and the life after.
Look for other survivors. Avoid monsters and bandits.
Good Luck.


Known Issues
//-----------------------------------------------------------------------------------------------
- Although the environment files may have a range of steps, the number in that range is calculated
  on startup, meaning the number of steps used in generation will always be the same.
- The GenerationProcesses currently only influence the number of generation steps to take, except
  for the FromData generator.
- The FromData generator does not support map legends; map data must use the same characters used
  in the game. Same with loading in a file.
- The river generator doesn't travel North-To-South very often.


How to Use
//-----------------------------------------------------------------------------------------------
During play, press the spacebar or Numpad5 to skip a turn
During play, movement is either hjklyubn or the numpad;
During play, press P to simulate an A* pathfinding step.
During play, press Shift + P to simulate 10,000 A* pathfinding steps.
During play, press R to draw a ray from the player to a random open tile. Note: This is buggy.
During play, press M to reveal the entire map and all entities.
During play, press PgUp or PgDn to scroll through the message log.
During play, press Q to use a consumable.
During play, press ',' to pick up an item.
During play, press A to toggle doors.


If a warning appears regarding FMOD, open the project settings for Defiance. Navigate to configuration 
properties -> Debugging and change the Working Directory field to $(SolutionDir)Run_$(PlatformName)/.
Repeat for the Engine project if necessary.


How to Test Features
//-----------------------------------------------------------------------------------------------
- I suggest using the Dungeon Generator to see doors
- Shots of Adrenaline double the player's speed