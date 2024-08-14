/*
Author: Nathan Barr
Class: ECE4122
Last Date Modified: 10/24/21

Description:
The header for the Sprite class (and its subclasses) whose purpose is the 
control the pos, vel, and sf::sprite of each Sprite in the game
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <math.h>

#define H		10.8	// Defines the height of the game screen
#define W		19.2	// Defines the width of the game screen

// Enumeration for defining what role an object in the game takes
enum Role
{
	MASCOT,
	BUZZY,
	INSECT,
	WILDLIFE,
	LIFE,
	WALL,
	BACKGROUND,
	UNICORN,
	NONE
};

// Superclass for each sprite object that exists that allow for manipulation of its sf::sprite
// and includes functions for changing the role, position, velocity, etc
class Sprite
{
public:
	Sprite() {}										// Base constructor for use in game constructor
	Sprite(std::string, Role);						// Sprite constructor for all others to initialize the sprite according to its name *in the graphics folder" and its role									
	float getAngle() { return angle; }				// Returns the current angle of the sprite
	float getW() const { return sprite.getScale().x * sprite.getLocalBounds().width; }		// Returns the width of the sprite
	float getH() const { return sprite.getScale().y * sprite.getLocalBounds().height; }		// Returns the height of the sprite
	bool outOfBounds() const { return pos.first > W || pos.second < 0; }					// Returns whether the sprite is currently out of bounds
	virtual void setSFPos(std::pair<double, double>);	// Sets the position of the sf::sprite based on a new position (this pos is based off of sf::pos and not Sprite::pos) and the pos
	void setAngle(float a);							// Sets the angle of the sprite
	void setRole(Role s) { role = s; }				// Sets the role of the sprite
	Role getRole() const { return role; }			// returns the role of the sprite
	void setCol(int c) { col = c; }					// Sets the col the sprite is currently in
	int getCol() const { return col; }				// Returns the column that the sprite is currently in						
	std::string getName() const { return name; }	// Gets the name of the sprite
	void fall();									// The sequence that make the sprite fall offscreen
	sf::Sprite sprite;								// The sf::sprite
protected:
	void setSFPos();								// Sets the position of the sf::sprite based on the current position in the game
	std::pair<double, double> pos;					// The position of the sprite on the game screen (not the sf::position)
	std::pair<double, double> vel;					// The velocity of the sprite (insect and buzzy)
	std::string name;								// The name of the sprite
	float angle;									// The angle the sprite is facing
	Role role;										// The role of the sprite
private:
	int col;										// The column of the current sprite (not applicable for buzzy and insect)
};

// The Sprite subclass for Buzzy, mainly focuses on the use of projectile
// motion unique to buzzy
class Buzzy : public Sprite
{
public:
	Buzzy();										//  Base constructor for buzzy
	void update(double);							// The function for projectile motion, updates the position and vel every frame									
	void incAngle(bool);							// Increments the angle on up(true) press or down(false) press
	void setInitVel(double, double);				// Sets the initial velocity of buzzy
	void incPower();								// Increments buzzy's power (0-100)
	void setPow(int p) { power = p; }				// Sets the power, used for setting buzzy's power to 0 only
	void resetBuzzy();								// Resets buzzy back to his initial position for another launch
	float getPow() const { return power; }			// Gets the power
	void setSFPos(std::pair<double, double> p);		// Sets the sf::pos along with pos and initialPos
private:
	std::pair<double, double> initialPos;			// The initial pos of the the sprite for update calculations (projectile motion)
	std::pair<double, double> initialVel;			// The initial vel of the the sprite for update calculations (projectile motion)
	float power;									// The power (0-100) that Buzzy is launched at
	float launchAngle;								// Buzzy's launch angle, used for resetting Buzzy and projectile motion calculations
};

// The Sprite subclass for the insect(bee),
// makes the insect fly from left to right and 
// vice versa at random speeds
class Insect : public Sprite
{
public:
	Insect();				// Constructor for insect
	void update(double);	// Updates the insect's position
};