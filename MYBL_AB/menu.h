#ifndef MENU_BITMAPS_H
#define MENU_BITMAPS_H

#include <Arduino.h>
#include "globals.h"

#define FONT_TINY                 0
#define FONT_SMALL                1
#define FONT_BIG                  2

#define DATA_TIMER                0
#define DATA_SCORE                1
#define DATA_LEVEL                2

byte blinkingFrames = 0;
byte sparkleFrames = 0;
byte cont = 0;

extern void drawNumbers(byte numbersX, byte numbersY, byte fontType, byte data);

void drawTitleScreen()
{
  if (arduboy.everyXFrames(7))blinkingFrames = (++blinkingFrames) % 32;
  for (byte i = 0; i < 4; i++) sprites.drawSelfMasked(32 * i, (HEIGHT / 4 - 16), titleScreen, i);
  #if WIDTH == 96
  sprites.drawSelfMasked(85 - 60, 45 + (HEIGHT - 64), badgeMysticBalloon, 0);
  sprites.drawSelfMasked(79 - 60, 43 + (HEIGHT - 64), stars, sparkleFrames);
  #else
  sprites.drawSelfMasked(85, 45 + (HEIGHT - 64), badgeMysticBalloon, 0);
  sprites.drawSelfMasked(79, 43 + (HEIGHT - 64), stars, sparkleFrames);
  #endif
  sprites.drawSelfMasked(9, 9 + (HEIGHT / 4 - 16), leftGuyLeftEye, pgm_read_byte(&blinkingEyesLeftGuy[blinkingFrames]));
  sprites.drawSelfMasked(15, 13 + (HEIGHT / 4 - 16), leftGuyRightEye, pgm_read_byte(&blinkingEyesLeftGuy[blinkingFrames]));
  sprites.drawSelfMasked(109, 34 + (HEIGHT / 4 - 16), rightGuyEyes, pgm_read_byte(&blinkingEyesRightGuy[blinkingFrames]));
}

void stateMenuIntro()
{
  globalCounter++;
  if (globalCounter < 160)
  {
    sprites.drawSelfMasked(34 - (64 - WIDTH / 2), 4 + (HEIGHT / 2 - 32), T_arg, 0);
  }
  else
  {
    drawTitleScreen();
    if ((globalCounter > 250) || arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
  }
}

void stateMenuMain()
{

  drawTitleScreen();
  sprites.drawOverwrite(51 - (64 - WIDTH / 2), 9 + (HEIGHT / 2 - 32), mainMenu, 0);
  if (arduboy.justPressed(DOWN_BUTTON) && (menuSelection < 5)) menuSelection++;
  if (arduboy.justPressed(UP_BUTTON) && (menuSelection > 2)) menuSelection--;
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = menuSelection;
  sprites.drawPlusMask(46 - (64 - WIDTH / 2), 9 + 9 * (menuSelection - 2) + (HEIGHT / 2 - 32), selector_plus_mask, 0);
}

void stateMenuHelp()
{
  for (byte i = 0; i < 2; i++) sprites.drawSelfMasked(32 - (64 - WIDTH / 2), 32 * i + (HEIGHT / 2 - 32), qrcode, i);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}


void stateMenuInfo()
{
  sprites.drawSelfMasked(43 - (64 - WIDTH / 2), 2, badgeMysticBalloon, 0);
  sprites.drawSelfMasked(37 - (64 - WIDTH / 2), 0, stars, sparkleFrames);
  sprites.drawSelfMasked(40 - (64 - WIDTH / 2), 48 + (HEIGHT / 2 - 32), madeBy, 0);
  EEPROM.get(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_HSCORE, scorePlayer);
  if (EEPROM.read(EEPROM_STORAGE_SPACE_START + EEPROM_MYSTIC_START + OFFSET_COINSHS) == TOTAL_COINS)
  {
    sprites.drawSelfMasked(21 - (64 - WIDTH / 2), 28 + (HEIGHT / 4 - 16), badgeSuper, 0);
  }
  else
  {
    sprites.drawSelfMasked(28 - (64 - WIDTH / 2), 28 + (HEIGHT / 4 - 16), badgeBorder, 0);
  }
  sprites.drawSelfMasked(30 - (64 - WIDTH / 2), 28 + (HEIGHT / 4 - 16), badgeHighScore, 0);
  drawNumbers(55 - (64 - WIDTH / 2), 30 + (HEIGHT / 4 - 16), FONT_BIG, DATA_SCORE);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuSoundfx()
{
  drawTitleScreen();
  sprites.drawOverwrite(51 - (64 - WIDTH / 2), 9 + (HEIGHT / 2 - 32), soundMenu, 0);
  if (arduboy.justPressed(DOWN_BUTTON)) arduboy.audio.on();
  if (arduboy.justPressed(UP_BUTTON)) arduboy.audio.off();
  sprites.drawPlusMask(54 - (64 - WIDTH / 2), 18 + 9 * arduboy.audio.enabled() + (HEIGHT / 2 - 32), selector_plus_mask, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    arduboy.audio.saveOnOff();
    gameState = STATE_MENU_MAIN;
  }
}

void stateMenuPlaySelect()
{
  drawTitleScreen();
  sprites.drawOverwrite(53 - (64 - WIDTH / 2), 18 + (HEIGHT / 2 - 32), continueMenu, 0);
  if (arduboy.justPressed(DOWN_BUTTON)) cont = 1;
  if (arduboy.justPressed(UP_BUTTON)) cont = 0;
  sprites.drawPlusMask(48 - (64 - WIDTH / 2), 18 + 9 * cont + (HEIGHT / 2 - 32), selector_plus_mask, 0);
  if (arduboy.justPressed(B_BUTTON))
  {
    gameState = STATE_GAME_PLAYCONTNEW + cont;
    cont = 0;
  }
  if (arduboy.justPressed(A_BUTTON))
  {
    gameState = STATE_MENU_MAIN;
  }
}


#endif
