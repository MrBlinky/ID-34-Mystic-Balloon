#ifndef ENEMIES_H
#define ENEMIES_H

#include <Arduino.h>
#include "globals.h"
//#include "vec2.h"

#define MAX_PER_TYPE 6 // total instances per enemy type

#define MAX_FAN_PARTICLES 4
#define FAN_POWER 3

struct Walker
{
  vec2 pos;
  int8_t direction;
  byte HP;
  bool hurt;
  bool active;
};

Walker walkers[MAX_PER_TYPE];

struct Spike
{
  Rect pos;
  bool active;
  const uint8_t * sprite;
  byte image;
};

Spike spikes[MAX_PER_TYPE];

struct Fan
{
  vec2 pos;
  vec2 particles[5];
  byte height;
  bool active;
};

Fan fans[MAX_PER_TYPE];

void enemiesInit()
{
  for (byte i = 0; i < MAX_PER_TYPE; ++i)
  {
    // Fans
    fans[i].pos = vec2(0, 0);
    for (byte a = 0; a < MAX_FAN_PARTICLES; ++a)
      fans[i].particles[a] = vec2(random(16), random(16));
    fans[i].height = 0;
    fans[i].active = false;

    // Spikes
    spikes[i].pos.x = 0;
    spikes[i].pos.y = 0;
    spikes[i].pos.width = 16;
    spikes[i].pos.height = 16;
    spikes[i].active = false;
    spikes[i].sprite = sprSpikes_vertical;
    spikes[i].image = 0;

    // Walkers
    walkers[i].pos.x = 0;
    walkers[i].pos.y = 0;
    walkers[i].active = false;
    walkers[i].HP = 10;
    walkers[i].direction = 1;
    walkers[i].hurt = false;
  }
}

void walkersCreate(vec2 pos)
{
  for (byte i = 0; i < MAX_PER_TYPE; ++i)
  {
    if (!walkers[i].active)
    {
      walkers[i].pos = pos << 4;
      walkers[i].pos.y += 8;
      walkers[i].active = true;
      return;
    }
  }
}

void spikesCreate(vec2 pos)
{
  for (byte i = 0; i < MAX_PER_TYPE; ++i)
  {
    if (!spikes[i].active)
    {
      spikes[i].active = true;
      spikes[i].pos.x = pos.x << 4;
      spikes[i].pos.y = pos.y << 4;
      // Solid above
      if (gridGetSolid(pos.x, pos.y - 1))
      {
        spikes[i].sprite = sprSpikes_vertical;
        spikes[i].image = 0;
        spikes[i].pos.width = 16;
        spikes[i].pos.height = 8;
      }
      // Solid below
      else if (gridGetSolid(pos.x, pos.y + 1))
      {
        spikes[i].sprite = sprSpikes_vertical;
        spikes[i].image = 1;
        spikes[i].pos.width = 16;
        spikes[i].pos.height = 8;
        spikes[i].pos.y += 8;
      }
      // Solid left
      else if (gridGetSolid(pos.x - 1, pos.y))
      {
        spikes[i].sprite = sprSpikes_horizontal;
        spikes[i].image = 1;
        spikes[i].pos.width = 8;
        spikes[i].pos.height = 16;
        //spikes[i].pos.x += 8;
      }
      // Solid right
      else if (gridGetSolid(pos.x + 1, pos.y))
      {
        spikes[i].sprite = sprSpikes_horizontal;
        spikes[i].image = 0;
        spikes[i].pos.width = 8;
        spikes[i].pos.height = 16;
        spikes[i].pos.x += 8;
      }
      return;
    }
  }
}

void fansCreate(vec2 pos, byte height)
{
  for (byte i = 0; i < MAX_PER_TYPE; ++i)
  {
    if (!fans[i].active)
    {
      fans[i].pos = pos << 4;
      fans[i].height = height;
      fans[i].active = true;
      return;
    }
  }
}

void enemiesUpdate()
{
  if (arduboy.everyXFrames(4))
    walkerFrame = (++walkerFrame) % 2;
  for (byte i = 0; i < MAX_PER_TYPE; ++i)
  {
    // Fans
    if (fans[i].active)
    {
      // Update
      if (arduboy.everyXFrames(2))
      for (byte a = 0; a < MAX_FAN_PARTICLES; ++a)
      {
        // Update Particles
        fans[i].particles[a].y =
          (fans[i].particles[a].y < (fans[i].height << 4)) ?
          fans[i].particles[a].y + 6 : random(fans[i].height << 2);
  
        // Draw particles
        arduboy.drawPixel(fans[i].pos.x + fans[i].particles[a].x - cam.pos.x,
                          fans[i].pos.y - fans[i].particles[a].y - cam.pos.y, 0);
      }
  
      // Draw fan
      sprites.drawErase(fans[i].pos.x - cam.pos.x, fans[i].pos.y - cam.pos.y, sprFan, millis() % 2);
    }

    // Spikes
    if (spikes[i].active)
    {
      sprites.drawErase(spikes[i].pos.x - cam.pos.x, spikes[i].pos.y - cam.pos.y, spikes[i].sprite, spikes[i].image);
    }

    // Walkers
    if (walkers[i].active)
    {
      if (arduboy.everyXFrames(2))
      {
        if (!gridGetSolid((walkers[i].pos.x + 4 + (walkers[i].direction * 5)) >> 4, walkers[i].pos.y >> 4)
        && gridGetSolid((walkers[i].pos.x + 4 + (walkers[i].direction * 5)) >> 4, (walkers[i].pos.y >> 4) + 1))
        {
          walkers[i].pos.x += walkers[i].direction;
        }
        else
        {
          walkers[i].direction = -walkers[i].direction;
        }
      }
      
      sprites.drawPlusMask(walkers[i].pos.x - cam.pos.x + walkers[i].hurt, walkers[i].pos.y - cam.pos.y, sprWalker, walkerFrame);
    }
  }
}


#endif
