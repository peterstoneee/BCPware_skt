#pragma once

#include <QVector>
#include <vcg/complex/complex.h>
#include <vcg/space/box3.h>
#include "interfaces.h"
#include "pack3D.h"
using namespace vcg;

typedef QVector<Box3m> BVHTree;


namespace SKT
{
	
	class Node;
	typedef SKT::Axis::AxisXYZ Axisc;
	float partitionScore(QVector<Box3f> &boxes, Axisc axis, float point, bool side);
	void partitionBox(Box3f &box, Axisc axis, float point, bool &left, bool &right);
	void partition(QVector<Box3f> &boxes, Axisc axis, float point, bool side, QVector<Box3f> &left, QVector<Box3f> &right);
	static bool treeintersects(BVHTree a, BVHTree b, Point3f t1, Point3f t2, int i, int j);
	static bool boxesIntersect(Box3m b1, Box3m b2, Point3f t1, Point3f t2);
	
	Point3f vector(SKT::Axis x);
	Node * NewNode(QVector<Box3f> &boxes, int depth);
	Box3f boxforBoxes(QVector<Box3f> &boxes);
	class Node
	{
	public:
		Box3f boxCurrent;
		Node *left;
		Node *right;
		Node(){};
		~Node(){};
		Node(Box3f _box, Node *_a, Node *_b)
		{
			boxCurrent = _box;
			left = _a;
			right = _b;
		}

		void Flatten(QVector<Box3m> &tree, int i)
		{
			tree[i] = this->boxCurrent;
			if (this->left != NULL)
			{
				this->left->Flatten(tree, i * 2 + 1);
			}
			if (this->right != NULL)
			{
				this->right->Flatten(tree, i * 2 + 2);
			}

			/*tree.push_back(this->boxCurrent);
			if (this->left != NULL)
			{
				this->left->Flatten(tree, i * 2 + 1);
			}
			if (this->right != NULL)
			{
				this->right->Flatten(tree, i * 2 + 2);
			}*/

		}



		void split(QVector<Box3f> &boxes, int depth)
		{
			if (depth == 0)return;

			Box3f tempBox = this->boxCurrent;
			float best = tempBox.Volume();
			//BVHTest::Axis bestAxis = BVHTest::AxisNone;
			Axisc bestAxis = SKT::Axis::AxisNone;
			float bestPoint = 0;
			bool bestSide = false;
			int N = 16;

			for (int s = 0; s < 2; s++)
			{
				bool side = ((s == 1) ? 1 : 0);
				for (int i = 1; i < N; i++)
				{
					double p = double(i) / N;
					float xDim = tempBox.min.X() + tempBox.DimX()*p;
					float yDim = tempBox.min.Y() + tempBox.DimY()*p;
					float zDim = tempBox.min.Z() + tempBox.DimZ()*p;

					/*qDebug() << "best" << best;
					qDebug() << "bestPoint" << bestPoint;
					qDebug() << "bestSide" << bestSide;
					qDebug() << "bestAxis" << (int)bestAxis;
					qDebug() << "p" << i;*/

					float sx, sy, sz;
					sx = partitionScore(boxes, Axisc::AxisX, xDim, side);
					if (sx < best)
					{
						best = sx;
						bestAxis = SKT::Axis::AxisX;
						bestPoint = xDim;
						bestSide = side;
					}
					sy = partitionScore(boxes, Axisc::AxisY, yDim, side);
					if (sy < best)
					{
						best = sy;
						bestAxis = SKT::Axis::AxisY;
						bestPoint = yDim;
						bestSide = side;
					}
					sz = partitionScore(boxes, Axisc::AxisZ, zDim, side);
					if (sz < best)
					{
						best = sz;
						bestAxis = SKT::Axis::AxisZ;
						bestPoint = zDim;
						bestSide = side;
					}
				}
			}
			if (bestAxis == SKT::Axis::AxisNone)return;
			QVector<Box3f> left, right;
			partition(boxes, bestAxis, bestPoint, bestSide, left, right);
			this->left = NewNode(left, depth - 1);
			this->right = NewNode(right, depth - 1);
		}

	};

	//static bool treeIntersects(BVHTree a, BVHTree b, Point3f t1, Point3f t2, int i, int j)
	//{
	//	if (!a[i].Collide(b[j]))
	//		return false;
	//	int i1 = i * 2 + 1;
	//	int i2 = i * 2 + 2;
	//	int j1 = j * 2 + 1;
	//	int j2 = j * 2 + 2;

