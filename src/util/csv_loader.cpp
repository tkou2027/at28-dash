#include "csv_loader.h"
#include <fstream>
#include <DirectXMath.h>
#include "util/debug_ostream.h"

void CSVLoader::Load(std::string filename, CSVData& out_data)
{
	std::ifstream file(filename);
	if (!file)
	{
		hal::dout << "Failed to open file: " << filename << std::endl;
		return;
	}

	std::string line;
	// ファイルから1行ずつ読み込む
	while (std::getline(file, line))
	{
		CSVDataItem item{};

		std::stringstream ss(line);
		std::string segment;
		std::vector<std::string> seg_list;

		while (std::getline(ss, segment, ','))
		{
			seg_list.push_back(segment);
		}

		if (seg_list.size() >= 10)
		{
			
			item.name = seg_list[0];

			Vector3 position{}, rotation{}, scale{};
			position.x = std::stof(seg_list[1]);
			position.y = std::stof(seg_list[2]);
			position.z = std::stof(seg_list[3]);
			rotation.x = std::stof(seg_list[4]);
			rotation.y = std::stof(seg_list[5]);
			rotation.z = std::stof(seg_list[6]);
			scale.x = std::stof(seg_list[7]);
			scale.y = std::stof(seg_list[8]);
			scale.z = std::stof(seg_list[9]);

			item.transform.m_position = position;
			DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
			item.transform.m_rotation.LoadXMVECTOR(q);
			item.transform.m_scale = scale;

			out_data.items.push_back(item);
		}
	}
}
