#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Combat/DefianceCombatSystem.hpp"
#include "Game/Entities/Agents/Agent.hpp"
#include "Game/UI/GameMessageBox.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Core/GameContext.hpp"
#include "Game/Entities/Agents/Player.hpp"


//-----------------------------------------------------------------------------------------------
STATIC void DefianceCombatSystem::PerformAttack(AttackData& attackData)
{
   bool didAttackLand = GetRandomTrueOrFalseWithProbability(attackData.percentChanceToHit);

   if (!didAttackLand)
   {
      if (attackData.minDamage > 0)
      {
         attackData.result = ATTACK_RESULT_ATTACK_MISS;
      }
      else
      {
         attackData.result = ATTACK_RESULT_HEAL_MISS;
      }

      attackData.damageDealt = 0;
      attackData.didHit = false;
      SendAttackData(attackData);
      return;
   }

   //else didAttackLand
   if (attackData.minDamage < 0)
   {
      attackData.damageDealt = GetRandomIntBetweenInclusive(attackData.maxDamage, attackData.minDamage);
   }
   else
   {
      attackData.damageDealt = GetRandomIntBetweenInclusive(attackData.minDamage, attackData.maxDamage);
      ApplyWeaponBonus(attackData);
      ApplyArmorBonus(attackData);
   }

   attackData.didHit = true;

   if (attackData.target->GetCurrentHealth() - attackData.damageDealt <= 0)
   {
      if (attackData.target->IsPlayer() && g_theGame->IsGodModeEnabled())
      {
         attackData.damageDealt = attackData.target->GetCurrentHealth() - 1;
         attackData.result = ATTACK_RESULT_HIT;
      }
      else
      {
         attackData.result = ATTACK_RESULT_KILL;
         if (attackData.instigator->IsPlayer())
         {
            g_theGame->IncrementPlayerKillCount();
         }
         else if (attackData.target->IsPlayer())
         {
            g_theGame->SetStateGameOver();
         }
      }
   }
   else if (attackData.minDamage < 0)
   {
      attackData.result = ATTACK_RESULT_HEAL;
   }
   else
   {
      attackData.result = ATTACK_RESULT_HIT;
   }

   SendAttackData(attackData);
}


//-----------------------------------------------------------------------------------------------
STATIC void DefianceCombatSystem::SendAttackData(AttackData& attackData)
{
   attackData.target->ReceiveAttack(attackData);
   LogAttackToMessageBox(attackData);
}


//-----------------------------------------------------------------------------------------------
STATIC void DefianceCombatSystem::LogAttackToMessageBox(AttackData& attackData)
{
   // This feels dirty
   const GameContext* context = g_theGame->GetGameContext();
   const Player* player = context->activePlayer;
   if (player == nullptr
      || (!(player->IsAgentVisible(attackData.instigator))
         && !(player->IsAgentVisible(attackData.target))))
   {
      return;
   }

   std::string attacker = attackData.instigator->IsPlayer() ? "You" : "The " + attackData.instigator->GetName();
   std::string target = attackData.target->IsPlayer() ? "you" : "the " + attackData.target->GetName();

   switch (attackData.result)
   {
   case ATTACK_RESULT_ATTACK_MISS:
   {
      std::string missString = attackData.instigator->IsPlayer() ? " miss " : " misses ";
      g_theGameMessageBox->PrintNeutralMessage(attacker + missString + target + ".");
      break;
   }

   case ATTACK_RESULT_HEAL_MISS:
   {
      std::string missString = attackData.instigator->IsPlayer() ? " fail to heal " : " fails to heal ";
      g_theGameMessageBox->PrintNeutralMessage(attacker + missString + target + ".");
      break;
   }

   case ATTACK_RESULT_HIT:
   {
      std::string hitString = attackData.instigator->IsPlayer() ? " hit " : " hits ";
      std::string damageString = Stringf(" for %d damage!", attackData.damageDealt);

      if (attackData.target->IsPlayer())
      {
         g_theGameMessageBox->PrintDangerMessage(attacker + hitString + target + damageString);
      }
      else
      {
         g_theGameMessageBox->PrintCautionMessage(attacker + hitString + target + damageString);
      }
      break;
   }

   case ATTACK_RESULT_KILL:
   {   
      if (attackData.target->IsPlayer())
      {
         std::string killString = " kills ";
         g_theGameMessageBox->PrintDangerMessage(attacker + killString + target + ".");
      }
      else if (attackData.instigator->IsPlayer())
      {
         std::string killString = " kill ";
         g_theGameMessageBox->PrintGoodMessage(attacker + killString + target + ".");
      }
      else
      {
         std::string killString = " kills ";
         g_theGameMessageBox->PrintCautionMessage(attacker + killString + target + ".");
      }
      break;
   }

   case ATTACK_RESULT_HEAL:
   {
      std::string healString = " heals ";
      std::string healAmountString = Stringf(" for %d health!", -attackData.damageDealt);
      
      if (attackData.target->IsPlayer())
      {
         g_theGameMessageBox->PrintGoodMessage(attacker + healString + target + healAmountString);
      }
      else
      {
         g_theGameMessageBox->PrintNeutralMessage(attacker + healString + target + healAmountString);
      }
   }
   }
}


//-----------------------------------------------------------------------------------------------
STATIC void DefianceCombatSystem::ApplyWeaponBonus(AttackData& attackData)
{
   Agent* attacker = attackData.instigator;
   attackData.damageDealt += attacker->GetWeaponBonus();
}


//-----------------------------------------------------------------------------------------------
STATIC void DefianceCombatSystem::ApplyArmorBonus(AttackData& attackData)
{
   Agent* defender = attackData.target;
   attackData.damageDealt -= (defender->GetArmorBonus() / 2);
   if (attackData.damageDealt < 0)
   {
      attackData.damageDealt = 0;
   }
}