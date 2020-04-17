#pragma once
#include "pack3D.h"


using namespace SKT;


GroupMesh::GroupMesh(){}

GroupMesh::GroupMesh(QVector<Item*> _items, float _minVolume, float _maxVolume, float _devitation)
{
	items = _items;
	minVolume = _minVolume;
	maxVolume = _maxVolume;
	deviation = _devitation;
};