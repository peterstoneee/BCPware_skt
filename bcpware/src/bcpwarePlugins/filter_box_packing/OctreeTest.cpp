#include "OctreeTest.h"


OctreeTest::OctreeTest(const Point3d &_origin, const Point3d &_halfDimension) : origin(_origin), halfDimension(_halfDimension)
{
	for (int i = 0; i < 8; i++)
		children[0] = NULL;
}

OctreeTest::OctreeTest(const OctreeTest &copy) : origin(copy.origin), halfDimension(copy.halfDimension), data(copy.data)
{

}

int OctreeTest::getOctantContainingPoint(const Point3d &point) const
{
	//point belongs which index of children

	int ret = 0;
	if (point.X() > origin.X())ret |= 4;
	if (point.X() > origin.Y())ret |= 2;
	if (point.X() > origin.Z())ret |= 1;
	return ret;
}

bool OctreeTest::isLeafNode()
{
	return children[0] == NULL;
}

void OctreeTest::insert(Point3d *point)
{

	if (isLeafNode())
	{
		if (data == NULL)
		{
			data = point;
			return;
		}
		else
		{
			Point3d *tempData = data;
			data = NULL;

			for (int i = 0; i < 8; i++)
			{
				Point3d newOrigin = origin;
				newOrigin.X() += halfDimension.X()*(i & 4 ? 0.5 : -0.5);
				newOrigin.Y() += halfDimension.Y()*(i & 2 ? 0.5 : -0.5);
				newOrigin.Z() += halfDimension.Z()*(i & 1 ? 0.5 : -0.5);				
				children[i] = new OctreeTest(newOrigin, halfDimension * 0.5f);
			}

			children[getOctantContainingPoint(*tempData)]->insert(tempData);
			children[getOctantContainingPoint(*point)]->insert(point);

		}
	}
	else
	{
		int childIndex = getOctantContainingPoint(*point);
		children[childIndex]->insert(point);
	}
}

void OctreeTest::getPointInsideBox(const Point3d &bmin, const Point3d &bmax, std::vector<Point3d *> &results)
{
	//check leaf node
	if (isLeafNode())
	{
		if (data != NULL)
		{
			if (data->X() > bmax.X() || data->Y() > bmax.Y() || data->Z() > bmax.Z())return;
			if (data->X() < bmin.X() || data->Y() < bmin.Y() || data->Z() < bmin.Z())return;
			results.push_back(data);
		}
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			Point3d omin = origin - halfDimension;
			Point3d omax = origin + halfDimension;

			if (omax.X() < bmin.X() || omax.Y() < bmin.Y() || omax.Z() < bmin.Z())return;
			if (omax.X() < bmin.X() || omax.Y() < bmin.Y() || omax.Z() < bmin.Z())return;

			children[i]->getPointInsideBox(bmin, bmax, results);

		}
	}
}
OctreeTest::~OctreeTest()
{
	for (int i = 0; i<8; ++i)
		delete children[i];
}
