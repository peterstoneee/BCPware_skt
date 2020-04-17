#pragma once
#include <cstdlib> 
#include <ctime>   
#include <functional>
#include <chrono>
#include <random>
#include "meshmodel.h"
#include "NodeBVH.h"

typedef QVector<Box3m> BVHTree;
typedef QVector<QVector<Box3m>> BVHTrees;

#define Z_ENERGY_WEIGHT 0.7
#define XYZ_ENERGY_WEIGHT (1.-Z_ENERGY_WEIGHT)

namespace SKT{
	static QVector<Box3f> newTreeForMesh(MeshDocument *md, MeshModel *mm, int depth);
	static bool treeIntersects(BVHTree a, BVHTree b, Point3f t1, Point3f t2);
	static int randint(int n);
	static Point3f RandomUnitVector();
	static double randDouble();
	static int randint(int n);
	static double randNormDouble();
	static Point3f RandomUnitVector_no_Z();

	Matrix44m RotateTo(Point3f a, Point3f b);
	const double PI = std::acos(-1);
	/*struct Item
	{
	MeshModel *mesh;
	Trees trees;
	int Rotation_num;
	Point3f Translation;
	};*/


	struct Axis
	{
		enum AxisXYZ{ AxisNone, AxisX, AxisY, AxisZ };
		static Point3f AxisVector(AxisXYZ x)
		{
			switch (x)
			{
			case AxisX:return Point3f(1, 0, 0);
				break;
			case AxisY:return Point3f(0, 1, 0);
				break;
			case AxisZ:return Point3f(0, 0, 1);
				break;
			}

		}
	};

	static QVector<Matrix44m> Rotations;
	static void initRotations()
	{
		for (int i = 0; i < 4; i++) {
			for (int s = -1; s <= 1; s += 2) {
				for (int a = 1; a <= 3; a++)
				{
					Point3f up = Axis::AxisVector(Axis::AxisZ);
					Matrix44m m;
					m.SetIdentity();
					m.SetRotateDeg(90 * float(i), up);
					Point3f vecTemp = Axis::AxisVector(Axis::AxisXYZ(a));
					vecTemp *= float(s);
					Matrix44m m2 = RotateTo(up, vecTemp);
					m = m2*m;
					qDebug() << up[0] << up[1] << up[2] << ": " << vecTemp[0] << vecTemp[1] << vecTemp[2];

					Rotations.append(m);
					/*up: = AxisZ.Vector();
					m : = fauxgl.Rotate(up, float64(i)*fauxgl.Radians(90))
					m = m.RotateTo(up, Axis(a).Vector().MulScalar(float64(s)))
					Rotations = append(Rotations, m)*/
				}
			}
		}
	}
	static void initRotations_zAxis()
	{
		double unit = 20;
		double circleDegree = 360;
		double xSteps = 360. / 20;

		for (int i = 0; i < circleDegree; i += xSteps)
		{
			Point3f up = Axis::AxisVector(Axis::AxisZ);
			Matrix44m m;
			m.SetIdentity();
			m.SetRotateDeg(float(i), up);

			//qDebug() << up[0] << up[1] << up[2] << ": " << vecTemp[0] << vecTemp[1] << vecTemp[2];

			Rotations.append(m);


		}
	}

	Matrix44m RotateTo(Point3f a, Point3f b)
	{
		float bDota = b.dot(a);
		if (bDota == 1)
		{
			Matrix44m temp44m;
			temp44m.SetIdentity();
			return temp44m;
		}
		else if (bDota == -1)
		{
			Matrix44m temp;
			return temp.SetRotateRad(float(PI), a.Perpendicular());
		}
		else
		{
			float angle = std::acos(bDota);
			Point3f v = b ^a;
			v = v.normalized();
			Matrix44m temp;
			temp.SetIdentity();
			return temp.SetRotateRad(angle, v);
		}

	}







	//class Annealable
	//{
	//public:
	//	
	//	/*static std::auto_ptr<Parent> GetThing()
	//	{
	//		return std::auto_ptr<Parent>(new Child());
	//	}*/
	//	Annealable(){}
	//
	//	void showProgress(int i, int n, double e, double d)
	//	{
	//		int pct = 100 * double(i) / double(n);
	//		qDebug() <<  pct;
	//		for (int p = 0; p < 100; p += 3)
	//		{
	//			if (pct>p)
	//				qDebug() << "=";
	//			else
	//				qDebug()<<" ";
	//		}
	//	}
	//	~Annealable(){}
	//	virtual double Energy() { return 0; }
	//	virtual Undo DoMove() {
	//		return Undo(); 
	//	}//Undo *temp = new Undo(); return *temp;}
	//	virtual void UndoMove() {}
	//	virtual Annealable Copy() 
	//	{
	//		Annealable *temp2 = new Annealable(); 
	//		return *temp2; 
	//	}
	//};


