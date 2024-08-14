/*
Author: Nathan Barr
Class: ECE4122
Last Date Modified: 10/24/21

Description:
The main class containing the game loop. Determines which parts of the game the
player is at, what part to go to next, controls the timer, updates buzzy,
and inputs player values such as the power
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include "Game.h"
#include <iostream>
#include "time.h"

#define WIN_H	1080
#define WIN_W	1920
#define H		10.8
#define W		19.2

// Enum for the current state the player is at
enum State
{
	START, RELOAD, MENU, SET_VALUES, CHARGED, LAUNCH, BUZZYFALL, COLFALL
};

int main()
{
	// Creates the window
	sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H), "Buzzy's Revenge");
	
	// Array of all animal names
	std::string sprites[10] =
	{
		"angry_unicorn",
		"bunny",
		"chicken",
		"dog",
		"frog",
		"mouse",
		"pig",
		"sheep",
		"Georgia_Bulldogs_logo_dog_g",
		"tiger"
	};

	// Initializes a Game object
	Game game(&window);

	// Sets Game font
	game.setFont("fonts\\KOMIKAP_.ttf");

	// Creates a sprite for the background
	Sprite background("background", BACKGROUND);

	// Creates timers for buzzy and bee
	clock_t buzzyTime;
	clock_t beeTime;

	// Sets the initial state
	State state = START;

	// Boolean for whether the power has been determined
	bool power = false;

	while (window.isOpen())
	{
		window.clear();					// Clears the window for a new frame
		window.draw(background.sprite);	// Draws the background every frame
		game.drawSprites();	// Draw sprites
		game.printLives();	// Prints the remaining lives
		game.printScore();	// prints the score

		// Makes sure every event is processed
		sf::Event event;				
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (state == START)	// Occurs at the start of the game and when the player has lost
		{
			power = false;
			// Sets up the columns
			game.clearCols();
			game.addCol(5);
			game.addCol(5);

			// Creates a new level
			game.createLevel(sprites, sizeof(sprites) / sizeof(sprites[0])); 
			state = MENU;	// Goes to next state
		}
		else if (state == MENU)	// Only occurs after start
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))		// Occurs when Esc is pressed
			{
				window.close();	// Closes the window
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))	// Occurs when Enter is pressed
			{
				state = SET_VALUES;	// Goes to buzzy angle and power calculations
				game.resetScore();	// Resets the score
				beeTime = clock();	// Initializes the bee timer
			}
			game.menu(sprites);		// Prints starting text
		}
		else if (state == RELOAD)	// Occurs if the player hit the mascots in every column
		{
			// Sets up the columns
			game.clearCols();
			game.addCol(5);
			game.addCol(5);

			// Creates a new level
			game.createLevel(sprites, sizeof(sprites) / sizeof(sprites[0]));

			// Sets the position of all animal sprites
			game.setAllPos();

			// Resets bee timer
			beeTime = clock();

			// Goes to buzzy angle and power calculations
			state = SET_VALUES;	
		}
		else
		{
			// Updates the bee as long as it is not hit
			if (!game.getHit())
			{
				game.bee.update(difftime(clock(), beeTime) / 1000.);
				beeTime = clock();	// Reset bee timer
			}

			if (state == SET_VALUES)
			{
				game.printPower();	// Prints the power bar
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))	// Occurs when Space is hit
				{
					// Resets power bar once each time power is calculated
					if (!power)	
						game.buzzy.setPow(0);
					power = true;

					// Increments the power
					game.buzzy.incPower();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Down))	// Occurs when up is pressed and down isnt
				{
					game.buzzy.incAngle(true);	// Increases the angle
				}

				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Up))	// Occurs when down is pressed and up isnt
				{
					game.buzzy.incAngle(false);	// Decreases the angle
				}
				else if (power)	// if power is hit and Space is not
				{
					state = CHARGED;	//	go to next state
				}

			}
			else if (state == CHARGED)	// Occurs only after buzzy angle and power calculations
			{
				game.buzzy.setInitVel(1, 4);	// Sets the initial velocity
				state = LAUNCH;					// Goes to next state
				buzzyTime = clock();			// Resets buzzy timer
			}
			else if (state == LAUNCH)	// Occurs after the power bar is charged
			{
				game.buzzy.update(difftime(clock(), buzzyTime) / 1000);	// updates buzzy
				Role role = game.collision();	// Calculate collision
				
				if (game.getLives() == 0)	// If all lives are gone
				{
					state = START;			// Go back to the start
				}
				else if (role == WILDLIFE)		// If buzzy collided with a wildlife animal
				{
					state = BUZZYFALL;
				}
				else if (role == MASCOT)	// If buzzy collided with a mascot
				{
					state = COLFALL;
				}
				else if (role != NONE)		// If buzzy collided with something else
				{
					game.buzzy.resetBuzzy();
					state = SET_VALUES;
					power = false;
				}
			}
			else if (state == BUZZYFALL)	// Makes buzzy fall, occurs after hitting wildlife
			{
				game.buzzy.fall();	// Decreases buzzy position and increases angle
				if (game.buzzy.outOfBounds())	// If buzzy has fallen off the screen,
				{
					game.buzzy.resetBuzzy();	// Reset buzzy
					state = SET_VALUES;			// Go back to angle and power calculations
					power = false;				// power pressed set to false
				}
				
			}
			else if (state == COLFALL)	// Makes the animals in a column fall, occurs after hitting a mascot
			{
				game.buzzy.resetBuzzy();	// Reset buzzy
				int count = 0;				// Counts the animals that have fallen out of bounds
				for (int i = 0; i < game.randomSprites.size(); i++)	// Loops through all animals
				{
					if (game.randomSprites[i].getCol() == game.getColToDel())	// If the animal is in a column to be deleted,
					{
						game.randomSprites[i].fall();	// Make it fall
						if (game.randomSprites[i].sprite.getPosition().y >= WIN_H + 200)	// If it has fallen offscreen
						{
							count++;	// Increases the number of animals that have fallen offscreen
						}
					}
				}
				if (count == game.getAnimalsInCol(game.getColToDel()))	// If all animals in the column have fallen offscreen
				{
					state = SET_VALUES;				// Go to buzzy angle and power calculations
					power = false;					// power pressed set to false
					game.delCol();					// Deletes the column and the animal to be deleted
                    if (game.getNumAnimals() == 0)	// If all animals are gone,
						state = RELOAD;				// Create a new level
				}
				
			}
		}
		window.display();	// Display all draws for the current frame
	}

	return 0;
}