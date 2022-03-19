#pragma once
#include "../shapeengine/Shape.h"



class CShapeFactory
{

public:
	
	virtual ~CShapeFactory(void) = 0;
	virtual CShape* CreateShape(const char* path) = 0;

protected:
	CShapeFactory(void);
};

