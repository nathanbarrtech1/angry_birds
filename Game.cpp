/*
Author: Nathan Barr
Class: ECE4122
Last Date Modified: 10/24/21

Description:
The implementation for the Game class that controls the actual game like setting up
The animals in the correct column, creating buzzy and the bee, setting the text,
and collision detection
*/

#include "Game.h"
#include "Sprite.h"
#include <iostream>
#include "time.h"
#include <string>
#include <math.h>
#include "_rotRect.h"

#define GND			80
#define WIN_H		1080
#define WIN_W		1920
#define LIFE_MAX	5
#define PI			3.14159265359

using namespace std;

Game::Game(sf::RenderWindow * w)
{
	window = w;			// Sets the window for drawing
	srand(time(0));		// Sets the random seed

	buzzy = Buzzy();	// Initializes buzzy
	bee = Insect();		// initializes the insect
	
	// Creates the Sprites for the lives and sets their position
	for (int i = 0; i < LIFE_MAX; i++)
	{
		Sprite life("Buzzy Life", LIFE);
		life.setSFPos(make_pair(110 + 60 * i, 0));
		lifeSprites[i] = life;
	}

	// Creates the power bar
	sf::RectangleShape powBar(sf::Vector2f(400, 30));
	powBar.setFillColor(sf::Color::Transparent);
	powBar.setOutlineThickness(5);
	powBar.setOutlineColor(sf::Color::Black);
	powBar.setPosition(175, 1000);

	// Creates the power bar fill
	sf::RectangleShape powBarFill(sf::Vector2f(100, 30));
	powBarFill.setOutlineThickness(-1);
	powBarFill.setOutlineColor(sf::Color::Blue);
	powBarFill.setFillColor(sf::Color::Red);
	powBarFill.setPosition(175, 1000);
	powBarFill.setSize(sf::Vector2f(400 * buzzy.getPow(), 30));

	powerBar = powBar;
	powerBarFill = powBarFill;

	lives = LIFE_MAX;	// Sets the lives to the max value
	score = 0;			// Sets the score to 0
	hit = false;		// Set bee being hit to false
}

void Game::createLevel(std::string sprites[], int numAnimals)
{
	bee = Insect();	// Creates a new insect object
	hit = false;	// Sets the bee being hit to false
	buzzy.resetBuzzy();	// Resets buzzy
	randomSprites.clear(); // Clears the randomSprites vector of all animals

	// Creates a string for the positions of all wildlife
	std::string randSeq = "";
	for (int i = 0; i < numAnimals - animalsInCols.size(); i++)
		randSeq += i;

	// Creates a string for the positions of all mascots
	std::string mascots = "";
	for (int i = 0; i < animalsInCols.size(); i++)
		mascots += (i + numAnimals - animalsInCols.size());

	// Randomizes the wildlife and mascot positions by putting a random position to the end 100 times (i.e. 1234, 2 -> 1342)
	for (int i = 0; i < 100; i++)
	{
		int random = rand() % (numAnimals - animalsInCols.size());
		randSeq = randSeq.substr(0, random) + randSeq.substr(random + 1) + randSeq[random];

		random = rand() % animalsInCols.size();
		mascots = mascots.substr(0, random) + mascots.substr(random + 1) + mascots[random];
	}

	//Puts one randomized mascot into a random spot in each column
	for (int i = animalsInCols.size() - 1; i >= 0; i--)
	{
		int random = rand() % (animalsInCols[i] - 1);
		randSeq = randSeq.substr(0, animalsInCols[i] * i + random) + mascots[i] + randSeq.substr(animalsInCols[i] * i + random);
	}

	int j = 0;	// The column of the current sprite
	int sum = animalsInCols[0];	// The sum is the sum of the animals from the 0th to jth columns
	for (int i = 0; i < randSeq.size(); i++)	// Loops through the entire the positions of all animals
	{
		// Sets the column of the current animal
		while (i - sum >= 0)
		{
			sum += animalsInCols[j];
			j++;
		}

		Sprite temp(sprites[randSeq[i]], WILDLIFE);	// Creates a sprite for pushing into randomSprites
		randomSprites.push_back(temp);	// Pushes the new sprites in the vector
		randomSprites[i].sprite.setScale(1.3, 1.3);	// Sets the sprites scale
		if (randSeq[i] >= numAnimals - animalsInCols.size())	// Occurs when the animal is a mascot
			randomSprites[i].setRole(MASCOT);	// Sets the animals role to mascot
		randomSprites[i].setCol(j); // sets the column to j
	}
}

void Game::menu(std::string sprites[])
{
	setAllPos();	// Sets the position of all animals
	lives = 5;		// Resets the lives

	// Creates the starting text
	sf::Text title("Buzzy's Revenge", font, 80);
	sf::Text dir("Press Enter to Restart Game\nPress Esc to exit\nPress Space to Powerup\nPress up/down arrow to aim", font, 50);
	sf::Text author("created by\nNathan Barr", font, 25);

	// Sets the position of the starting text
	title.setPosition((*window).getSize().x / 2 - title.getLocalBounds().width / 2, 150);
	dir.setPosition((*window).getSize().x / 2 - dir.getLocalBounds().width / 2, 275);
	author.setPosition((*window).getSize().x / 2 - author.getLocalBounds().width / 2, 750);

	title.setFillColor(sf::Color::Red);	// Sets the color of the title to red

	// Draws the starting text
	(*window).draw(title);
	(*window).draw(dir);
	(*window).draw(author);

	printPower();
}

