#include "pch.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "EulerFluid.h"
#include "VectorField.h"

using namespace std;

// Based on the paper writted by Jos Stam, "Real-Time Fluid Dynamics for Games" 
// https://pdfs.semanticscholar.org/847f/819a4ea14bd789aca8bc88e85e906cfc657c.pdf

bool containedInWindow(int, int, int, int);

int main()
{
	// Smoke Particle Size
	int particleSize = 4;

	// test area	

	// Window Attributes
	int framerate = 100;

	int windowX = 90;
	int windowY = 90;
	sf::RenderWindow window(sf::VideoMode((windowX+2)*particleSize, (windowY+2)*particleSize), "Euler Fluid");
	window.setFramerateLimit(framerate);
	//window.setKeyRepeatEnabled(false);

	// Fluid Attributes
	float dt = (float)1 / framerate;

	// Mouse (Source) Information
	int mouseX = 0;
	int mouseY = 0;
	int prevMouseX = 0;
	int prevMouseY = 0;

	// Rendering vars
	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;
	srand(time(NULL));
	//cout << rand() << " " << rand();
	//auto color = sf::Color(rand() % 240, rand() % 240, rand() % 240);
	image.create((windowX + 2)*particleSize, (windowY + 2)*particleSize, sf::Color::Black);

	//Euler Fluid
	EulerFluid fluid(windowX, windowY, dt, particleSize);

	// Open Window
	while (window.isOpen()) {

		// Deals with closing window
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Single Euler Fluid simulation frame
		prevMouseX = mouseX;
		prevMouseY = mouseY;

		// If mouse is moving, find new positionn relative to window
		sf::Vector2i position = sf::Mouse::getPosition(window);
		mouseX = position.x;
		mouseY = position.y;

		// There is now a difference dx between mouseX and prevMouseX and same for Y
		int dx = mouseX - prevMouseX;
		int dy = mouseY - prevMouseY;

		// Add source if left button is pressed, random color!
		bool changedFluid = true;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && containedInWindow(window.getSize().x, window.getSize().y, mouseX, mouseY)) {
			
			// Turn on fluid generation
			fluid.setAntiFluid(false);

			// Add source
			fluid.addSource(mouseX, mouseY, dx, dy);

		}
		
		// Don't add source, only velocity vectors
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && containedInWindow(window.getSize().x, window.getSize().y, mouseX, mouseY)) {


			// Turn off fluid generation
			fluid.setAntiFluid(true);


			// alter velocity fields but not density fields
			fluid.addSource(mouseX, mouseY, dx, dy);

		}
		
		// Recolor fluid (random color!)
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && containedInWindow(window.getSize().x, window.getSize().y, mouseX, mouseY)) {
			fluid.setColor(rand() % 240, rand() % 240, rand() % 240);
		}

		// Reset fluid
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && containedInWindow(window.getSize().x, window.getSize().y, mouseX, mouseY)) {

			// Reset fluid fields
			fluid.reset();

			// Clear window
			for (int i = 0; i < (windowX + 2)*particleSize; i++) {
				for (int j = 0; j < (windowY + 2) * particleSize; j++) {
					image.setPixel(i, j, sf::Color::Black);
				}
			}

		}

		//update fluid
		fluid.update();

		// render fluid on window
		fluid.render(particleSize, image);

		// load texture from image
		texture.loadFromImage(image);

		// set sprite texture
		sprite.setTexture(texture, true);

		// draw sprite to window
		window.draw(sprite);

		// display window
		window.display();

	}

	return 0;
}

// Determines if cursor is within window
bool containedInWindow(int xSize, int  ySize, int mouseX, int mouseY) {

	if (mouseX > 0 && mouseX < xSize && mouseY > 0 && mouseY < ySize) {
		return true;
	}
	else {
		return false;
	}

}

