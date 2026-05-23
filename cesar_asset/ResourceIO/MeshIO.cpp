#include "MeshIO.h"
#include "../../cesar_core/Core/MathConstants.h"
#include "../ResourceCache.h"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#include <meshoptimizer/meshoptimizer.h>

using namespace cesar;

namespace cesar {

    std::unique_ptr<Resource> MeshIO::LoadFromFile(ResourceLoadDesc& load_desc)
    {
        std::unique_ptr<Mesh> mesh_resource = std::make_unique<Mesh>();

        LinearAllocator<SubMeshData>* submesh_data_allocator = resource_cache->GetMeshAllocator();
        LinearAllocator<Vertex>*   vertex_allocator    = resource_cache->GetVertexAllocator();
        LinearAllocator<Uint32>*   index_allocator     = resource_cache->GetIndexAllocator();
        LinearAllocator<Meshlet>* meshlet_allocator = resource_cache->GetMeshletAllocator();
        LinearAllocator<Uint32>* meshlet_vertex_allocator = resource_cache->GetMeshletVertexAllocator();
        LinearAllocator<Uint32>* meshlet_triangle_allocator = resource_cache->GetMeshletTriangleAllocator();

        const Uint32 global_vertex_start         = vertex_allocator->GetOffset();
        const Uint32 global_index_start          = index_allocator->GetOffset();
        const Uint32 global_meshlet_start        = meshlet_allocator->GetOffset();
        const Uint32 global_meshlet_vertex_start = meshlet_vertex_allocator->GetOffset();
        const Uint32 global_meshlet_triangle_start = meshlet_triangle_allocator->GetOffset();

        std::vector<Vertex>      vertices;
        std::vector<Uint32>      indices;
        std::vector<SubMeshData> submesh_data;

        const std::string file_extension = load_desc.file_path.extension().string();
        if (file_extension == ".gltf" || file_extension == ".glb" || file_extension == ".obj") {
            //FastGLTF Loader is not setting transforms
            //LoadGLTF(load_desc, vertices, indices, submesh_data, mesh_resource->default_materials, mesh_resource->submesh_names);
            LoadWithAssimp(load_desc, vertices, indices, submesh_data, mesh_resource.get());
        }
        else if (false) {
            LOG_FATAL("Cannot load OBJ Models.");
            CESAR_FEATURE_NO_IMPL("Loading OBJ Models has not been implemented yet");
            return nullptr;
        }

        //I don't understand what is happening properly will check out later. 5/22/2026
        //OptimizeMesh(submesh_data, vertices, indices);

        std::vector<Meshlet> meshlets;
        std::vector<Uint32> meshlet_vertices;
        std::vector<Uint32> meshlet_triangles;
        GenerateMeshlets(submesh_data, vertices, indices, meshlets, meshlet_vertices, meshlet_triangles);

        for (auto& submesh : submesh_data)
        {
            submesh.vertex_start += global_vertex_start;
            submesh.index_start += global_index_start;

            submesh.meshlet_start += global_meshlet_start;
            submesh.meshlet_vertice_start += global_meshlet_vertex_start;
            submesh.meshlet_triangle_start += global_meshlet_triangle_start;
        }

        MemoryBlock<SubMeshData> submesh_data_block = submesh_data_allocator->Allocate(submesh_data.size());
        MemoryBlock<Vertex>      vertex_block       = vertex_allocator->Allocate(vertices.size());
        MemoryBlock<Uint32>      index_block        = index_allocator->Allocate(indices.size());

        MemoryBlock<Meshlet> meshlet_block          = meshlet_allocator->Allocate(meshlets.size());
        MemoryBlock<Uint32>  meshlet_vertice_block  = meshlet_vertex_allocator->Allocate(meshlet_vertices.size());
        MemoryBlock<Uint32>  meshlet_triangle_block = meshlet_triangle_allocator->Allocate(meshlet_triangles.size());

        CopyToMemoryBlock(vertex_block,       vertices);
        CopyToMemoryBlock(index_block,        indices);
        CopyToMemoryBlock(submesh_data_block, submesh_data);
        
        CopyToMemoryBlock(meshlet_block,          meshlets);
        CopyToMemoryBlock(meshlet_vertice_block,  meshlet_vertices);
        CopyToMemoryBlock(meshlet_triangle_block, meshlet_triangles);

        mesh_resource->submesh_start      = submesh_data_allocator->GetIndex(submesh_data_block);
        mesh_resource->submesh_data_count = static_cast<Uint32>(submesh_data.size());

        return mesh_resource;
    }

