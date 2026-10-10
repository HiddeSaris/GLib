#include "GltfLoader.h"

#include <iostream>

#include <glad/glad.h>
#include <stb_image.h>

#include <fastgltf/tools.hpp>
#include <fastgltf/glm_element_traits.hpp>


namespace GLib {

    std::shared_ptr<fastgltf::Asset> GltfLoader::LoadAsset(const std::filesystem::path& path) const {
        fastgltf::Parser parser;

        auto buffer = fastgltf::GltfDataBuffer::FromPath(path);
        if (buffer.error() != fastgltf::Error::None) {
            std::cout << "Error: [GltfLoader::LoadData]: " << fastgltf::getErrorMessage(buffer.error()) << "\n";
        }

        auto options = fastgltf::Options::LoadExternalBuffers |
                       fastgltf::Options::LoadExternalImages;

        auto asset = parser.loadGltf(buffer.get(), path.parent_path(), options);

        if (asset.error() != fastgltf::Error::None) {
            std::cout << "Error: [GltfLoader::LoadData]: " << fastgltf::getErrorMessage(asset.error()) << "\n";
        }

        return std::make_shared<fastgltf::Asset>(std::move(asset.get()));
    }

    void GltfLoader::LoadMaterials(std::shared_ptr<fastgltf::Asset> asset, ImportedModel &model) const {
        for (size_t i = 0; i < asset->materials.size(); i++) {
            auto& material = asset->materials[i];
            auto& pbr = material.pbrData;
            MaterialData mat;

            mat.baseColor = glm::vec4(
                pbr.baseColorFactor.x(), 
                pbr.baseColorFactor.y(), 
                pbr.baseColorFactor.z(), 
                pbr.baseColorFactor.w());
            
            mat.metallic = pbr.metallicFactor;
            mat.roughness = pbr.roughnessFactor;

            if (pbr.baseColorTexture.has_value()) {
                mat.albedoTexIndex = pbr.baseColorTexture->textureIndex;
            }
            if (material.normalTexture.has_value()) {
                mat.normalTexIndex = material.normalTexture->textureIndex;
            }
            if (pbr.metallicRoughnessTexture.has_value()) {
                mat.metallicRoughnessTexIndex = pbr.metallicRoughnessTexture->textureIndex;
            }
            if (material.occlusionTexture.has_value()) {
                mat.aoTexIndex = material.occlusionTexture->textureIndex;
            }

            model.materials.push_back(std::move(mat));
        }
    }

