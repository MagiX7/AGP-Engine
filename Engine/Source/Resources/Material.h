#pragma once

#include "../Renderer/Texture2D.h"
#include "../Renderer/Shader.h"

#include <string>
#include <glm/vec3.hpp>

#include <memory>

class Material
{
public:
	Material(std::string materialName);
	virtual ~Material();

	void Bind();
	void Unbind();

	inline void SetName(const std::string& value) { name = value; }
	inline const std::string GetName() { return name; }

	inline void SetAlbedoColor(const glm::vec3& albedo) { albedoColor = albedo; }
	inline const glm::vec3& GetAlbedoColor() const { return albedoColor; }
	inline glm::vec3& GetAlbedoColor() { return albedoColor; }

	inline void SetEmissiveColor(const glm::vec3& emissive) { emissiveColor = emissive; }
	inline const glm::vec3& GetEmissiveColor() { return emissiveColor; }

	inline void SetSmoothness(float value) { smoothness = value; }
	inline float GetSmoothness() { return smoothness; }

	inline void SetAlbedoMap(const std::shared_ptr<Texture2D> albedoTexture) { albedoMap = albedoTexture; }
	inline const std::shared_ptr<Texture2D>& GetAlbedoMap() { return albedoMap; }

	inline void SetMetallicMap(const std::shared_ptr<Texture2D> metallicTexture) { metallicMap = metallicTexture; }
	inline const std::shared_ptr<Texture2D>& GetMetallicMap() { return metallicMap; }

	inline void SetEmissiveMap(const std::shared_ptr<Texture2D> emissiveTexture) { emissiveMap = emissiveTexture; }
	inline const std::shared_ptr<Texture2D>& GetEmissiveMap() { return emissiveMap; }

	inline void SetRoughnessMap(const std::shared_ptr<Texture2D> roughnessTexture) { roughnessMap = roughnessTexture; }
	inline const std::shared_ptr<Texture2D>& GetRoughnessMap() { return roughnessMap; }

	inline void SetNormalMap(const std::shared_ptr<Texture2D> normalTexture) { normalMap = normalTexture; }
	inline const std::shared_ptr<Texture2D>& GetNormalMap() { return normalMap; }

	inline void SetBumpMap(const std::shared_ptr<Texture2D> bumpTexture) { bumpMap = bumpTexture; }
	inline const std::shared_ptr<Texture2D>& GetBumpMap() { return bumpMap; }
	

private:
	std::string name;

	std::shared_ptr<Shader> shader;

	glm::vec3 albedoColor;
	glm::vec3 emissiveColor;
	float smoothness;

	std::shared_ptr<Texture2D> albedoMap;
	std::shared_ptr<Texture2D> metallicMap;
	std::shared_ptr<Texture2D> roughnessMap;
	std::shared_ptr<Texture2D> normalMap;


	std::shared_ptr<Texture2D> emissiveMap;
	std::shared_ptr<Texture2D> bumpMap;

};