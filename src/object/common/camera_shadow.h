#pragma once
#include "object/game_object.h"
#include "render/config/camera_data.h"
#include "config/camera_names.h"

class CameraShadow : public GameObject
{
public:
	void Initialize() override;
	void InitializeConfig(
		const CameraPreset& camera_preset,
		std::weak_ptr<class GameObject> target_object,
		const Vector3& light_dir,
		float distance_to_target = 1.0f,
		float z_near = 1.0f,
		float z_far = 1000.0f
	);
	void Update() override;
	void SetLightDirection(const Vector3& light_dir);
private:
	void UpdateCameraTransform();
	ComponentId m_comp_id_camera{};
	CameraShapeConfig m_camera_shape_config{};
	Vector3 m_light_dir{ 0.0f, -1.0f, 0.0f };
	float m_distance_to_target{ 1.0f };
	std::weak_ptr<GameObject> m_target_object{};
};