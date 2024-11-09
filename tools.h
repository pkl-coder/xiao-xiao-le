#pragma once
#include <graphics.h>

void putimagePNG(int  picture_x, int picture_y, IMAGE* picture);
void putimageTMD(int  picture_x, int picture_y, IMAGE* picture, int tmd);
int getDelay();
void setFont(const char* fontName, int fontWidth, int fontHeight);
void  preLoadSound(const char* name);
void  playSound(const char* name);