    void GltfLoader::LoadTextures(std::shared_ptr<fastgltf::Asset> asset, ImportedModel& model, const std::filesystem::path& baseDir) const {
        for (const fastgltf::Texture& tex : asset->textures) {
            const fastgltf::Image& image = asset->images[tex.imageIndex.value()];

            TextureData data;
            std::visit(fastgltf::visitor{
                [&](const fastgltf::sources::URI& uri) {
                    data.uri = baseDir.string() + "/" + std::string(uri.uri.path());
                    data.isEmbedded = false;
                },
                [&](const fastgltf::sources::Array& arr) {
                    data.pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(arr.bytes.data()), static_cast<int>(arr.bytes.size()), &data.width, &data.height, &data.channels, 4);
                    data.isEmbedded = true;
                },
                [&](const fastgltf::sources::BufferView& bufView) {
                    const auto& view = asset->bufferViews[bufView.bufferViewIndex];
                    const auto& buffer = asset->buffers[view.bufferIndex];
                    std::visit(fastgltf::visitor{
                        [&](const fastgltf::sources::Array& arr) {
                            data.pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(arr.bytes.data() + view.byteOffset),
					                                            static_cast<int>(view.byteLength), &data.width, &data.height, &data.channels, 4);
                        },
                        [](auto&) { /* unsupported buffer source */ }
                    }, buffer.data);
                    data.isEmbedded = true;
                },
                [](auto&) { /* unsupported image source */ }
            }, image.data);
            model.textures.push_back(std::move(data));
        }
    }

    void GltfLoader::LoadMeshes(std::shared_ptr<fastgltf::Asset> asset, ImportedModel& model) const {
        for (int i = 0; i < asset->meshes.size(); i++) {
            const fastgltf::Mesh& mesh = asset->meshes[i];
            model.primitives.push_back({});
            for (const fastgltf::Primitive& prim : mesh.primitives) {
                MeshData data;
                if (prim.materialIndex.has_value()) {
                    data.materialIndex = prim.materialIndex.value();
                }
                
                auto* posIt = prim.findAttribute("POSITION");
                auto* normIt = prim.findAttribute("NORMAL");
                auto* uvIt = prim.findAttribute("TEXCOORD_0");
                auto* colorIt = prim.findAttribute("COLOR_0");

                const auto& posAccessor   = asset->accessors[posIt->accessorIndex];
                data.vertices.resize(posAccessor.count * 12); // 3(pos) + 3(norm) + 2(uv) + 4(color)
                
                // position (3 floats)
                fastgltf::iterateAccessorWithIndex<glm::vec3>(*asset, posAccessor, 
                    [&](glm::vec3 pos, size_t i) {
                        data.vertices[i * 12 + 0] = pos.x;
                        data.vertices[i * 12 + 1] = pos.y;
                        data.vertices[i * 12 + 2] = pos.z;
                    });

                // normal (3 floats)
                if (normIt != prim.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec3>(*asset, asset->accessors[normIt->accessorIndex], 
                    [&](glm::vec3 n, size_t i) {
                        data.vertices[i * 12 + 3] = n.x;
                        data.vertices[i * 12 + 4] = n.y;
                        data.vertices[i * 12 + 5] = n.z;
                    });
                }
                else {
                    for (size_t i = 0; i < posAccessor.count; ++i) {
                        data.vertices[i * 12 + 3] = 0.0f;
                        data.vertices[i * 12 + 4] = 1.0f;
                        data.vertices[i * 12 + 5] = 0.0f;
                    }
                }

                // uv (2 floats)
                if (uvIt != prim.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec2>(*asset, asset->accessors[uvIt->accessorIndex], 
                    [&](glm::vec2 uv, size_t i) {
                        data.vertices[i * 12 + 6] = uv.x;
                        data.vertices[i * 12 + 7] = uv.y;
                    });
                }
                else {
                    for (size_t i = 0; i < posAccessor.count; i++) {
                        data.vertices[i * 12 + 6] = 0.0f;
                        data.vertices[i * 12 + 7] = 0.0f;
                    }
                }

                // color (4 floats)
                if (colorIt != prim.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<glm::vec4>(*asset, asset->accessors[colorIt->accessorIndex], 
                    [&](glm::vec4 color, size_t i) {
                        data.vertices[i * 12 + 8]  = color.x;
                        data.vertices[i * 12 + 9]  = color.y;
                        data.vertices[i * 12 + 10] = color.z;
                        data.vertices[i * 12 + 11] = color.w;
                    });
                }
                else {
                    for (size_t i = 0; i < posAccessor.count; i++) {
                        data.vertices[i * 12 + 8]  = 1.0f;
                        data.vertices[i * 12 + 9]  = 1.0f;
                        data.vertices[i * 12 + 10] = 1.0f;
                        data.vertices[i * 12 + 11] = 1.0f;
                    }
                }

                // indices
                if (prim.indicesAccessor.has_value()) {
                    const auto& idxAccessor = asset->accessors[prim.indicesAccessor.value()];
                    data.indices.resize(idxAccessor.count);
                    fastgltf::iterateAccessorWithIndex<uint32_t>(*asset, idxAccessor, 
                    [&](uint32_t idx, size_t i) {
                        data.indices[i] = idx;
                    });
                }
                model.primitives[i].push_back(std::move(data));
            }
        }
    }

    void GltfLoader::GetNodeTransform(const fastgltf::Node& node, glm::vec3& scale, glm::quat& rotation, glm::vec3& translation) const {
        if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform)) {
            translation = glm::vec3(trs->translation[0], trs->translation[1], trs->translation[2]);
            rotation = glm::quat(trs->rotation[3], trs->rotation[0], trs->rotation[1], trs->rotation[2]); // glTF is x,y,z,w; glm ctor is w,x,y,z
            scale = glm::vec3(trs->scale[0], trs->scale[1], trs->scale[2]);
        }
        if (auto* mat = std::get_if<fastgltf::math::fmat4x4>(&node.transform)) {
            glm::mat4 m = glm::make_mat4(mat->data());
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(m, scale, rotation, translation, skew, perspective);
        }
    }

    void GltfLoader::LoadNodes(std::shared_ptr<fastgltf::Asset> asset, ImportedModel &model) const {
        for (fastgltf::Node node : asset->nodes) {
            NodeData data;
            
            data.name = node.name;

            GetNodeTransform(node, data.translation, data.rotation, data.scale);

            if (node.meshIndex.has_value()) {
                data.meshIndex = node.meshIndex.value();
            }

            for (auto child : node.children) {
                data.children.push_back(static_cast<uint32_t>(child));
            }

            model.nodes.push_back(std::move(data));
        }
    }

    void GltfLoader::LoadScenes(std::shared_ptr<fastgltf::Asset> asset, ImportedModel &model) const {
        for (auto scene : asset->scenes) {
            SceneData data;

            data.name = scene.name;
            for (uint32_t node : scene.nodeIndices) {
                data.rootNodes.push_back(node);
            }
            model.scenes.push_back(std::move(data));
        }
    }

    std::any GltfLoader::LoadData(const std::filesystem::path &path) const {
        auto asset = LoadAsset(path);
        auto model = std::make_shared<ImportedModel>();
        
        LoadMaterials(asset, *model);
        LoadTextures(asset, *model, path.parent_path());
        LoadMeshes(asset, *model);
        LoadNodes(asset, *model);
        LoadScenes(asset, *model);
        model->defaultScene = asset->defaultScene;

        return model;
    }


    std::any GltfLoader::InitAsset(std::any data) const {
        auto imported = std::any_cast<std::shared_ptr<ImportedModel>>(data);
        auto model = std::make_shared<Gltf>();

        std::vector<Handle<Texture>> textures;
        for (auto& tex : imported->textures) {
            if (tex.isEmbedded) {
                TextureSpecification spec(tex.width, tex.height, tex.channels == 4 ? GL_RGBA : GL_RGB, tex.channels == 4 ? GL_RGBA8 : GL_RGB8, TextureType::Albedo, true);
                auto texture = std::make_shared<Texture>(spec);
                texture->SetData(tex.pixels, tex.width * tex.height * tex.channels);
                stbi_image_free(tex.pixels);
                textures.push_back(AssetManager::Add<Texture>(texture));
            }
            else {
                textures.push_back(AssetManager::Load<Texture>(tex.uri));
            }
        }

        for (auto& mat : imported->materials) {
            auto data = std::make_shared<GltfMaterial>();
            
            data->baseColor = mat.baseColor;
            if (mat.albedoTexIndex != -1) {
                data->baseColorTexture = textures[mat.albedoTexIndex];
            }
            data->metallic = mat.metallic;
            data->rougness = mat.roughness;
            if (mat.metallicRoughnessTexIndex != -1) {
                data->metallicRoughnessTexture = textures[mat.metallicRoughnessTexIndex];
            }
            if (mat.normalTexIndex != -1) {
                data->normalTexture = textures[mat.normalTexIndex];
            }
            if (mat.aoTexIndex != -1) {
                data->ambientOcclusionTexture = textures[mat.aoTexIndex];
            }
            model->materials.push_back(AssetManager::Add<GltfMaterial>(data));
        }

        for (int i = 0; i < imported->primitives.size(); i++) {
            auto mesh = std::make_shared<GltfMesh>();
            for (auto prim : imported->primitives[i]) {
                GltfPrimitive primitive;
                
                auto m = std::make_shared<Mesh>(prim.vertices.data(), (uint32_t)prim.vertices.size(), prim.indices.data(), (uint32_t)prim.indices.size());
                primitive.mesh = AssetManager::Add<Mesh>(m);
                if (prim.materialIndex != -1) {
                    primitive.material = model->materials[prim.materialIndex];
                }
                mesh->primitives.push_back(primitive);
            }
            model->meshes.push_back(AssetManager::Add(mesh));
        }
    

        for (auto node : imported->nodes) {
            auto data = std::make_shared<GltfNode>();
            data->name = node.name;
            if (node.meshIndex != -1) {
                data->mesh = model->meshes[node.meshIndex];
            }

            data->translation = node.translation;
            data->rotation = node.rotation;
            data->scale = node.scale;
            model->nodes.push_back(AssetManager::Add(data));
        }
        for (int i = 0; i < imported->nodes.size(); i++) {
            for (uint32_t child : imported->nodes[i].children) {
                AssetManager::Get(model->nodes[i])->children.push_back(model->nodes[child]);
            }
        }

        for (auto scene : imported->scenes) {
            auto data = std::make_shared<GltfScene>();
            data->name = scene.name;

            for (uint32_t root : scene.rootNodes) {
                data->rootNodes.push_back(model->nodes[root]);
            }
            model->scenes.push_back(AssetManager::Add(data));
        }

        return model;
    }
}