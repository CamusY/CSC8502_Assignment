/**
 * @file B_GLTFMesh.h
 * @brief I_Mesh adapter that renders meshes loaded via GLTFLoader.
 *
 * The class owns a shared GLTFScene instance and issues draw calls for
 * each mesh contained in the scene. This allows the renderer to work with
 * complex GLTF assets without exposing the underlying nclgl::Mesh type.
 */
#pragma once

#include "IAL/I_Mesh.h"

#include "nclgl/Extra/GLTFLoader.h"

#include <memory>

namespace NCLGL_Impl {

    class B_GLTFMesh : public Engine::IAL::I_Mesh {
    public:
        explicit B_GLTFMesh(std::shared_ptr<GLTFScene> scene);
        ~B_GLTFMesh() override;

        void Draw() override;

    private:
        std::shared_ptr<GLTFScene> m_scene;
    };

}