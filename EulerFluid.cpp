#include "pch.h"
#include "EulerFluid.h"

using namespace std;

EulerFluid::EulerFluid()
{
}

EulerFluid::EulerFluid(int xSize, int ySize, float dt, int particleSize) {

	// Instantiate basic Fluid attributes
	this->dt = dt;
	this->xSize = xSize;
	this->ySize = ySize;
	this->particleSize = particleSize;

	// Default Cyan fluid color
	this->color = sf::Color::Cyan;

	// Instantiate VectorField attributes
	xVel.assignSize(xSize, ySize);
	yVel.assignSize(xSize, ySize);
	density.assignSize(xSize, ySize);
	prevXVel.assignSize(xSize, ySize);
	prevYVel.assignSize(xSize, ySize);
	prevDensity.assignSize(xSize, ySize);
	xVelSource.assignSize(xSize, ySize);
	yVelSource.assignSize(xSize, ySize);
	densitySource.assignSize(xSize, ySize);
	div.assignSize(xSize, ySize);
	p.assignSize(xSize, ySize);

}


EulerFluid::~EulerFluid()
{
}

// Adds area where mouse is pressed as a fluid source
void EulerFluid::addSource(int x, int y, int dx, int dy) {

	
	// Scales down mouse coordinates to particle coordinates
	x /= particleSize;
	y /= particleSize;
	dx /= particleSize;
	dy /= particleSize;

	// Digression from axis of higher velocity
	int digression = 0;

	// Higher x direction speed
	if (abs(dx) >= abs(dy)) {

		//cout << "Higher X velocity" << endl;

		// Calculate diffusivity constant
		digression = 3 * abs(dy) - abs(dx);

		// Move vertically by 1 particle
		int yStep = 1;
		if (dy < 0) {
			yStep = -yStep;
		}

		// Fill in velocity vector fields
		for (int i = 0; i < abs(dx); i++) {

			// Amount fluid trails from source
			int count = i;

			// If moving left, decrease count
			if (dx < 0) {
				count = -count;
			}

			// Edge cases
			if (x + count <= 0 || x+count >= (xSize) || y <= 0 || y >= (ySize)) {
				break;
			}

			// Update density and velocity fields
			if (!antiFluid) {
				densitySource.set(x + count, y, 1);
			}

			// Set velocities
			xVelSource.set(x+count, y, dx / dt);
			yVelSource.set(x+count, y, dy / dt);

			// Flow into y direction
			if (digression > 0) {
				y += yStep;
				digression -= 2 * abs(dx);

			}
			digression += 2 * abs(dy);

		}


	}

	// Higher y direction speed 
	else {
		//cout << "Higher Y velocity" << endl;

		// Calculate diffusivity constant
		digression = 3 * abs(dx) - abs(dy);

		// Move horizontally by 1 particle
		int xStep = 1;
		if (dx < 0) {
			xStep = -xStep;
		}

		// Fill in velocity vector fields
		for (int i = 0; i < abs(dy); i++) {

			// Amount fluid trails from source
			int count = i;

			// If moving down, change sign of count
			if (dy < 0) {
				count = -count;
			}

			// Edge cases
			if (y + count <= 0 || y + count >= (ySize) || x <= 0 || x >= (xSize)) {
				break;
			}

			//cout << "Index: (" << x + count << ", " << y << ")" << endl;

			// Update density and velocity fields
			if (!antiFluid) {
				densitySource.set(x, y+count, 1);
			}
			else {
				densitySource.set(x, y+count, 0);
			}

			// Set velocities
			xVelSource.set(x, y+count, dx / dt);
			yVelSource.set(x, y+count, dy / dt);

			// Flow into x direction
			if (digression > 0) {
				x += xStep;
				digression -= 2 * abs(dy);

			}
			digression += 2 * abs(dx);

		}


		


	}

	if (dx == 0 && dy == 0 && !antiFluid) {
		density.set(x, y, 1);
		//cout << "L" << endl;
	}



}