    void MeshIO::SaveToDisk(const std::filesystem::path& file_path)
    {}

    Vector2 MeshIO::OctEncode(const DirectX::XMFLOAT3& n)
    {
        DirectX::XMVECTOR v = DirectX::XMVector3Normalize(XMLoadFloat3(&n));

        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, v);

        float invL1Norm = 1.0f / (fabsf(f.x) + fabsf(f.y) + fabsf(f.z));

        f.x *= invL1Norm;
        f.y *= invL1Norm;
        f.z *= invL1Norm;

        DirectX::XMFLOAT2 enc = { f.x, f.y };

        if (f.z < 0.0f)
        {
            float oldX = enc.x;
            float oldY = enc.y;

            enc.x = (1.0f - fabsf(oldY)) * (oldX >= 0.0f ? 1.0f : -1.0f);
            enc.y = (1.0f - fabsf(oldX)) * (oldY >= 0.0f ? 1.0f : -1.0f);
        }

        return enc;
    }

    Vector3 MeshIO::OctDecode(const DirectX::XMFLOAT2& e)
    {
        DirectX::XMFLOAT3 n;

        n.x = e.x;
        n.y = e.y;
        n.z = 1.0f - fabsf(e.x) - fabsf(e.y);

        if (n.z < 0.0f)
        {
            float oldX = n.x;
            float oldY = n.y;

            n.x = (1.0f - fabsf(oldY)) * (oldX >= 0.0f ? 1.0f : -1.0f);
            n.y = (1.0f - fabsf(oldX)) * (oldY >= 0.0f ? 1.0f : -1.0f);
        }

        DirectX::XMVECTOR v = DirectX::XMVector3Normalize(XMLoadFloat3(&n));

        DirectX::XMFLOAT3 result;
        DirectX::XMStoreFloat3(&result, v);

        return result;
    }

    void MeshIO::OptimizeMesh(std::vector<SubMeshData>& submeshes, std::vector<Vertex>& vertices, std::vector<Uint32>& indices)
    {
        for (const auto& submesh : submeshes)
        {
            std::span submesh_vertices(vertices.data() + submesh.vertex_start, submesh.vertex_count);
            std::span submesh_index(indices.data() + submesh.index_start, submesh.index_count);

            for (auto& idx : submesh_index) idx -= submesh.vertex_start;

            meshopt_optimizeVertexCache(submesh_index.data(), submesh_index.data(), submesh_index.size(), submesh_vertices.size());
            meshopt_optimizeOverdraw(submesh_index.data(), submesh_index.data(), submesh_index.size(), &submesh_vertices.data()->position.x, submesh_vertices.size(), CESAR_SIZEOF(Vertex), 1.05f);
            std::vector<Uint32>remap(submesh_vertices.size());
            meshopt_optimizeVertexFetchRemap(remap.data(), submesh_index.data(), submesh_index.size(), submesh_vertices.size());
            meshopt_remapIndexBuffer(submesh_index.data(), submesh_index.data(), submesh_index.size(), remap.data());
            meshopt_remapVertexBuffer(submesh_vertices.data(), submesh_vertices.data(), submesh_vertices.size(), CESAR_SIZEOF(Vertex), remap.data());

            for (auto& idx : submesh_index) idx += submesh.vertex_start;
        }
    }

    void MeshIO::GenerateMeshlets(std::vector<SubMeshData>& submeshes, std::vector<Vertex>& vertices, std::vector<Uint32>& indices,
        std::vector<Meshlet>& meshlets, std::vector<Uint32>& meshlet_vertices, std::vector<Uint32>& meshlet_triangles)
    {
        for (auto& submesh : submeshes)
        {
            std::span submesh_vertices(vertices.data() + submesh.vertex_start, submesh.vertex_count);
            std::span submesh_index(indices.data() + submesh.index_start, submesh.index_count);

            for (auto& idx : submesh_index) idx -= submesh.vertex_start;

            const Uint32 max_meshlet_count = meshopt_buildMeshletsBound(submesh_index.size(), MAX_MESHLET_VERTICE, MAX_MESHLET_TRIANGLE);

            const Uint32 meshlet_vertex_count = meshlet_vertices.size();

            std::vector<meshopt_Meshlet> opt_meshlets(max_meshlet_count);
            std::vector<Uint8> opt_triangles(max_meshlet_count * MAX_MESHLET_TRIANGLE);
            meshlet_vertices.resize(meshlet_vertex_count + (max_meshlet_count * MAX_MESHLET_VERTICE));

            const Uint32 meshlet_count = meshopt_buildMeshlets(opt_meshlets.data(), meshlet_vertices.data() + meshlet_vertex_count,
                opt_triangles.data(), submesh_index.data(), submesh_index.size(), &submesh_vertices.data()->position.x,
                submesh_vertices.size(), CESAR_SIZEOF(Vertex), MAX_MESHLET_VERTICE, MAX_MESHLET_TRIANGLE, 0.5f);
            opt_meshlets.resize(meshlet_count);

            const Uint32 submesh_meshlets_count = meshlets.size();
            const Uint32 submesh_meshlets_triangle_count = meshlet_triangles.size();
            meshlets.resize(submesh_meshlets_count + meshlet_count);
            for (Uint32 i = 0; i < meshlet_count; i++)
            {
                const meshopt_Meshlet& opt_meshlet = opt_meshlets[i];
                Meshlet& meshlet = meshlets[submesh_meshlets_count + i];

                const Uint32 meshlet_triangle_count = meshlet_triangles.size();
                meshlet_triangles.resize(meshlet_triangle_count + (opt_meshlet.triangle_count * 3));
                for (Uint32 j = 0; j < opt_meshlet.triangle_count; j++)
                {
                    meshlet_triangles[meshlet_triangle_count + j * 3 + 0] = opt_triangles[opt_meshlet.triangle_offset + j * 3 + 0];
                    meshlet_triangles[meshlet_triangle_count + j * 3 + 1] = opt_triangles[opt_meshlet.triangle_offset + j * 3 + 1];
                    meshlet_triangles[meshlet_triangle_count + j * 3 + 2] = opt_triangles[opt_meshlet.triangle_offset + j * 3 + 2];
                }

                meshlet.center = { 0.0f, 0.0f, 0.0f };
                meshlet.radius = 1.0f;

                //relative to submesh
                meshlet.vertex_count = opt_meshlet.vertex_count;
                meshlet.vertex_start = opt_meshlet.vertex_offset;
                
                meshlet.triangle_count = opt_meshlet.triangle_count;
                meshlet.triangle_start = opt_meshlet.triangle_offset;
            }

            //relative to mesh
            meshopt_Meshlet& last_meshlet = opt_meshlets[meshlet_count - 1];
            meshlet_vertices.resize(meshlet_vertex_count + (last_meshlet.vertex_offset + last_meshlet.vertex_count));

            submesh.meshlet_start = submesh_meshlets_count;
            submesh.meshlet_count = meshlet_count;
            submesh.meshlet_vertice_start = meshlet_vertex_count;
            submesh.meshlet_triangle_start = submesh_meshlets_triangle_count;

            for (auto& idx : submesh_index) idx += submesh.vertex_start;
        }
    }

    void MeshIO::LoadGLTF(ResourceLoadDesc& load_desc, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submesh_data, std::vector<Uint32>& mtl, std::vector<std::string>& submesh_name)
    {
        fastgltf::Parser parser;

        auto data = fastgltf::GltfDataBuffer::FromPath(load_desc.file_path);
        if (data.error() != fastgltf::Error::None) {
            LOG_ERROR("Failed to load GLTF file.");
            return;
        }

        std::string directory = load_desc.file_path.parent_path().string();
        auto asset = parser.loadGltf(data.get(), directory.c_str(),
            fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::LoadExternalImages);

        if (asset.error() != fastgltf::Error::None) {
            LOG_ERROR("Failed to parse GLTF asset.");
            return;
        }

        Uint32 submesh_count = 0;
        for (auto& mesh : asset->meshes)
            submesh_count += mesh.primitives.size();

        submesh_data.resize(submesh_count);
        submesh_name.resize(submesh_count);
        mtl.resize(submesh_count);

        Uint32 submesh_index = 0;

        for (auto& mesh : asset->meshes) {
            Uint32 primitive_index = 0;
            for (auto& primitive : mesh.primitives) {

                const size_t vertex_offset = vertices.size();
                const size_t index_offset = indices.size();

                auto& submesh = submesh_data[submesh_index];

                BoundingSphere sm_bounding_sphere{};

                auto* posAttr = primitive.findAttribute("POSITION");
                if (posAttr == primitive.attributes.end()) {
                    ++submesh_index;
                    continue;
                }

                auto& posAccessor = asset->accessors[posAttr->accessorIndex];
                const size_t prim_vertex_count = posAccessor.count;

                vertices.resize(vertex_offset + prim_vertex_count);

                fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset.get(), posAccessor,
                    [&](fastgltf::math::fvec3 pos, size_t i) {
                        vertices[vertex_offset + i].position = { pos[0], pos[1], pos[2] };
                    });

                sm_bounding_sphere = RitterSphere(vertices, vertex_offset, prim_vertex_count);

                submesh.vertex_start = vertex_offset;
                submesh.vertex_count = prim_vertex_count;

                // --- NORMAL ---
                auto* normAttr = primitive.findAttribute("NORMAL");
                if (normAttr != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset.get(), asset->accessors[normAttr->accessorIndex],
                        [&](fastgltf::math::fvec3 n, size_t i) {
                            vertices[vertex_offset + i].normal = { n[0], n[1], n[2] };
                        });
                }

                // --- TEXCOORD_0 ---
                auto* uvAttr = primitive.findAttribute("TEXCOORD_0");
                if (uvAttr != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset.get(), asset->accessors[uvAttr->accessorIndex],
                        [&](fastgltf::math::fvec2 uv, size_t i) {
                            vertices[vertex_offset + i].uv = { uv[0], uv[1] };
                        });
                }

                // --- TANGENT (xyzw, w = handedness) ---
                auto* tanAttr = primitive.findAttribute("TANGENT");
                if (tanAttr != primitive.attributes.end()) {
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(asset.get(), asset->accessors[tanAttr->accessorIndex],
                        [&](fastgltf::math::fvec4 t, size_t i) {
                            vertices[vertex_offset + i].tangent = { t[0], t[1], t[2], t[3] };
                        });
                }

                if (primitive.indicesAccessor.has_value()) {
                    auto& accessor = asset->accessors[primitive.indicesAccessor.value()];
                    submesh.index_count = accessor.count;
                    submesh.index_start = index_offset;

                    indices.reserve(index_offset + accessor.count);

                    fastgltf::iterateAccessor<uint32_t>(asset.get(), accessor,
                        [&](uint32_t idx) {
                            indices.push_back((Uint32)idx + (Uint32)vertex_offset);
                        });
                }
                else {
                    submesh.index_start = index_offset;
                    submesh.index_count = prim_vertex_count;

                    indices.reserve(index_offset + prim_vertex_count);

                    for (size_t i = 0; i < prim_vertex_count; i++)
                        indices.push_back((Uint32)(vertex_offset + i));
                }

                submesh.bounding_sphere     = sm_bounding_sphere;
                submesh_name[submesh_index] = std::format("{}_{}", mesh.name, primitive_index++);
                mtl[submesh_index]          = 0;

                ++submesh_index;
            }
        }

        auto* material_allocator = resource_cache->GetMaterialAllocator();
        for (Uint32 i = 0; i < asset->materials.size(); i++)
        {
            struct _data_
            {
                const void* data;
                Uint32 index;
            };
            _data_ data{
                .data = &asset,
                .index = i
            };

            MaterialLoadDesc material_load_desc{};
            material_load_desc.flags = MaterialLoadFlags::LoadFromGlb_Gltf;
            material_load_desc.payload = &data;
            material_load_desc.no_path = true;
            material_load_desc.file_path = std::format("{}_material_{}", load_desc.file_path.stem().string(), i);
            Material* material = resource_cache->LoadResource<Material>(material_load_desc);
            MemoryBlock<MaterialData> mtl_block(material->material_data, 1);
            mtl[i] = material_allocator->GetIndex(mtl_block);
        }
    }

    DirectX::XMMATRIX GetWorldTransform(aiNode* node)
    {
        aiMatrix4x4 world = node->mTransformation;
        aiNode* parent = node->mParent;

        while (parent)
        {
            world = parent->mTransformation * world;
            parent = parent->mParent;
        }

        DirectX::XMFLOAT4X4 m(
            world.a1, world.a2, world.a3, world.a4,
            world.b1, world.b2, world.b3, world.b4,
            world.c1, world.c2, world.c3, world.c4,
            world.d1, world.d2, world.d3, world.d4
        );

        return DirectX::XMLoadFloat4x4(&m);
    }

    void MeshIO::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, 
        std::vector<SubMeshData>& submeshes, std::vector<std::string>& submesh_names,std::vector<Matrix>& model_matrix, ResourceLoadDesc & load_desc)
    {
        Matrix world = GetWorldTransform(node);

        for (uint32_t i = 0; i < node->mNumMeshes; i++) {

            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            model_matrix.push_back(world);

            /*
            {
                ResourceLoadDesc loadDesc{};

                aiReturn status;

                aiColor4D diffuseColor;
                std::string cTexturePath;
                aiString texturePath;

                loadDesc.desc.imageTextureDesc.type = MapType::Albedo;
                if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
                    material->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);
                cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                auto albedoResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);

                loadDesc.desc.imageTextureDesc.type = MapType::Normal;
                Resource::Ref normalResource;
                if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
                {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    normalResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else  if (material->GetTexture(aiTextureType_HEIGHT, 0, &texturePath) == AI_SUCCESS) {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    normalResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else  if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &texturePath) == AI_SUCCESS) {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    normalResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else {
                    normalResource = resourceCache->GetDefaultImageTexture(MapType::Normal);
                }


                loadDesc.desc.imageTextureDesc.type = MapType::Roughness;
                Resource::Ref roughnessResource;
                if (material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texturePath) == AI_SUCCESS) {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    roughnessResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else if (material->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS) {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    roughnessResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else {
                    roughnessResource = resourceCache->GetDefaultImageTexture(MapType::Roughness);
                }

                loadDesc.desc.imageTextureDesc.type = MapType::AO;
                Resource::Ref aoResource;
                if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS) {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    aoResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else if (material->GetTexture(aiTextureType_LIGHTMAP, 0, &texturePath) == AI_SUCCESS) {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    aoResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else {
                    aoResource = resourceCache->GetDefaultImageTexture(MapType::AO);
                }


                loadDesc.desc.imageTextureDesc.type = MapType::Metalloic;
                status = material->GetTexture(aiTextureType_METALNESS, 0, &texturePath);
                Resource::Ref metallicResource;
                if (status == AI_SUCCESS)
                {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    metallicResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else if (material->GetTexture(aiTextureType_SPECULAR, 0, &texturePath) == AI_SUCCESS)
                {
                    cTexturePath = modelDirectory.string() + "/" + texturePath.C_Str();
                    metallicResource = resourceCache->LoadResource<ImageTexture>(cTexturePath, loadDesc);
                }
                else {
                    metallicResource = resourceCache->GetDefaultImageTexture(MapType::Metalloic);
                }

                aiString materialName = material->GetName();
                std::string materialPath = std::string(materialName.C_Str()) + std::string(".toml");

                //CreateNew Sets Default Values.
                auto newMaterialResource = resourceCache->materialLoader->CreateNew(materialName.C_Str());
                Wiley::ResourceCache::ResourceDesc newMtlDesc = {
                    .type = ResourceType::Material,
                    .path = materialPath,
                    .state = ResourceState::NotOnDisk
                };
                resourceCache->Cache(newMaterialResource, newMtlDesc, WILEY_INVALID_UUID);
                const auto newMtlUUID = newMaterialResource->GetUUID();

                resourceCache->SetMaterialMap(newMtlUUID, albedoResource->GetUUID(), Wiley::MapType::Albedo);
                resourceCache->SetMaterialMap(newMtlUUID, normalResource->GetUUID(), Wiley::MapType::Normal);
                resourceCache->SetMaterialMap(newMtlUUID, aoResource->GetUUID(), Wiley::MapType::AO);
                resourceCache->SetMaterialMap(newMtlUUID, roughnessResource->GetUUID(), Wiley::MapType::Roughness);
                resourceCache->SetMaterialMap(newMtlUUID, metallicResource->GetUUID(), Wiley::MapType::Metalloic);

                Material* mtl = static_cast<Material*>(newMaterialResource.get());
                auto mtlData = mtl->dataPtr;

                material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
                material->Get(AI_MATKEY_METALLIC_FACTOR, mtlData->metallic.value);
                material->Get(AI_MATKEY_ROUGHNESS_FACTOR, mtlData->roughness.value);
                material->Get(AI_MATKEY_BUMPSCALING, mtlData->normal.strength);

                meshData.loadMaterials.push_back(newMtlUUID);
            }
            */

            SubMeshData submesh{};
            std::string name = mesh->mName.C_Str();
            submesh_names.push_back(name);

            submesh.vertex_count = mesh->mNumVertices;
            submesh.vertex_start = vertices.size();
            submesh.index_start = indices.size();
                        
            submesh.bounding_sphere = {};

            for (int v = 0; v < mesh->mNumVertices; v++) {
                Vertex vertex = {};

                {
                    vertex.position.x = mesh->mVertices[v].x;
                    vertex.position.y = mesh->mVertices[v].y;
                    vertex.position.z = mesh->mVertices[v].z;
                }



                if (mesh->HasNormals())
                {
                    vertex.normal.x = mesh->mNormals[v].x;
                    vertex.normal.y = mesh->mNormals[v].y;
                    vertex.normal.z = mesh->mNormals[v].z;
                }

                if (mesh->mTextureCoords[0])
                {
                    vertex.uv.x = mesh->mTextureCoords[0][v].x;
                    vertex.uv.y = mesh->mTextureCoords[0][v].y;
                }

                if (mesh->HasTangentsAndBitangents())
                {
                    DirectX::XMVECTOR N = DirectX::XMVectorSet(
                        mesh->mNormals[v].x,
                        mesh->mNormals[v].y,
                        mesh->mNormals[v].z,
                        0.0f
                    );

                    DirectX::XMVECTOR T = DirectX::XMVectorSet(
                        mesh->mTangents[v].x,
                        mesh->mTangents[v].y,
                        mesh->mTangents[v].z,
                        0.0f
                    );

                    DirectX::XMVECTOR assimpB = DirectX::XMVectorSet(
                        mesh->mBitangents[v].x,
                        mesh->mBitangents[v].y,
                        mesh->mBitangents[v].z,
                        0.0f
                    );

                    DirectX::XMVECTOR calculatedB = DirectX::XMVector3Cross(N, T);

                    float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(calculatedB, assimpB));
                    float handedness = (dotProduct < 0.0f) ? -1.0f : 1.0f;

                    vertex.tangent.x = mesh->mTangents[v].x;
                    vertex.tangent.y = mesh->mTangents[v].y;
                    vertex.tangent.z = mesh->mTangents[v].z;
                    vertex.tangent.w = handedness;


                }

                vertices.push_back(vertex);
            }

            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {
                aiFace face = mesh->mFaces[f];
                for (unsigned int idx = 0; idx < face.mNumIndices; idx++)
                {
                    indices.push_back(static_cast<UINT>(submesh.vertex_start) + face.mIndices[idx]);
                }
            }

            submesh.index_count = indices.size() - submesh.index_start;
            submeshes.push_back(submesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, vertices, indices, submeshes, submesh_names, model_matrix, load_desc);
        }
    }


    void MeshIO::LoadWithAssimp(ResourceLoadDesc& load_desc, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submeshes, Mesh* mesh_resource)
    {
        Assimp::Importer importer;

        unsigned int flags = aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_FlipUVs ;

        const aiScene* scene = importer.ReadFile(load_desc.file_path.string().c_str(), flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            return;

        aiNode* root_node = scene->mRootNode;
        mesh_resource->model_matrix = GetWorldTransform(root_node);

        ProcessNode(root_node, scene, vertices, indices, submeshes, mesh_resource->submesh_names, mesh_resource->model_matrixes, load_desc);
    }

    BoundingSphere MeshIO::RitterSphere(const std::vector<Vertex>& vertices, size_t vertex_offset, size_t vertex_count)
    {
        if (vertex_count == 0) return {};

        auto Len = [](Vector3 v) {
            return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            };

        Vector3 first = vertices[vertex_offset].position;

        auto FarthestFrom = [&](Vector3 origin) -> Vector3 {
            Vector3 farthest = first;
            float maxDist = 0.0f;
            for (size_t i = vertex_offset; i < vertex_offset + vertex_count; ++i) {
                Vector3 d = { vertices[i].position.x - origin.x,
                           vertices[i].position.y - origin.y,
                           vertices[i].position.z - origin.z };
                float dist = Len(d);
                if (dist > maxDist) { maxDist = dist; farthest = vertices[i].position; }
            }
            return farthest;
            };

        Vector3 A = FarthestFrom(first);
        Vector3 B = FarthestFrom(A);

        Vector3  center = { (A.x + B.x) * 0.5f, (A.y + B.y) * 0.5f, (A.z + B.z) * 0.5f };
        float radius = Len({ B.x - A.x, B.y - A.y, B.z - A.z }) * 0.5f;

        for (size_t i = vertex_offset; i < vertex_offset + vertex_count; ++i) {
            Vector3 diff = { vertices[i].position.x - center.x,
                          vertices[i].position.y - center.y,
                          vertices[i].position.z - center.z };
            float dist = Len(diff);
            if (dist > radius) {
                float newRadius = (radius + dist) * 0.5f;
                float scale = (dist - newRadius) / dist;
                center.x += diff.x * scale;
                center.y += diff.y * scale;
                center.z += diff.z * scale;
                radius = newRadius;
            }
        }

        return { center, radius };
    }

    BoundingSphere MeshIO::MergeSpheres(BoundingSphere a, BoundingSphere b)
    {
        Vector3  d = { b.center.x - a.center.x, b.center.y - a.center.y, b.center.z - a.center.z };
        float dist = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);

        if (dist + b.radius <= a.radius) return a;
        if (dist + a.radius <= b.radius) return b;

        float newRadius = (a.radius + dist + b.radius) * 0.5f;
        float scale = (newRadius - a.radius) / dist;
        return {
            { a.center.x + d.x * scale, a.center.y + d.y * scale, a.center.z + d.z * scale },
            newRadius
        };
    }

}