	class Item
	{
	public:

		Item(){};
		Item(MeshModel *_meshmodel, BVHTrees _trees, int _rotationNum, Point3f _translation_mesh)
		{
			meshItem = _meshmodel;
			treess = _trees;
			rotationNum = _rotationNum;
			translation_mesh = _translation_mesh;

		};
		~Item(){};
		MeshModel * meshItem;
		BVHTrees treess;
		int rotationNum;
		Point3f translation_mesh;
		Matrix44m* Matrix()
		{
			Matrix44m temp = Rotations[this->rotationNum];
			return &(temp.SetTranslate(translation_mesh));
		}
		Item* copy()
		{
			//Item dup = *this;
			Item *dup = new Item(meshItem, treess, rotationNum, translation_mesh);
			return dup;
		}
	};


	struct Undo
	{
		int Index;
		int Rotation;
		Point3m Translation;
		Undo(int _index, int _Rotation, Point3m _Translation)
		{
			Index = _index;
			Rotation = _Rotation;
			Translation = _Translation;
		}
		Undo(){}
	};
	class GroupMesh// :public Annealable
	{
	public:
		enum packing_Parameter{ Volume_pa, Z_Axis_volume };
		GroupMesh()
		{
			minVolume = 0;
			maxVolume = 0;
			deviation = 0;
			min_DimZ = 0;
			max_DimZ = 0;
			max_DimZ_Volume = 0;
			//deviation = 0;
		}
		GroupMesh(QVector<Item*> _items, float _minVolume, float _maxVolume, float _devitation, float _min_DimZ, float  _max_DimZ, float _max_DimZ_Volume, float _deviation)
		{
			items = _items;
			minVolume = _minVolume;
			maxVolume = _maxVolume;
			deviation = _devitation;
			min_DimZ = _min_DimZ;
			max_DimZ = _max_DimZ;
			max_DimZ_Volume = _max_DimZ_Volume;
			deviation = _deviation;
		};
		~GroupMesh(){};

		QVector<Item*> items;
		float minVolume;
		float maxVolume;
		float min_DimZ;
		float max_DimZ;

		float max_DimZ_Volume;
		float deviation;



		static BVHTrees Addd(MeshModel *mesh, int detail, int count)
		{
			BVHTree tree = SKT::newTreeForMesh(mesh->parent, mesh, detail);
			BVHTrees trees(Rotations.size());

			for (int i = 0; i < Rotations.size(); i++)
			{
				QVector<Box3m> tempTree;
				for (int j = 0; j < tree.size(); j++)
				{
					Box3m bb = tree[j].Transformbox(Rotations[i]);

					tempTree.push_back(bb);

				}
				trees[i] = tempTree;
			}

			return trees;
		}



		void Add(MeshModel *mesh, int detail, int count, packing_Parameter papa)
			//static BVHTrees Add(MeshModel *mesh, int detail, int count)
		{
			BVHTree tree = SKT::newTreeForMesh(mesh->parent, mesh, detail);
			BVHTrees trees(Rotations.size());

			for (int i = 0; i < Rotations.size(); i++)
			{
				QVector<Box3m> tempTree;
				for (int j = 0; j < tree.size(); j++)
				{
					Box3m bb;
					if (papa == Volume_pa)
					{
						bb = tree[j].Transformbox(Rotations[i]);
					}
					else if (papa == Z_Axis_volume)
					{
						bb = tree[j].Transformbox_test(Rotations[i]);
					}
					tempTree.push_back(bb);

				}
				trees[i] = tempTree;
			}
			for (int i = 0; i < count; i++)
			{
				add(mesh, trees, papa);
				//this->items
			}
			//return trees;
		}