void EulerFluid::update() {

	// Itterate through velocity fields and set previous equal to the source, then set source equal to 0
	for (int i = 1; i <= xSize; i++) {
		for (int j = 1; j <= ySize; j++) {

			prevXVel.set(i, j, xVelSource.at(i, j));
			prevYVel.set(i, j, yVelSource.at(i, j));

			xVelSource.set(i, j, 0);
			yVelSource.set(i, j, 0);

		}
	}

	diffuseVelocity();

	// make velocity mass conserving
	projectVelocity();

	// swap values of previous x velocity and x velocity
	swapPointers(prevXVel.field, xVel.field);

	// swap values of previous y velocity and y velocity
	swapPointers(prevYVel.field, yVel.field);

	advectVelocity();

	// make velocity mass conserving
	projectVelocity();

	// go thorugh density field and do same as above
	for (int i = 1; i <= xSize; i++) {
		for (int j = 1; j <= ySize; j++) {

			prevDensity.set(i, j, densitySource.at(i, j) + prevDensity.at(i, j));

			densitySource.set(i, j, 0);

		}
	}

	// diffuseDensity
	diffuseDensity();

	// swap values of previous density and density
	swapPointers(prevDensity.field, density.field);

	advectDensity();

}

// Move velocities into neighboring cells
void EulerFluid::diffuseVelocity() {

	for (int i = 1; i <= xSize; i++) {
		for (int j = 1; j <= ySize; j++) {

			xVel.set(i, j, prevXVel.at(i, j) + viscosity*(.25*xVel.at(i-1, j) + .25*xVel.at(i+1, j) + .25*xVel.at(i, j-1) + .25*xVel.at(i, j+1) - prevXVel.at(i, j)));

			yVel.set(i, j, prevYVel.at(i, j) + viscosity * (.25*yVel.at(i - 1, j) + .25*yVel.at(i + 1, j) + .25*yVel.at(i, j - 1) + .25*yVel.at(i, j + 1) - prevYVel.at(i, j)));
		}
	}

}

// Move density into neighboring cells
void EulerFluid::diffuseDensity() {
	//cout << "l" << endl;
	for (int i = 1; i <= xSize; i++) {
		for (int j = 1; j <= ySize; j++) {

			// Only diffuse if not an antifluid
			if (!antiFluid) {
				density.set(i, j, prevDensity.at(i, j) + diffuseRate * (0.25*density.at(i - 1, j) + 0.25*density.at(i + 1, j) +
					0.25*density.at(i, j - 1) + 0.25*density.at(i, j + 1) - prevDensity.at(i, j)));
			}
		}
	}
}

// trace velocity field back in time
void EulerFluid::advectVelocity() {

	for (int x = 1; x <= xSize; x++) {
		for (int y = 1; y <= ySize; y++) {
			double ax = 0;
			double ay = 0;
			int ax0 = 0;
			int ay0 = 0;
			double s0 = 0;
			double s1 = 0;
			double t0 = 0;
			double t1 = 0;
			float dt0 = dt * (xSize);

			ax = x - dt0 * xVel.at(x, y);
			ay = y - dt0 * yVel.at(x, y);
			if (ax < 0.5) {
				ax = 0.5;
			}
			if (ax > xSize + 0.5) {
				ax = xSize + 0.5;
			}
			ax0 = (int)ax;

			if (ay < 0.5) {
				ay = 0.5;
			}
			if (ay > xSize + 0.5) {
				ay = xSize + 0.5;
			}
			ay0 = (int)ay;

			s1 = ax - ax0;
			s0 = 1 - s1;
			t1 = ay - ay0;
			t0 = 1 - t1;

			xVel.set(x, y, s0 * (t0*prevXVel.at(ax0, ay0) + t1 * prevXVel.at(ax0, ay0 + 1)) +
				s1 * (t0*prevXVel.at(ax0 + 1, ay0) + t1 * prevXVel.at(ax0 + 1, ay0 + 1)));
			yVel.set(x, y, s0 * (t0*prevYVel.at(ax0, ay0) + t1 * prevYVel.at(ax0, ay0 + 1)) +
				s1 * (t0*prevYVel.at(ax0 + 1, ay0) + t1 * prevYVel.at(ax0 + 1, ay0 + 1)));
		}
	}

}