void Game::printLives()
{
	// Creates the lives text
	sf::Text lifeTxt("Lives", font, 25);
	lifeTxt.setPosition(20, 20);

	// Prints all lives remaining
	for (int i = 0; i < lives; i++)
	{
		(*window).draw(lifeSprites[i].sprite);
	}

	// Draws the lives text
	(*window).draw(lifeTxt);
}

void Game::printPower()
{
	// Creates the power text
	sf::Text powerTxt("Power", font, 25);
	powerTxt.setPosition(50, 1000);

	// Sets the size of the power bar
	powerBarFill.setSize(sf::Vector2f(400 * buzzy.getPow(), 30));

	// Draw the power bar, txt, and fill
	(*window).draw(powerBar);
	(*window).draw(powerBarFill);
	(*window).draw(powerTxt);
}

void Game::printScore()
{
	// Creates the score text (including the score value)
	sf::Text scoreTxt("Score: " + to_string(score), font, 25);
	scoreTxt.setPosition((*window).getSize().x - 200, 20);

	// Draws the score text
	(*window).draw(scoreTxt);
}

void Game::delCol()
{
	animalsInCols.erase(animalsInCols.begin() + colToDel);	// Deletes the current column to be deleted
	
	// Loops through all animals and deletes them if their col == colToDel
	int i = 0;
	while (i < randomSprites.size())
	{
		if (randomSprites[i].getCol() == colToDel)
			randomSprites.erase(randomSprites.begin() + i);
		else
			i++;
	}
}

Role Game::collision()
{
	_RotRect buzzyRect(buzzy);	// Creates a rectangle for buzzy for collision detection
	_RotRect beeRect(bee);		// Creates a rectangle for bee for collision detection
	
	if (buzzy.outOfBounds())	// If buzzy is out of bounds
	{
		lives--;	// decrease lives
		return WALL;
	}
	else if (_RotRect::check_two_rotated_rects_intersect(&buzzyRect, &beeRect))	// If buzzy hits bee
	{
		hit = true;							// Set bee hit to true
		bee.sprite.setPosition(-100, -100);	// Move bee off screen
		score += 75;						// Add 75 points
		return INSECT;
	}
	else
	{
		for (int i = 0; i < randomSprites.size(); i++)	// Loops through all animals
		{
			_RotRect rect(randomSprites[i]);	// Creates a collision rectangle for the current animal
			if (_RotRect::check_two_rotated_rects_intersect(&buzzyRect, &rect))	// If buzzy hits current animal
			{
				if (randomSprites[i].getRole() == MASCOT)	// If the animal hit is a mascot
				{
					score += 25;							// Add 25 points
					colToDel = randomSprites[i].getCol();	// Set the column to be deleted
					return MASCOT;
				}
				else if (randomSprites[i].getName() == "angry_unicorn")	// If the animal hit is the unicorn
				{
					if (lives < LIFE_MAX)	// Adds lives if less than the maximum
						lives++;	

					animalsInCols[randomSprites[i].getCol()]--;		// Decreases animals in unicorn's column by one
					randomSprites.erase(randomSprites.begin() + i);	// Deletes the unicorn
					setAllPos();	// Sets the positions of all animals after unicorn is deleted
					return UNICORN;
				}
				else
				{
					lives--;	// If a wildlife animal is hit, decrease live
					return WILDLIFE;
				}
			}
		}
	}
	return NONE;	// Occurs when nothing is hit
}

void Game::drawSprites()
{
	(*window).draw(buzzy.sprite);	// Draws buzzy
	(*window).draw(bee.sprite);		// Draws bee

	// Draws all animals
	for (int i = 0; i < randomSprites.size(); i++)
	{
		(*window).draw(randomSprites[i].sprite);
	}
}

void Game::setAllPos()
{
    int sepDist = 10;	// Min distance between animals in a column
	std::pair<int, int> spritesPos = std::make_pair(1800, WIN_H - GND);	// Sets the position of the bottom sprite

	int dist = 120;	// distance between column centers
	for (int i = 0; i < randomSprites.size(); i++)	// Loops through all animals
	{
		if (i == 0 || randomSprites[i].getCol() != randomSprites[i - 1].getCol())	// Resets spritesPos if a new column is reached
			spritesPos.second = WIN_H - GND;

		spritesPos.first = (WIN_W - (dist + 200 * randomSprites[i].getCol())) - randomSprites[i].getW() / 2;	// Sets the x position of the sprite
		spritesPos.second -= randomSprites[i].getH() + sepDist;													// Sets the y position of the sprite
		randomSprites[i].setSFPos(std::make_pair(spritesPos.first, spritesPos.second));							// Sets the sf::position and position of the sprite
	}
}