		void add(MeshModel *mesh, BVHTrees trees, packing_Parameter papa)
		{
			int index = items.size();
			Item *item_o = new Item(mesh, trees, 0, Point3f(0, 0, 0));
			items.push_back(item_o);
			items[0]->rotationNum;

			float d = 1.0;

			while (!this->ValidChange(index))
			{
				if (papa == Volume_pa){
					Point3f test3f = RandomUnitVector();
					item_o->rotationNum = randint(Rotations.size());
					item_o->translation_mesh = RandomUnitVector()*d;
					d *= 1.2;
				}
				else if (papa == Z_Axis_volume)
				{
					//Point3f test3f = RandomUnitVector();
					item_o->rotationNum = randint(Rotations.size());
					item_o->translation_mesh = RandomUnitVector_no_Z()*d;
					d *= 1.2;
				
				}
				//qDebug() << "i" << item_o->rotationNum;
				//qDebug() << "item->translation_mesh" << item_o->translation_mesh[0] << item_o->translation_mesh[1] << item_o->translation_mesh[2];

			}
			BVHTree treetemp = trees[0];
			if (papa == Volume_pa)
			{
				minVolume = std::max(minVolume, treetemp[0].Volume());
				maxVolume += treetemp[0].Volume();
			}
			else if (papa == Z_Axis_volume)
			{
				minVolume = std::max(minVolume, treetemp[0].Volume());
				maxVolume += treetemp[0].Volume();
				//maxVolume += treetemp[0].DimX()*treetemp[0].DimY();

				min_DimZ = std::max(min_DimZ, treetemp[0].DimZ()* treetemp[0].DimZ()* treetemp[0].DimZ());
				max_DimZ += treetemp[0].DimZ()* treetemp[0].DimZ()* treetemp[0].DimZ();
				/*min_DimZ = std::max(min_DimZ, treetemp[0].DimZ()* treetemp[0].DimZ());
				max_DimZ += treetemp[0].DimZ()* treetemp[0].DimZ();*/

				max_DimZ_Volume = Z_ENERGY_WEIGHT*max_DimZ + XYZ_ENERGY_WEIGHT*maxVolume;
				//max_DimZ_Volume = max_DimZ + maxVolume;
			}
		}


		void Reset()
		{
			QVector<Item*> _items = items;// Maybe can't use pointer
			this->items.clear();
			minVolume = 0;
			maxVolume = 0;

			foreach(Item *itemX, _items)
			{
				add(itemX->meshItem, itemX->treess, packing_Parameter::Volume_pa);
			}

		}


		//GroupMesh pack(int iterations/*, std::function<void(int)> AnnealCallback*/)
		GroupMesh pack(int iterations, vcg::CallBackPos *cb, packing_Parameter papa)
			//int pack(int iterations/*, std::function<void()> AnnealCallback*/)
		{
			double e = 0.5;
			double p1 = 1e0*e;
			double p2 = 1e-4*e;

			//GroupMesh temp = Anneal(*this, 1e0*e, 1e-4*e, iterations,cb).Copy();//,NULL);
			if (papa == packing_Parameter::Volume_pa)
			{
				GroupMesh temp = Anneal(*this, 1e0*e, 1e-4*e, iterations, cb).Copy();//,NULL);
				return temp;
			}
			else if (papa == packing_Parameter::Z_Axis_volume)
			{
				GroupMesh temp = Anneal2(*this, 1e0*e, 1e-4*e, iterations, cb).Copy();//,NULL);
				return temp;
			}

			//return dynamic_cast<GroupMesh*>(temp);
		}
		//QVector<MeshModel *>  Meshes()
		//{
		//	QVector<MeshModel *> result(this->items->size());
		//	//foreach(Item *item, items)
		//	//{
		//	//	//MeshModel *Mesh = new MeshModel(meshDoc(), meshDoc()->getMesh(i)->fullName(), meshDoc()->getMesh(i)->label());;
		//	//	//vcg::tri::Append<CMeshO, CMeshO >::MeshCopy(copyMesh->cm, meshDoc()->getMesh(i)->cm);
		//	//}
		//	return QVector<MeshModel *>();
		//}
		//MeshModel * Mesh()
		//{
		//}
		//QVector<MeshModel *> TreeMeshes()
		//{
		//return QVector<MeshModel *>();
		//}
		//MeshModel * TreeMesh()
		//{
		//}
		//float Volumn(){ return 0; }
		//float Energy(){ return 0; }
		//float DoMove(){ return 0; }
		//float copy(){ return 0; }





		Box3m BoundingBox()
		{
			Box3m returnBox;
			//foreach(Item *item, items)
			for (int i = 0; i < items.size(); i++)
			{
				Item *item = (items)[i];
				BVHTree temptree = item->treess[item->rotationNum];
				temptree[0].Translate(item->translation_mesh);
				returnBox.Add(temptree[0]);
			}
			//qDebug() << "BoundingBoxVolume" << returnBox.DimX()*returnBox.DimY()*returnBox.DimZ();
			return returnBox;
		}


		double Volume()
		{
			Box3m box = BoundingBox();
			//qDebug() << "Energy_dimVolume" << box.DimX()*box.DimY()*box.DimZ();
			return box.DimX()*box.DimY()*box.DimZ();
			//return box.Volume();
		}

