#pragma once
#include <common/interfaces.h>
class OctreeTest
{
public:
	Point3d origin;
	Point3d halfDimension;

	OctreeTest *children[8];
	Point3d *data;

	OctreeTest(const Point3d &origin, const Point3d &halfDimension);

	OctreeTest(const OctreeTest &copy);

	~OctreeTest();

	int getOctantContainingPoint(const Point3d &point)const;

	bool isLeafNode();

	void insert(Point3d *point);
	
	//tool function
	void getPointInsideBox(const Point3d &bmin, const Point3d &bmax, std::vector<Point3d *> &results);


	




};

