// Rotated Rectangles Collision Detection, Oren Becker, 2001
// Modified to include use of Sprites for _RotRect construction

#pragma once
#include "Sprite.h"

struct _Vector2D
{
	float x, y;
};

// C:center; S: size (w,h); ang: in radians, 
// rotate the plane by [-ang] to make the second rectangle axis in C aligned (vertical)
struct _RotRect
{
	_RotRect(Sprite);
	_Vector2D C;
	_Vector2D S;
	float ang;
	static bool check_two_rotated_rects_intersect(_RotRect *, _RotRect *);
private:
	static void SubVectors2D(_Vector2D *, _Vector2D *);
	static void AddVectors2D(_Vector2D *, _Vector2D *);
	static void RotateVector2DClockwise(_Vector2D *, float);
};