	//	if (i1>=a.size() && j1>=b.size())
	//		return true;
	//	else if (i1 > a.size());
	//	else if (j1>=b.size());
	//	else
	//	{
	//		return treeIntersects(a, b, t1, t2, i1, j1) || treeIntersects(a, b, t1, t2, i1, j2)
	//			|| treeIntersects(a, b, t1, t2, i2, j1) || treeIntersects(a, b, t1, t2, i2, j2);
	//	}
	//}



	static Node * NewNode(QVector<Box3f> &boxes, int depth)
	{
		Box3f box = boxforBoxes(boxes);//
		box.Offset(0.5);
		Node *node = new Node(box, NULL, NULL);
		node->split(boxes, depth);
		return node;
	}


	static QVector<Box3f> newTreeForMesh(MeshDocument *md, MeshModel *mm, int depth)
	{
		MeshModel *mp = new MeshModel(md, "testt", "bvh");
		//MeshModel *mp = new MeshModel(nullptr, "testt", "bvh");
		//MeshModel *mp = md;
		vcg::tri::Append<CMeshO, CMeshO>::MeshCopy(mp->cm, mm->cm);
		Matrix44m toCenter;
		Point3m translatePath;
		translatePath.SetZero();
		translatePath = Point3m(0, 0, 0) - mp->cm.bbox.Center();
		toCenter.SetTranslate(translatePath);
		tri::UpdatePosition<CMeshO>::Matrix(mp->cm, toCenter, true);

		QVector<Box3f> boxes;
		CMeshO::FaceIterator fi;
		for (fi = mp->cm.face.begin(); fi < mp->cm.face.end(); fi++)
		{
			Point3m v1 = fi->V(0)->P();
			Point3m v2 = fi->V(1)->P();
			Point3m v3 = fi->V(2)->P();
			///*face min max*/
			float sx1 = fi->V(0)->P().X();
			float sx2 = fi->V(1)->P().X();
			float sx3 = fi->V(2)->P().X();

			float sy1 = fi->V(0)->P().Y();
			float sy2 = fi->V(1)->P().Y();
			float sy3 = fi->V(2)->P().Y();

			float sz1 = fi->V(0)->P().Z();
			float sz2 = fi->V(1)->P().Z();
			float sz3 = fi->V(2)->P().Z();

			Point3m facemin;
			Point3m facemax;

			facemax[0] = sx1 > sx2 ? (sx1 > sx3 ? sx1 : sx3) : (sx2 > sx3 ? sx2 : sx3);
			facemax[1] = sy1 > sy2 ? (sy1 > sy3 ? sy1 : sy3) : (sy2 > sy3 ? sy2 : sy3);
			facemax[2] = sz1 > sz2 ? (sz1 > sz3 ? sz1 : sz3) : (sz2 > sz3 ? sz2 : sz3);

			facemin[0] = sx1 < sx2 ? (sx1 < sx3 ? sx1 : sx3) : (sx2 < sx3 ? sx2 : sx3);
			facemin[1] = sy1 < sy2 ? (sy1 < sy3 ? sy1 : sy3) : (sy2 < sy3 ? sy2 : sy3);
			facemin[2] = sz1 < sz2 ? (sz1 < sz3 ? sz1 : sz3) : (sz2 < sz3 ? sz2 : sz3);

			Box3m faceb(facemin, facemax);
			boxes.push_back(faceb);
		}
		Node *root = NewNode(boxes, depth);
		QVector<Box3f> new_tree(511);
		root->Flatten(new_tree, 0);


		md->delMesh(mp);
		//delete mp;
		return new_tree;


	}




	static Box3f boxforBoxes(QVector<Box3f> &boxes)
	{
		if (boxes.size() == 0)
		{
			Box3f a; a.SetNull();
			return a;
		}

		Point3f bbmin = boxes[0].min;
		Point3f bbmax = boxes[0].max;

		foreach(Box3f box, boxes)
		{
			bbmin.Import(Point3f(std::min(bbmin.X(), box.min.X()), std::min(bbmin.Y(), box.min.Y()), std::min(bbmin.Z(), box.min.Z())));
			bbmax.Import(Point3f(std::max(bbmax.X(), box.max.X()), std::max(bbmax.Y(), box.max.Y()), std::max(bbmax.Z(), box.max.Z())));
		}

		return Box3f(bbmin, bbmax);


	}

