/**
* @file B_GLTFMesh.cpp
 * @brief Implementation of the GLTF-backed mesh adapter.
 */
#include "B_GLTFMesh.h"

#include <utility>

namespace NCLGL_Impl {

    B_GLTFMesh::B_GLTFMesh(std::shared_ptr<GLTFScene> scene)
        : m_scene(std::move(scene)) {
    }

    B_GLTFMesh::~B_GLTFMesh() = default;

    void B_GLTFMesh::Draw() {
        if (!m_scene) {
            return;
        }

        for (const auto& mesh : m_scene->meshes) {
            if (mesh) {
                mesh->Draw();
            }
        }
    }

}