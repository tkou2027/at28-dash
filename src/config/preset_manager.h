#pragma once
#include <string>
#include <memory>
#include "render/config/sprite_desc.h"
#include "render/config/model_desc.h"
#include "util/csv_loader.h"

template <typename T>
class ResourcePool;

class PresetManager
{
public:
	PresetManager();
	~PresetManager();
	void Initialize();
	void Finalize() {}; // TODO: release tetures?
	void LoadPresets();
	const ModelDesc& GetModelDesc(const std::string& key) const;
	const SpriteDesc& GetSpriteDesc(const std::string& key) const;
	const TextureResourceId& GetTextureId(const std::string& key) const;
	const CSVData& GetCSVData(const std::string& key) const;
private:
	void LoadSpritePresets();
	void LoadMeshGeometryPresets();
	void LoadModelPresets();
	void LoadTexturePresets();
	void LoadDataPresets();

	void RegisterModelDesc(const std::string& key, const ModelDesc& desc);
	void RegisterAndLoadModelDesc(const std::string& key, ModelDesc& desc);
	void RegisterAndLoadSpriteDesc(const std::string& key, SpriteDesc& desc);
	void RegisterTexture(const std::string& key, const TextureResourceId& texture_id);
	void RegisterCSVData(const std::string& key, const std::string& filename);

	std::unique_ptr<ResourcePool<ModelDesc> > m_model_pool; // name -> model desc
	std::unique_ptr<ResourcePool<SpriteDesc> > m_sprite_pool; // name -> sprite desc
	std::unique_ptr<ResourcePool<TextureResourceId> > m_texture_pool; // name -> texture id ?
	std::unique_ptr<ResourcePool<CSVData> > m_csv_pool; // name -> csv data
};