// Trace density field backwards in time
void EulerFluid::advectDensity() {

	double ax = 0;
	double ay = 0;
	int ax0 = 0;
	int ay0 = 0;
	double s0 = 0;
	double s1 = 0;
	double t0 = 0;
	double t1 = 0;
	float dt0 = dt * (xSize);

	for (int x = 1; x <= xSize; x++) {
		for (int y = 1; y <= xSize; y++) {

			// ax is equal to x minus dt * xSize * xVelocity at x and y
			ax = x - dt0 * xVel.at(x, y);
			// ax is equal to y minus dt * xSize * yVelocity at x and y
			ay = y - dt0 * yVel.at(x, y);

			// Lower Bound Cap X
			if (ax < 0.5) {
				ax = 0.5;
			}
			// Upper Bound Cap X
			if (ax > (xSize + 2) - 2) {
				ax = (xSize + 2) - 2;
			}
			ax0 = (int)ax;


			// Lower Bound  Cap Y
			if (ay < 0.5) {
				ay = 0.5;
			}
			// Upper Bound Cap Y
			if (ay > (xSize + 2) - 1.5) {
				ay = (xSize + 2) - 1.5;
			}
			ay0 = (int)ay;
			

			// Distance between ax and floor(ax)
			s1 = ax - ax0;
			s0 = 1 - s1;
			// Distance between ay and floor(ay)
			t1 = ay - ay0;
			t0 = 1 - t1;

			// Advect backwards through time
			density.set(x, y, s0 * (t0*prevDensity.at(ax0, ay0) + t1 * prevDensity.at(ax0, ay0 + 1)) +
				s1 * (t0*prevDensity.at(ax0 + 1, ay0) + t1 * prevDensity.at(ax0 + 1, ay0 + 1)));
			
			

		}
	}

} 

// Make the velocity field mass conserving
void EulerFluid::projectVelocity() {

	float h = 1.0 / xSize;

	// Uses Hodge decomposition to solve for an incompressible field (poisson equation)
	for (int i = 1; i <= xSize; i++) {
		for (int j = 1; j <= ySize; j++) {

			div.set(i, j, -0.5*h*(xVel.at(i+1, j) - xVel.at(i-1, j) + yVel.at(i, j+1) - yVel.at(i, j-1)));

		}
	}

	for (int k = 0; k < 5; k++) {
		for (int i = 1; i <= xSize; i++) {
			for (int j = 1; j <= ySize; j++) {
				p.set(i, j, (div.at(i, j) + p.at(i-1, j) + p.at(i+1, j) + p.at(i, j-1) + p.at(i, j+1)) / 4);

			}
		}
	}

	for (int i = 1; i <= xSize; i++) {
		for (int j = 1; j <= ySize; j++) {

			xVel.set(i, j, xVel.at(i, j) - 0.5*(p.at(i + 1, j) - p.at(i - 1, j)) / h);
			yVel.set(i, j, yVel.at(i, j) - 0.5*(p.at(i, j+1) - p.at(i, j-1)) / h);

		}
	}

}

// Draw fluid to the screen
void EulerFluid::render(int size, sf::Image& image) {

	// Re write each indivdual pixel of window
	for (int x = 0; x < (xSize+2) - particleSize; x++) {

		for (int y = 0; y < ySize+2 - particleSize; y++) {

			if (density.at(x, y) > 1) {
				density.set(x, y, 1);
			}

			for (int i = 0; i <= particleSize; i++) {

				for (int j = 0; j <= particleSize; j++) {					

					image.setPixel(x*size + i, y*size + j, sf::Color(color.r * density.at(x, y), color.g * density.at(x, y), color.b * density.at(x, y)));

				}

			}

			prevDensity.set(x, y, density.at(x, y));
			prevXVel.set(x, y, xVel.at(x, y));
			prevYVel.set(x, y, yVel.at(x, y));

		}

	}



}

// Reset fields
void EulerFluid::reset()
{
	xVel.reset();
	yVel.reset();
	density.reset();
	prevXVel.reset();
	prevYVel.reset();
	xVelSource.reset();
	yVelSource.reset();
	densitySource.reset();
	prevDensity.reset();
}

void EulerFluid::swapFields(VectorField & v1, VectorField & v2)
{

	float* temp = v1.getPtr();
	v1.replacePtr(v2.getPtr());
	v2.replacePtr(temp);
}

void EulerFluid::swapPointers(float *& x0, float *& x1)
{

	float *memory = x0;
	x0 = x1;
	x1 = memory;

}

// used for debugging
void EulerFluid::print()
{
	xVel.print();
	cout << endl;
	yVel.print();

	swapFields(xVel, yVel);
	cout << endl << endl;
	xVel.print();
	cout << endl;
	yVel.print();

}

void EulerFluid::randColor()
{
	this->color.r = rand() % 240;
	this->color.g = rand() % 240;
	this->color.b = rand() % 240;
}

void EulerFluid::setColor(int r, int g, int b)
{
	this->color.r = r;
	this->color.g = g;
	this->color.b = b;
}

void EulerFluid::setAntiFluid(bool antifluid)
{
	this->antiFluid = antifluid;
}

bool EulerFluid::getAntiFluid()
{
	return antiFluid;
}


