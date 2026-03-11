#pragma once
#include <cstdint>

// defines if object(material) will be rendered by camera
enum class CameraRenderLayer : uint8_t
{
	DEFAULT = 0,
	CAST_SHADOW = 1,
	MAX
};

class CameraRenderLayerMask
{
public:
	static constexpr uint32_t DEFAULT{ 1 << static_cast<uint8_t>(CameraRenderLayer::DEFAULT) };
	static constexpr uint32_t CAST_SHADOW{ 1 << static_cast<uint8_t>(CameraRenderLayer::CAST_SHADOW) };

	static constexpr uint32_t NONE{ 0u };
	static constexpr uint32_t ALL{ 0xFFFFFFFFu };

	static uint32_t GetLayerMask(CameraRenderLayer layer)
	{
		return 1 << static_cast<uint8_t>(layer);
	}

	static bool IfLayerInMask(uint32_t mask, CameraRenderLayer layer)
	{
		uint32_t layer_mask = GetLayerMask(layer);
		return (mask & layer_mask) != 0;
	}
};