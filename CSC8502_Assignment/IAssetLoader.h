#pragma once

#include <string>
#include <memory>

struct MeshData;

class IAssetLoader {
public:

	virtual ~IAssetLoader() = default;

	virtual std::shared_ptr<MeshData> loadMesh(const std::string& filePath) = 0;
	virtual std::shared_ptr<MeshData> loadAnimation(const std::string& filePath) = 0;
};