		double XY_Area()
		{
			Box3m box = BoundingBox();
			//qDebug() << "Energy_dimVolume" << box.DimX()*box.DimY()*box.DimZ();
			return box.DimX()*box.DimY();
			//return box.Volume();
		}
		double Z_Volume()
		{
			Box3m box = BoundingBox();
			//qDebug() << "Energy_dimVolume" << box.DimX()*box.DimY()*box.DimZ();
			return box.DimZ()*box.DimZ()*box.DimZ();
		}


		double Energy()
		{
			//qDebug() << "Volume:" << Volume() << "_maxVolume:" << maxVolume <<"_Volume() / maxVolume:"<< Volume() / maxVolume;
			return (Volume() / maxVolume);
		}

		double Energy2()
		{
			//qDebug() << "DimZ" << 0.8*BoundingBox().DimZ() << "_maxVolume:" << maxVolume  ;
			return (Z_ENERGY_WEIGHT*Z_Volume() + XYZ_ENERGY_WEIGHT*Volume()) / max_DimZ_Volume;
			//return (Z_Volume() + XY_Area()) / max_DimZ_Volume;
		}



		Undo DoMove()
		{

			int i = SKT::randint(items.size());
			Item *item = (items)[i];
			Undo undoo(i, item->rotationNum, item->translation_mesh);
			bool changero = false;
			while (true)
			{

				if (randint(4) == 0)
				{

					item->rotationNum = randint(Rotations.size());
					/*if (i == 0)
					{
					qDebug() << "i" << item->rotationNum;
					}*/
					changero = true;
				}
				else
				{
					//translate
					Point3f randAxis = Axis::AxisVector(Axis::AxisXYZ(SKT::randint(3) + 1));
					randAxis *= (randNormDouble()* this->deviation);
					item->translation_mesh += randAxis;
				}
				if (ValidChange(i))
				{

					//qDebug() << "item->rotationNum" << item->rotationNum;
					//qDebug() << "item->translation_mesh" << item->translation_mesh[0] << item->translation_mesh[1] << item->translation_mesh[2];
					break;
				}//
				item->rotationNum = undoo.Rotation;
				item->translation_mesh = undoo.Translation;
			}

			return undoo;

		}

		void UndoMove(Undo undo)
		{
			Item *item = (items)[undo.Index];
			item->rotationNum = undo.Rotation;
			item->translation_mesh = undo.Translation;

		}


		GroupMesh Copy()
		{
			QVector<Item*> itemss(items.size());
			for (int i = 0; i < items.size(); i++)
			{
				itemss[i] = (items)[i]->copy();
			}
			return *(new GroupMesh(itemss, minVolume, maxVolume, deviation, min_DimZ, max_DimZ, max_DimZ_Volume, deviation));
		}


	public:
		bool ValidChange(int i)
		{
			Item  *item1 = items.at(i);
			BVHTree tree1 = item1->treess[item1->rotationNum];

			for (int j = 0; j < items.size(); j++)
			{
				if (j == i)continue;
				Item *item2 = (items)[j];
				BVHTree  tree2 = item2->treess[item2->rotationNum];
				if (treeIntersects(tree1, tree2, item1->translation_mesh, item2->translation_mesh))
					return false;
			}
			return true;

		}

