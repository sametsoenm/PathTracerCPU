#include "scene.h"

#define TINYOBJLOADER_DISABLE_FAST_FLOAT
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/gtx/norm.hpp>
#include <iostream>


void Scene::load_obj(const std::string& path,
    const std::shared_ptr<Material>& defaultMaterial,
    const glm::vec3& translation,
    const glm::vec3& scale)
{
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;

    if (!reader.ParseFromFile(path, config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader error: " << reader.Error() << std::endl;
        }
        return;
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader warning: " << reader.Warning() << std::endl;
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    const bool hasNormalsInFile = !attrib.normals.empty();

    std::vector<glm::vec3> accumulatedNormals(attrib.vertices.size() / 3, glm::vec3(0.0f));

    if (!hasNormalsInFile) {
        for (const auto& shape : shapes) {
            size_t index_offset = 0;

            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
                const int fv = shape.mesh.num_face_vertices[f];

                if (fv != 3) {
                    index_offset += fv;
                    continue;
                }

                const tinyobj::index_t idx0 = shape.mesh.indices[index_offset + 0];
                const tinyobj::index_t idx1 = shape.mesh.indices[index_offset + 1];
                const tinyobj::index_t idx2 = shape.mesh.indices[index_offset + 2];

                if (idx0.vertex_index < 0 || idx1.vertex_index < 0 || idx2.vertex_index < 0) {
                    index_offset += fv;
                    continue;
                }

                const int v0i = idx0.vertex_index;
                const int v1i = idx1.vertex_index;
                const int v2i = idx2.vertex_index;

                glm::vec3 v0(
                    attrib.vertices[3 * v0i + 0],
                    attrib.vertices[3 * v0i + 1],
                    attrib.vertices[3 * v0i + 2]
                );

                glm::vec3 v1(
                    attrib.vertices[3 * v1i + 0],
                    attrib.vertices[3 * v1i + 1],
                    attrib.vertices[3 * v1i + 2]
                );

                glm::vec3 v2(
                    attrib.vertices[3 * v2i + 0],
                    attrib.vertices[3 * v2i + 1],
                    attrib.vertices[3 * v2i + 2]
                );

                const glm::vec3 e1 = v1 - v0;
                const glm::vec3 e2 = v2 - v0;
                const glm::vec3 crossResult = glm::cross(e1, e2);

                if (glm::length2(crossResult) < 1e-20f) {
                    index_offset += fv;
                    continue;
                }

                const glm::vec3 faceNormal = glm::normalize(crossResult);

                accumulatedNormals[v0i] += faceNormal;
                accumulatedNormals[v1i] += faceNormal;
                accumulatedNormals[v2i] += faceNormal;

                index_offset += fv;
            }
        }

        for (glm::vec3& n : accumulatedNormals) {
            if (glm::length2(n) > 1e-20f) {
                n = glm::normalize(n);
            }
            else {
                n = glm::vec3(0.0f, 1.0f, 0.0f); 
            }
        }
    }

    
    for (const auto& shape : shapes) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            const int fv = shape.mesh.num_face_vertices[f];

            if (fv != 3) {
                index_offset += fv;
                continue;
            }

            glm::vec3 positions[3];
            glm::vec3 normals[3];

            bool validTriangle = true;
            bool useNormals = true;

            tinyobj::index_t indices[3];

            for (int v = 0; v < 3; ++v) {
                indices[v] = shape.mesh.indices[index_offset + v];
                const tinyobj::index_t idx = indices[v];

                if (idx.vertex_index < 0) {
                    validTriangle = false;
                    break;
                }

                const int vx = 3 * idx.vertex_index + 0;
                const int vy = 3 * idx.vertex_index + 1;
                const int vz = 3 * idx.vertex_index + 2;

                glm::vec3 p(
                    attrib.vertices[vx],
                    attrib.vertices[vy],
                    attrib.vertices[vz]
                );

                p = p * scale + translation;
                positions[v] = p;

                if (hasNormalsInFile) {
                    if (idx.normal_index >= 0) {
                        const int nx = 3 * idx.normal_index + 0;
                        const int ny = 3 * idx.normal_index + 1;
                        const int nz = 3 * idx.normal_index + 2;

                        glm::vec3 n(
                            attrib.normals[nx],
                            attrib.normals[ny],
                            attrib.normals[nz]
                        );

                        normals[v] = glm::normalize(n);
                    }
                    else {
                        useNormals = false;
                    }
                }
                else {
                    normals[v] = accumulatedNormals[idx.vertex_index];
                }
            }

            if (!validTriangle) {
                index_offset += fv;
                continue;
            }

            std::shared_ptr<Triangle> tri;

            if (hasNormalsInFile) {
                if (useNormals) {
                    tri = std::make_shared<Triangle>(
                        positions[0], positions[1], positions[2],
                        normals[0], normals[1], normals[2],
                        defaultMaterial
                    );
                }
                else {
                    tri = std::make_shared<Triangle>(
                        positions[0], positions[1], positions[2],
                        defaultMaterial
                    );
                }
            }
            else {
                tri = std::make_shared<Triangle>(
                    positions[0], positions[1], positions[2],
                    normals[0], normals[1], normals[2],
                    defaultMaterial
                );
            }

            _objects.add(tri);
            index_offset += fv;
        }
    }

    _world = nullptr; 
}