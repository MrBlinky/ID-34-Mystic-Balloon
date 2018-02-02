#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "globals.h"
#include "inputs.h"
#include "player.h"
#include "enemies.h"
#include "elements.h"
#include "levels.h"

#define TOTAL_TONES 10
const int tones[] = {
  200, 100, 250, 125, 300, 150, 350, 400, 425, 475
};

byte toneindex = 0;

void stateMenuPlayNew()
{
  level = LEVEL_TO_START_WITH - 1;
  coinsCollected = 0;
  totalCoins = 0;
  balloonsLeft = 0;
  scorePlayer = 0;
  globalCounter = 0;
  kid.balloons = 3;
  gameState = STATE_GAME_NEXT_LEVEL;
  scoreIsVisible = false;
  nextLevelIsVisible = true;
  pressKeyIsVisible = false;
}

void stateMenuPlayContinue()
{
  //level = LEVEL_TO_START_WITH - 1;
  //EEPROM.get(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + 1, level);
  level = EEPROM.read(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_LEVEL);
  totalCoins = EEPROM.read(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_COINS);
  coinsCollected = 0;
  balloonsLeft = 0;
  //scorePlayer = 0;
  EEPROM.get(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_SCORE, scorePlayer);
  globalCounter = 0;
  kid.balloons = 3;
  gameState = STATE_GAME_NEXT_LEVEL;
  scoreIsVisible = false;
  nextLevelIsVisible = true;
  pressKeyIsVisible = false;
}


void stateGameNextLevel()
{
  //if (level < TOTAL_LEVELS)
  //{
    if (arduboy.everyXFrames(20))
    {
      canPressButton = false;
      if (coinsCollected > 0)
      {
        coinsCollected--;
        scorePlayer += 20;
        sound.tone(tones[toneindex++], 150);
      }
      else if (balloonsLeft > 0)
      {
        balloonsLeft--;
        scorePlayer += 30;
        sound.tone(tones[toneindex++], 150);
      }
      else
      {
        canPressButton = true;
        scoreIsVisible = false;
        pressKeyIsVisible = !pressKeyIsVisible;
        if (toneindex < TOTAL_TONES)
        {
          sound.tone(tones[toneindex], 150);
          toneindex = TOTAL_TONES;
        }
        if (level >= TOTAL_LEVELS)
          gameState = STATE_GAME_OVER;
      }
    }
  /*}
  else
  {
    gameState = STATE_GAME_OVER;
    return;
  }*/

  // Update EEPROM
  EEPROM.put(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_LEVEL, level);
  EEPROM.put(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_COINS, totalCoins);
  EEPROM.put(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_SCORE, scorePlayer);


  //if (nextLevelIsVisible)
  //{
  if (level < TOTAL_LEVELS)
  {
    sprites.drawSelfMasked(35 - (64 - WIDTH / 2), 4, badgeNextLevel, 0);
    drawNumbers(78 - (64 - WIDTH / 2), 13, FONT_BIG, DATA_LEVEL);
  }
  else
  {
    EEPROM.put(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_LEVEL, (byte)LEVEL_TO_START_WITH - 1);
    // Score remains after completing game? (no)
    EEPROM.put(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_SCORE, (unsigned long)0);
  }
  drawNumbers(43 - (64 - WIDTH / 2), 49 + (HEIGHT / 2 - 32), FONT_BIG, DATA_SCORE);
  //}

  if (scoreIsVisible)
  {
    byte totalBadges = coinsCollected + balloonsLeft;

    for (byte i = 0; i < totalBadges; ++i)
    {
    //#if WIDTH == 96
    //  if (i < coinsCollected) sprites.drawOverwrite(49  - (7 * coinsCollected) + (i * 14), 27, badgeElements, 0);
    //  else sprites.drawOverwrite(49 - (7 * balloonsLeft) + ((i - coinsCollected) * 14), 27 + 18), badgeElements, 1);
    //#else  
      if (i < coinsCollected) sprites.drawOverwrite(65  - (64 - WIDTH / 2) - (7 * totalBadges) + (i * 14), 27 + (HEIGHT / 4 - 16), badgeElements, 0);
      else sprites.drawOverwrite(65 - (64 - WIDTH / 2) - (7 * totalBadges) + (i * 14), 27 + (HEIGHT / 4 - 16), badgeElements, 1);
    //#endif
    }
  }

  if (canPressButton)
  {

    if (pressKeyIsVisible) sprites.drawOverwrite(38 - (64 - WIDTH / 2), 29 + (HEIGHT / 4 - 16), badgePressKey, 0);
    if (arduboy.justPressed(A_BUTTON | B_BUTTON))
    {
      toneindex = 0;
      sound.tone(425, 20);
      setKid();
      //cam.pos = vec2(0, 0);
      cam.pos = vec2(0, LEVEL_HEIGHT - 64);
      cam.offset = vec2(0, 0);
      enemiesInit();
      levelLoad(levels[level]);
      gameState = STATE_GAME_PLAYING;
    }
  }
};


void stateGamePlaying()
{
  checkInputs();
  checkKid();
  updateCamera();

  drawGrid();
  enemiesUpdate();

  drawKid();
  drawHUD();

  checkCollisions();
}


void stateGamePause()
{
  sprites.drawSelfMasked(47 - (64 - WIDTH / 2), 17 + (HEIGHT / 2 - 32), badgePause, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    gameState = STATE_GAME_PLAYING;
  }
}


void stateGameOver()
{
  byte x = 35 + 12;
  if (level < TOTAL_LEVELS)
  {
    drawNumbers(78 - (64 - WIDTH / 2), 26, FONT_BIG, DATA_LEVEL);
    x -= 12;
  }
  sprites.drawSelfMasked(x  - (64 - WIDTH / 2), 17, badgeGameOver, 0);
  drawNumbers(43 - (64 - WIDTH / 2), 49, FONT_BIG, DATA_SCORE);

  unsigned long highscore = 0;
  EEPROM.get(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_HSCORE, highscore);
  if (scorePlayer > highscore) {
    EEPROM.put(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_COINSHS, totalCoins);
    EEPROM.put(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_HSCORE, scorePlayer);
  }
    
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    gameState = STATE_MENU_MAIN;
  }
}

#endif
