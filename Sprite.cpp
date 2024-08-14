/*
Author: Nathan Barr
Class: ECE4122
Last Date Modified: 10/24/21

Description:
The implementation for the Sprite class (and its subclasses) whose purpose is the
control the pos, vel, and sf::sprite of each Sprite in the game
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Sprite.h"
#include <string>
#include <math.h>

#define PI			3.14159265359	// PI
#define G		   -9.8		// Gravity
#define H			10.8	// The custom height of the game screen
#define W			19.2	// The custom width of the game screen
#define WIN_H		1080	// The height of the window in pixels
#define WIN_W		1920	// The width of the window in pixels
#define GND			80		// The distance from the bottom to the 'ground' in pixels
#define SPRITE_W	100		// Sets the width of the animal sprite

Sprite::Sprite(std::string s, Role r)
{
	// Creates the sf::sprite and sets its texture
	sf::Texture * texture = new sf::Texture;	// Needs the new keyword or else the texture is overwritten by the latest sprite
	(*texture).loadFromFile("graphics\\" + s + ".png");
	sprite.setTexture(*texture);

	// Initializes the sprite values
	pos = std::make_pair(0, 0);
	vel = std::make_pair(0, 0);
	angle = 0;
	name = s;
	role = r;
	sprite.setOrigin(0, 0);
}
void Sprite::setAngle(float a) 
{ 
	angle = a;					// Sets the Sprites angle
	sprite.setRotation(-1 * a); // Sets the sf::sprites rotation according to the angle
}

void Sprite::setSFPos() 
{ 
	int x = pos.first * WIN_W / W;					// Converts from game position to window position for x
	int y = (H - pos.second) * (WIN_H - GND) / H;	// Converts from game position to window position for y

	sprite.setPosition(x, y);						// Sets the sf::position
}

void Sprite::setSFPos(std::pair<double, double> p)
{
	pos.first = p.first * W / WIN_W;				// Converts from window position to game position for y
	pos.second = H - p.second * H / (WIN_H - GND);	// Converts from window position to game position for y
	setSFPos();										// Sets the sf::position
}

void Sprite::fall()
{
	sprite.setOrigin(getW() / 2, getH() / 2);		// Sets the origin of the sprite to the center for spinning
	pos.second -= 0.005;							// Incrementally decreases the y position
	setSFPos();										// Sets the sf::position
	angle += 0.5;									// Incrementally increases the angle
	sprite.setRotation(-1 * angle);					// Sets the sf::sprites rotation according to the angle
}

Buzzy::Buzzy() : Sprite("smallBuzzy", BUZZY)
{
	setAngle(45);									// Sets the starting angle to 45 degrees
	launchAngle = angle;							// Sets the launch angle to the current angle
	sprite.setOrigin(26, 66);						// Sets the origin to the tail of buzzy
	setSFPos(std::make_pair(0, 660));				// Sets the sf::position
	power = 0.6;									// Sets the initial power
}

void Buzzy::update(double time)
{
	// Calculates the vel_x where pos_x = pos_x + v_ix * t
	pos.first = initialPos.first + initialVel.first * time;

	// Calculates the pos_y where pos_y = pos_iy + v_ix * t + G * t^2
	pos.second = initialPos.second + initialVel.second * time + G * time * time / 2;
	Sprite::setSFPos(); // Sets the sf::position

	// Sets the v_y where v_y = v_iy + G * t
	vel.second = initialVel.second + G * time;

	// Calculates the angle and converts it from radians to degrees
	int a = 180 / PI * atan(vel.second / vel.first);

	sprite.setRotation(-1 * a); // Sets the sf::sprites rotation according to the angle
}

void Buzzy::incAngle(bool up)
{
	if (up && angle < 89)		// Increases the angle if up is pressed
		angle += 0.035;
	else if (!up && angle > 1)	// Decreases the angle if down is pressed
		angle -= 0.035;

	sprite.setRotation(-1 * angle); // Sets the sf::sprites rotation according to the angle
}

void Buzzy::incPower()
{
	power += 0.00035;	// Increments the power
	if (power > 1)		// Loops the power from 0-100
		power = 0;
}

void Buzzy::setInitVel(double max, double min)
{
	double t = (max - min) * power + min; // Sets the time taken to cross the screen according the the power and min, max times
	initialVel.first = W / t;	// Sets vel_ix
	initialVel.second = initialVel.first * tan(angle * PI / 180); // Sets vel_iy where vel_iy = vel_x * tan(angle);
	vel = initialVel;	// vel_x always = vel_ix
	launchAngle = angle;	// Sets the launch angle to the current angle
}

void Buzzy::resetBuzzy()
{
	sprite.setOrigin(26, 66);			// Resets Buzzy's origin to his tail
	setSFPos(std::make_pair(0, 660));	// Resets the position
	setAngle(launchAngle);				// Resets the angle to the launch position
}

void Buzzy::setSFPos(std::pair<double, double> p)
{
	initialPos.first = p.first * W / WIN_W;					// Converts from window position to game position for x
	initialPos.second = H - p.second * H / (WIN_H - GND);	// Converts from window position to game position for y
	pos = initialPos;	// Sets the position
	Sprite::setSFPos();	// sets the sf::position
}

Insect::Insect() : Sprite("insect", INSECT)
{
	setSFPos(std::make_pair(rand() % (WIN_W - 200) + 100, rand() % 200 + 50));	// Sets the insect's sf::position to a random spot from (100 to 980, 50 to 250)
	sprite.setScale(1.3, 1.3);	// Sets the scale of the insect
	vel.first = 3;				// Sets the velocity of the insect
}

void Insect::update(double time)
{
	static int dir = 1;						// Insect starts moving to the right
	if (pos.first > (W - 1))				// Occurs if the insect hits right side
	{
		dir = -1;							// Switches direction
		vel.first = abs(vel.first) * -1;	// Negates the velocity
		sprite.setScale(abs(sprite.getScale().x) * -1, sprite.getScale().y);	// Changes the orientation of the insect
	}
	else if (pos.first < 1)					// Occurs if the insect hits left side
	{
		dir = 1;							// Switches direction
		vel.first = abs(vel.first);			// Makes the velocity positive
		sprite.setScale(abs(sprite.getScale().x), sprite.getScale().y);			// Changes the orientation of the insect
	}

	if (vel.first < 0)						// Occcurs when the velocity is negative
		vel.first = -1 * (rand() % 8 + 3);	// Sets the velocity to a random negative velocity
	else									// Occcurs when the velocity is positive
		vel.first = rand() % 8;				// Sets the velocity to a random positive velocity
	
	if (abs(angle) > 30)					// Occurs if the angle is steeper than 30 degrees
		angle += 0.1 * dir;					// Makes the angle less steep
	else if (pos.second <= 8)				// Occurs when the insect tries to leave the bottom bounds
		angle = 1;							// Forces the insect to move up
	else if (pos.second >= 10)				// Occurs when the insect tries to leave the top bounds
		angle = -1;							// Forces the bee to move down
	
	// Otherwise, randomly increments the angle
	else if (rand() % 2)
		angle += 0.1 * dir;
	else
		angle -= 0.1 * dir;

	// Loops the angle
	if (angle >= 360)						
		angle -= 360;
	else if (angle <= -360)
		angle += 360;

	// Sets the vel_x where vel_x = vel_x * tan(angle) in the correct direction
	vel.second = dir * vel.first * tan(angle * PI / 180);

	// Increments the position (time should be reset after every time this function is called)
	pos.first += vel.first * time;
	pos.second += vel.second * time;

	sprite.setRotation(-1 * dir * angle); // Sets the sf::sprites rotation according to the angle and direction
	setSFPos();	// set the sf::position
}