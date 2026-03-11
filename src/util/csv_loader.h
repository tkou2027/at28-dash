#pragma once
#include <string>
#include <vector>
#include "math/transform3d.h"

struct CSVDataItem
{
	std::string name;
	Transform3D transform;
};

struct CSVData
{
	std::vector<CSVDataItem> items;
};

class CSVLoader
{
public:
	static void Load(std::string filename, CSVData& out_data);
};