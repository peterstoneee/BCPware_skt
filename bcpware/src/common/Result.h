#pragma once
#ifndef RESULTSKT_H
#define BINPACKING_H
#include <cstdlib> 
#include <ctime>   
#include <functional>
#include <chrono>
#include <random>


namespace BINPACK{
	//class Placement;

	template <class PlacementS>
	class Result
	{
	public:
		int Score;
		vector<PlacementS> Placements;

		Result()
		{
			Score = 0;
			Placements.clear();
		}
		Result(int _Score, vector<PlacementS> _Placements)
		{
			if (_Placements.size()>100)
				int test = 0;
			Score = _Score;
			Placements = _Placements;
		}

		Result Translate(Point3f offset)
		{
			if (Placements.size() > 100)
				int test = 0;
			vector<PlacementS> tempPlacements(Placements.size());
			for (int i = 0; i < this->Placements.size(); i++)
			{
				PlacementS pii = Placements[i];
				pii.position = pii.position + offset;
				tempPlacements[i] = pii;
				//tempPlacements.push_back(pii);
			}
			Result *tempre = new Result(Score, tempPlacements);
			return *tempre;
		}

		Result & operator =(const Result &a)
		{
			Score = a.Score;
			Placements.clear();
			Placements.resize(a.Placements.size());
			Placements = a.Placements;			
			return *this;
		}




	};
	//typedef Result<Placement>  ResultP;
}
#endif