#pragma once
#include <iostream>

class VectorField
{



private:
	float* field;
	int size = 0;
	int x = 0;
	int y = 0;

	friend class EulerFluid;

public:

	
	void replacePtr(float*);
	float* getPtr();
	VectorField();
	void assignSize(int, int);
	VectorField(int, int);
	void print();
	float at(int, int);
	void set(int, int, float);
	int getXSize();
	int getYSize();
	void reset();
	~VectorField();
};