	static float partitionScore(QVector<Box3f> &boxes, Axisc axis, float point, bool side)
	{
		Box3f major;
		foreach(Box3f box, boxes)
		{
			bool l = true, r = true;
			partitionBox(box, axis, point, l, r);
			bool test1 = l && r;
			bool test2 = l && side;
			bool test3 = r && !side;
			if ((l && r) || (l && side) || (r && !side))
			{
				major.Add(box);// Extend(box)
			}

		}
		Box3f minor;
		foreach(Box3f box, boxes)
		{
			//if (!box.isInside(major))
			if (!box.Collide(major))
			{
				minor.Add(box);
			}
		}
		float majorvolume = major.Volume();
		float minorvolume = minor.Volume();
		float intersectVolume = (major.IntersectionBox(minor)).Volume();
		return major.Volume() + minor.Volume() - major.IntersectionBox(minor).Volume();

	}


	static void partitionBox(Box3f &box, Axisc axis, float point, bool &left, bool &right)
	{
		switch (axis)
		{
		case Axisc::AxisX:
			left = (box.min.X() <= point) ? true : false;
			right = (box.max.X() >= point) ? true : false;
			break;
		case Axisc::AxisY:
			left = (box.min.Y() <= point) ? true : false;
			right = (box.max.Y() >= point) ? true : false;
			break;
		case Axisc::AxisZ:
			left = (box.min.Z() <= point) ? true : false;
			right = (box.max.Z() >= point) ? true : false;
			break;
		}
		return;
	}

	static void partition(QVector<Box3f> &boxes, Axisc axis, float point, bool side, QVector<Box3f> &left, QVector<Box3f> &right)
	{
		Box3f major;
		foreach(Box3f box, boxes)
		{
			bool l, r;
			partitionBox(box, axis, point, l, r);
			if ((l && r) || (l && side) || (r && !side))
			{
				major.Add(box);// Extend(box)
			}

		}

		foreach(Box3f box, boxes)
		{
			if (box.Collide(major))
			{
				left.append(box);
			}
			else
			{
				right.append(box);
			}


		}

		if (!side)
		{
			left.swap(right);
		}

		return;

	}

	static bool treeIntersects(BVHTree a, BVHTree b, Point3f t1, Point3f t2)
	{
		return treeintersects(a, b, t1, t2, 0, 0);
	}

	static bool treeintersects(BVHTree a, BVHTree b, Point3f t1, Point3f t2, int i, int j )
	{
		if (!boxesIntersect(a[i], b[j], t1, t2))
			return false;
		int i1 = i * 2 + 1;
		int i2 = i * 2 + 2;
		int j1 = j * 2 + 1;
		int j2 = j * 2 + 2;

		if (i1>=a.size() && j1>=b.size())
		{
			return true;
		}
		else if (i1>=a.size())
		{
			return (treeintersects(a, b, t1, t2, i, j1) || treeintersects(a, b, t1, t2, i, j2));
		}
		else if (j1>=b.size())
		{
			return treeintersects(a, b, t1, t2, i1, j) || treeintersects(a, b, t1, t2, i2, j);
		}
		else
		{
			return treeintersects(a, b, t1, t2, i1, j1) ||
				treeintersects(a, b, t1, t2, i1, j2) ||
				treeintersects(a, b, t1, t2, i2, j1) ||
				treeintersects(a, b, t1, t2, i2, j2);
		}		

	}
	static bool boxesIntersect(Box3m b1, Box3m b2, Point3f t1, Point3f t2)
	{
		if (b1.IsNull() || b2.IsNull())
			return false;
		return !(
			b1.min.X() + t1.X() > b2.max.X() + t2.X() ||
			b1.max.X() + t1.X() < b2.min.X() + t2.X() ||
			b1.min.Y() + t1.Y() > b2.max.Y() + t2.Y() ||
			b1.max.Y() + t1.Y()< b2.min.Y() + t2.Y() ||
			b1.min.Z() + t1.Z() > b2.max.Z() + t2.Z() ||
			b1.max.Z() + t1.Z()< b2.min.Z() + t2.Z());
	}
	


}