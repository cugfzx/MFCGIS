#include "pch.h"
#include "ESRIFactory.h"
#include "../shapeengine/Dot.h"
#include "../shapeengine/Polygon.h"
#include "../shapeengine/Polyline.h"




CESRIFactory::CESRIFactory(void)
{
}

CESRIFactory::~CESRIFactory(void)
{
}

CShape* CESRIFactory::CreateShape(const char* path)
{
	const int MAXPATHLEN = 256;
	
	char shpPath[MAXPATHLEN];
	char dbfPath[MAXPATHLEN];
	
	strcpy( shpPath, path);
	strcpy( dbfPath, path);

	char* shpExt = ".shp";
	char* dbfExt = ".dbf";
	
	strncat( shpPath, shpExt, __min( strlen(shpExt), MAXPATHLEN - strlen(shpPath)) );
	strncat( dbfPath, dbfExt, __min( strlen(dbfExt), MAXPATHLEN - strlen(dbfPath)) );

	SHPHandle hShape = SHPOpen(shpPath,"rb+");

	if(!hShape)
		return NULL;

	int nType = hShape->nShapeType;

	CString name = path;
	int i = name.ReverseFind('\\');
	name = name.Right(name.GetLength() - i -1);


	switch(nType)
	{
	case SHPT_ARC:
	case SHPT_ARCM:
		{
			CShape* shape = new CPolyline(hShape);
			if (shape)
			{	
				shape->LoadXBase(dbfPath);
				shape->SetName(name);
			}
			return shape;
		}
	case SHPT_POLYGON:
	case SHPT_POLYGONM:
		{
			CShape* shape = new CPolygon(hShape);
			if (shape)
			{
				shape->LoadXBase(dbfPath);
				shape->SetName(name);
			}
			return shape;
		}
	case SHPT_POINT:
	case SHPT_POINTM:
		{
			CShape* shape = new CDot(hShape);
			if (shape)
			{
				shape->LoadXBase(dbfPath);
				shape->SetName(name);
			}
			return shape;
		}
	default:
		return NULL;
	}

}