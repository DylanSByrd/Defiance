#pragma once


//-----------------------------------------------------------------------------------------------
class Agent;


//-----------------------------------------------------------------------------------------------
enum AttackResult
{
   ATTACK_RESULT_ATTACK_MISS,
   ATTACK_RESULT_HIT,
   ATTACK_RESULT_KILL,
   ATTACK_RESULT_HEAL,
   ATTACK_RESULT_HEAL_MISS,
   NUM_ATTACK_RESULTS,
};


//-----------------------------------------------------------------------------------------------
struct AttackData
{
   int minDamage;
   int maxDamage;
   float percentChanceToHit;
   Agent* target;
   Agent* instigator;
   AttackResult result;
   int damageDealt;
   bool didHit;
};


//-----------------------------------------------------------------------------------------------
class DefianceCombatSystem
{
public:
   static void PerformAttack(AttackData& attackData);
   static void SendAttackData(AttackData& attackData);
   static void LogAttackToMessageBox(AttackData& attackData);
   static void ApplyWeaponBonus(AttackData& attackData);
   static void ApplyArmorBonus(AttackData& attackData);
};