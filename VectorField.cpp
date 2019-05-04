#include "pch.h"
#include "VectorField.h"

using namespace std;

// Velocity field throughout SFML window

void VectorField::replacePtr(float * ptr)
{
	this->field = ptr;
}

float * VectorField::getPtr()
{
	return field;
}

VectorField::VectorField() {

}

VectorField::VectorField(int x, int y) {

	assignSize(x, y);

}

// Assigns size to the vector field
void VectorField::assignSize(int x, int y) {

	this->x = x;
	this->y = y;

	size = (x + 2)*(y + 2);

	field = new float[size];

	// Initializes vector field to default values
	for (int i = 0; i < size; i++) {
		field[i] = 0;
	}
}

// Prints vertically
void VectorField::print() {

	cout << "Printing:" << endl;

	for (int j = 0; j < x + 2; j++) {
		for (int i = 0; i < y + 2; i++) {

			if (field[i + (y + 2)*j] < .00001f) {
				cout << "0" << " ";
			}
			else {
				cout << field[i + (y + 2)*j] << " ";
			}
			
		}
		cout << endl;
	}

}

// Retrives values at 2d array location from 1d array
float VectorField::at(int i, int j) {
	return field[i + j*(y+2)];
}

void VectorField::set(int i, int j, float value) {

	field[i + j * (y + 2)] = value;

}

int VectorField::getXSize()
{
	return x;
}

int VectorField::getYSize()
{
	return y;
}

void VectorField::reset()
{
	for (int i = 0; i < (x + 2)*(y + 2); i++) {
		field[i] = 0;
	}
}

VectorField::~VectorField() {

	delete field;

}
