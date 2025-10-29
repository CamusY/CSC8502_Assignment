#pragma once
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>


constexpr int MAX_BONES_PER_VERTEX = 4;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;

	int boneIDs[MAX_BONES_PER_VERTEX] = { 0 };
	float boneWeights[MAX_BONES_PER_VERTEX] = { 0.0f };

};

struct SubMesh {
	std::vector<unsigned int> indices;
	unsigned int materialIndex = 0;
	std::string name;

	SubMesh() = default;
};

struct MeshData {
	std::vector<Vertex> vertices;

	std::map<std::string, unsigned int> boneNameMap;
	std::vector<glm::mat4> inverseBindPoses;
	unsigned int boneCount = 0;

	std::string sourcePath;

	std::vector<SubMesh> subMeshes;

	MeshData() = default;
};