		GroupMesh Anneal(GroupMesh state, double maxTemp, double minTemp, int steps, vcg::CallBackPos *cb)// , std::function<void(void)> AnnealCallback)
		{

			QTime time;
			double factor = -std::log(maxTemp / minTemp);
			state = state.Copy();
			GroupMesh bestState = state.Copy();
			//if (AnnealCallback != NULL)

			double bestEnergy = state.Energy();
			double previousEnergy = bestEnergy;

			int rate = steps / 200.;

			for (int xStep = 0; xStep < steps; xStep++)
			{
				double pct = (double)xStep / (steps - 1.);
				double temp = maxTemp* exp(factor * pct);

				cb((xStep * 100) / steps, "Simulated Annealing");
				if (xStep % rate == 0)
				{
					qDebug() << "xStep" << xStep << " : " << steps << "bestEnergy" << bestEnergy << endl;
					//showProgress(xStep, steps, bestEnergy, time.elapsed());
				}

				Undo undo = state.DoMove();
				//qDebug() << "BoundingBox2" << state.BoundingBox().Volume();
				double newenergy = state.Energy();
				double change = newenergy - previousEnergy;
				double exppp = exp(-change / temp);
				if ((change >= 0) && (exp(-change / temp) < randDouble()))
				{
					state.UndoMove(undo);
				}
				else
				{
					//qDebug() << "BoundingBox3" << BoundingBox().Volume();
					previousEnergy = newenergy;
					if (newenergy < bestEnergy)
					{
						bestEnergy = newenergy;
						//qDebug() << "testEnergy" << state.Energy();
						bestState = state.Copy();

						double testbboxve = BoundingBox().Volume();
						//qDebug() << "BoundingBox4" << bestState.BoundingBox().Volume();

						//qDebug() << "bestEnergy" << bestEnergy;
					}
				}
			}
			//showProgress


			return bestState;
		}
		GroupMesh Anneal2(GroupMesh state, double maxTemp, double minTemp, int steps, vcg::CallBackPos *cb)// , std::function<void(void)> AnnealCallback)
		{

			QTime time;
			double factor = -std::log(maxTemp / minTemp);
			state = state.Copy();
			GroupMesh bestState = state.Copy();
			//if (AnnealCallback != NULL)

			double bestEnergy = state.Energy2();
			double previousEnergy = bestEnergy;

			int rate = steps / 200.;

			for (int xStep = 0; xStep < steps; xStep++)
			{
				double pct = (double)xStep / (steps - 1.);
				double temp = maxTemp* exp(factor * pct);

				cb((xStep * 100) / steps, "Simulated Annealing");
				if (xStep % rate == 0)
				{
					qDebug() << "xStep" << xStep << " : " << steps << "bestEnergy" << bestEnergy << endl;
					//showProgress(xStep, steps, bestEnergy, time.elapsed());
				}

				Undo undo = state.DoMove();
				//qDebug() << "BoundingBox2" << state.BoundingBox().Volume();
				double newenergy = state.Energy2();
				double change = newenergy - previousEnergy;
				double exppp = exp(-change / temp);
				if ((change >= 0) && (exp(-change / temp) < randDouble()))
				{
					state.UndoMove(undo);
				}
				else
				{

					previousEnergy = newenergy;
					if (newenergy < bestEnergy)
					{
						bestEnergy = newenergy;
						//qDebug() << "testEnergy" << state.Energy();
						bestState = state.Copy();

						//double testbboxve = BoundingBox().Volume();
						//qDebug() << "BoundingBox4" << bestState.BoundingBox().Volume();

						//qDebug() << "bestEnergy" << bestEnergy;
					}
				}
			}
			//showProgress


			return bestState;
		}



	};


	static int randint(int n) {
		//srand(time(NULL));
		if ((n - 1) == RAND_MAX) {
			return rand();
		}
		else {

			long end = RAND_MAX / n;
			assert(end > 0L);
			end *= n;

			int r;
			while ((r = rand()) >= end);

			return r % n;
		}
	}

	static double randNormDouble() {
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine generator(seed);

		std::normal_distribution<double> distribution(0.0, 1.0);

		//std::cout << "some Normal-distributed(0.0,1.0) results:" << std::endl;
		/*for (int i = 0; i<10; ++i)
			std::cout << distribution(generator) << std::endl;*/
		return distribution(generator);
	}

	static double randDouble() {
		//srand(time(NULL));

		/* 產生 [0, 1) 的浮點數亂數 */
		double x = (double)rand() / (RAND_MAX + 1.0);
		return x;
	}



	static Point3f RandomUnitVector()
	{
		//srand(time(NULL));
		while (true)
		{
			double x = ((double)rand() / (RAND_MAX + 1.0))*2. - 1;
			double y = ((double)rand() / (RAND_MAX + 1.0))*2. - 1;
			double z = ((double)rand() / (RAND_MAX + 1.0))*2. - 1;
			if ((x*x + y*y + z*z) > 1)
				continue;
			return Point3f(x, y, z).Normalize();
		}

	}

	static Point3f RandomUnitVector_no_Z()
	{
		//srand(time(NULL));
		while (true)
		{
			double x = ((double)rand() / (RAND_MAX + 1.0))*2. - 1;
			double y = ((double)rand() / (RAND_MAX + 1.0))*2. - 1;
			double z = 0;// ((double)rand() / (RAND_MAX + 1.0))*2. - 1;
			if ((x*x + y*y + z*z) > 1)
				continue;
			return Point3f(x, y, z).Normalize();
		}

	}



	/*func RandomUnitVector() Vector{
		for {
		x: = rand.Float64() * 2 - 1
		y : = rand.Float64() * 2 - 1
		z : = rand.Float64() * 2 - 1
		if x*x + y*y + z*z > 1 {
		continue
		}
		return Vector{ x, y, z }.Normalize()
		}
		}*/




	static GroupMesh *newGroupMesh()
	{
		QVector<Item*> temp;
		return new GroupMesh(temp, 0, 0, 1, 0, 0, 0, 0);
	}


}