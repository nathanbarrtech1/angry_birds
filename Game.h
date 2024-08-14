/*
Author: Nathan Barr
Class: ECE4122
Last Date Modified: 10/24/21

Description:
The header for the Game class that controls the actual game like setting up
The animals in the correct column, creating buzzy and the bee, setting the text,
and collision detection
*/

#pragma once
#include <string>
#include "Sprite.h"

#define LIFE_MAX 5	// The the max and initial life count


// The Game class that controls the actual game like setting up
// The animals in the correct column, creating buzzy and the bee, setting the text,
// and collision detection
class Game
{
public:
	Game(sf::RenderWindow *);			// The base constructor for game class
	Buzzy buzzy;						// Declares an instance of buzzy
	Insect bee;							// Declares an instance of insect
	std::vector<Sprite> randomSprites;	// Declares the vector for the sprites in the columns

	void addCol(int numOfAnimals) { animalsInCols.push_back(numOfAnimals); }	// Adds a column to the game
	void setFont(std::string f) { font.loadFromFile(f); }		// Sets the font of the text in the game
	void clearCols() { animalsInCols.clear(); }					// Clears all columns
	void delCol();						// Deletes the animals in one column and the column itself
	void menu(std::string sprites[]);	// Creates the starting screen
	void printLives();					// Prints the remaining lives
	void printPower();					// Prints the power bar
	void printScore();					// Prints the score
	Role collision();					// Determines whether buzzy has collided with an object
	void createLevel(std::string [], int);						// Creates a new level
	float getLives() const { return lives; }					// Gets the current number of remaining lives
	bool getHit() const { return hit; }	// Gets whether bee was hit or not
	void drawSprites();					// Draws all the sprites
	int getNumAnimals() { return randomSprites.size(); }		// Gets the remaining number of animals in the columns
	int getAnimalsInCol(int i) { return animalsInCols[i]; }		// Gets the remaining number of animals in a specific column
	void setAllPos();					// Sets the position of the animals in the columns
	int getColToDel() const { return colToDel; }				// Gets the column to be deleted after collision with a mascot
	void resetScore() { score = 0; }	// Resets the score

private:
	Sprite lifeSprites[LIFE_MAX];		// an array for the life sprites
	sf::RectangleShape powerBar;		// The power bar
	sf::RectangleShape powerBarFill;	// The power bar outline
	sf::RenderWindow * window;			// The render window for drawing
	std::vector<int> animalsInCols;		// A vector for number of animals in each column
	sf::Font font;						// The text font
	int score;							// The game score
	float lives;						// The number of lives remaining
	bool hit;							// Bool for whether the bee has been hit
	int colToDel;						// The column to be deleted after being hit by a mascot
	
};