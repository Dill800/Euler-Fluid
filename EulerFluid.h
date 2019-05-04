#pragma once
#include "VectorField.h"
#include <iostream>
#include <SFML/Graphics.hpp>

class EulerFluid
{

private:

	// Fluid information
	float dt;
	int xSize;
	int ySize;
	float diffuseRate = .4f;
	float viscosity = .9945f;
	int particleSize = 2;

	sf::Color color;
	bool antiFluid = false;

	// Components of velocity field
	VectorField xVel;
	VectorField yVel;
	
	// Density field (actually a scalar field, but same implementation)
	VectorField density;

	bool draw = true;

	// Previous Timestep Fields
	VectorField prevXVel;
	VectorField prevYVel;
	VectorField prevDensity;

	// Source Vector/ (Scalar) Fields
	VectorField xVelSource;
	VectorField yVelSource;
	VectorField densitySource;
	VectorField div;
	VectorField p;

	void diffuseVelocity();
	void diffuseDensity();
	void advectVelocity();
	void advectDensity();

	void projectVelocity();

public:
	EulerFluid();
	EulerFluid(int, int, float, int);
	~EulerFluid();

	void addSource(int x, int y, int dx, int dy);
	void update();

	// Image
	void render(int, sf::Image&);
	void reset();

	void swapFields(VectorField&, VectorField&);
	void swapPointers(float*&, float*&);
	void print();
	void randColor();
	void setColor(int r, int g, int b);
	void setAntiFluid(bool);
	bool getAntiFluid();


};

