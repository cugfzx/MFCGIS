#pragma once
#include "../shapeengine/ShapeFactory.h"



class CESRIFactory :
	public CShapeFactory
{
public:
	CESRIFactory(void);

	~CESRIFactory(void);

	CShape* CreateShape(const char* path);
};
