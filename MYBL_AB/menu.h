#ifndef MENU_BITMAPS_H
#define MENU_BITMAPS_H

#include <Arduino.h>
#include "globals.h"

byte blinkingFrames = 0;
byte sparkleFrames = 0;
byte cont = 0;

void drawTitleScreen()
{
  if (arduboy.everyXFrames(7))blinkingFrames = (++blinkingFrames) % 32;
  if (arduboy.everyXFrames(10)) sparkleFrames = (++sparkleFrames) % 5;
  for (byte i = 0; i < 4; i++) sprites.drawSelfMasked(32 * i, 0, titleScreen, i);
  sprites.drawSelfMasked(85, 45, badgeMysticBalloon, 0);
  sprites.drawSelfMasked(79, 43, stars, sparkleFrames);
  sprites.drawSelfMasked(9, 9, leftGuyLeftEye, pgm_read_byte(&blinkingEyesLeftGuy[blinkingFrames]));
  sprites.drawSelfMasked(15, 13, leftGuyRightEye, pgm_read_byte(&blinkingEyesLeftGuy[blinkingFrames]));
  sprites.drawSelfMasked(109, 34, rightGuyEyes, pgm_read_byte(&blinkingEyesRightGuy[blinkingFrames]));
}

void stateMenuIntro()
{
  globalCounter++;
  if (globalCounter < 160)
  {
    for (byte i = 0; i < 4; i++) sprites.drawSelfMasked(32 * i, 10, TEAMarg, i);
    sprites.drawSelfMasked(43, 50, TEAM_argPart5, 0);
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
  sprites.drawOverwrite(51, 9, mainMenu, 0);
  if (arduboy.justPressed(DOWN_BUTTON) && (menuSelection < 5)) menuSelection++;
  if (arduboy.justPressed(UP_BUTTON) && (menuSelection > 2)) menuSelection--;
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = menuSelection;
  sprites.drawPlusMask(46, 9 + 9 * (menuSelection - 2), selector_plus_mask, 0);
}

void stateMenuHelp()
{
  for (byte i = 0; i < 2; i++) sprites.drawSelfMasked(32, 32 * i, qrcode, i);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}


void stateMenuInfo()
{
  if (arduboy.everyXFrames(10)) sparkleFrames = (++sparkleFrames) % 5;
  sprites.drawSelfMasked(43, 13, badgeMysticBalloon, 0);
  sprites.drawSelfMasked(37, 11, stars, sparkleFrames);
  sprites.drawSelfMasked(40, 40, madeBy, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) gameState = STATE_MENU_MAIN;
}

void stateMenuSoundfx()
{
  drawTitleScreen();
  sprites.drawOverwrite(51, 9, soundMenu, 0);
  if (arduboy.justPressed(DOWN_BUTTON)) arduboy.audio.on();
  if (arduboy.justPressed(UP_BUTTON)) arduboy.audio.off();
  sprites.drawPlusMask(54, 18 + 9 * arduboy.audio.enabled(),selector_plus_mask, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    arduboy.audio.saveOnOff();
    gameState = STATE_MENU_MAIN;
  }
}

void stateMenuPlaySelect()
{
  drawTitleScreen();
  sprites.drawOverwrite(51, 9, continueMenu, 0);
  if (arduboy.justPressed(DOWN_BUTTON)) cont = 1;
  if (arduboy.justPressed(UP_BUTTON)) cont = 0;
  sprites.drawPlusMask(45, 10 + 13 * cont, selector_plus_mask, 0);
  if (arduboy.justPressed(B_BUTTON))
  {
    gameState = STATE_GAME_PLAYCONTNEW + cont;
  }
  if (arduboy.justPressed(A_BUTTON))
  {
    gameState = STATE_MENU_MAIN;
  }
}


#endif
