#include "camera_shadow.h"
#include "component/render/component_camera.h"
#include "object/game_object_list.h"

void CameraShadow::Initialize()
{
	auto& camera = m_components.Add<ComponentCamera>(m_comp_id_camera);
	camera.SetActive(true);
}

void CameraShadow::InitializeConfig(
	const CameraPreset& camera_preset,
	std::weak_ptr<class GameObject> target_object,
	const Vector3& light_dir,
	float distance_to_target,
	float z_near, float z_far
)
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	camera.SetActive(true);

	m_light_dir = light_dir.GetNormalized();
	m_distance_to_target = distance_to_target;
	m_target_object = target_object;

	CameraUsageConfig config{};
	config.type = CameraType::SHADOW; // TODO: shadow camera type
	config.render_layer = camera_preset.render_layer;
	config.render_order = camera_preset.render_order;
	config.render_camera_key = camera_preset.name;
	config.width = camera_preset.width;
	config.height = camera_preset.height;
	config.render_path_id = camera_preset.render_path_id;

	CameraShapeConfig shape_config{};
	shape_config.shape_type = CameraShapeType::ORTHOGRAPHIC;
	shape_config.fov = camera_preset.height * 0.5f / 3.0f; //TODO
	shape_config.aspect_ratio = static_cast<float>(camera_preset.width) / static_cast<float>(camera_preset.height);
	shape_config.z_near = z_near;
	shape_config.z_far = z_far;
	camera.InitializeCamera(config, shape_config);

	UpdateCameraTransform();
}

void CameraShadow::Update()
{
	UpdateCameraTransform();
}

void CameraShadow::SetLightDirection(const Vector3& light_dir)
{
	m_light_dir = light_dir.GetNormalized();
	UpdateCameraTransform();
}

void CameraShadow::UpdateCameraTransform()
{
	auto& camera = m_components.Get<ComponentCamera>(m_comp_id_camera);
	auto target = m_target_object.lock();
	if (!target)
	{
		camera.SetActive(false);
		return;
	}
	camera.SetActive(true);
	const Vector3 target_pos = target->GetTransform().GetPositionGlobal();
	const Vector3 camera_pos = target_pos - m_light_dir * m_distance_to_target;
	m_transform.SetPosition(camera_pos);

	camera.SetTarget(target_pos);
	camera.SetUp({0.0f, 1.0f, 0.0f});
}