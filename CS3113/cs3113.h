#ifndef CS3113_H
#define CS3113_H
#define LOG(argument) std::cout << argument << '\n'

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <iostream>

enum AppStatus  { TERMINATED, RUNNING };
enum TextureType { SINGLE, ATLAS };

Color     ColorFromHex(const char *hex);
float     GetLength(const Vector2 vector);
void      Normalise(Vector2 *vector);
Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols);
void      panCamera(Camera2D *camera, const Vector2 *targetPosition);

#endif // CS3113_H