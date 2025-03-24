#include "Mesh.h"

#include <fstream>
#include <chrono>

Mesh::Mesh(const std::string& filename, const int&& material_index) : Object(glm::vec3(1.0f), material_index), BoundingBox(Bounds3f(glm::vec3(0.0f), glm::vec3(0.0f)), std::move(material_index)) {
	LoadFromOBJ(filename);
	// Calculate bounding box
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
	for (const auto& vert : Vertices) {
		min.x = std::min(min.x, vert.x);
		min.y = std::min(min.y, vert.y);
		min.z = std::min(min.z, vert.z);

		max.x = std::max(max.x, vert.x);
		max.y = std::max(max.y, vert.y);
		max.z = std::max(max.z, vert.z);
	}
	printf("Mesh bounding box min: (%f, %f, %f) max: (%f, %f, %f)\n", min.x, min.y, min.z, max.x, max.y, max.z);
	BoundingBox = Box(Bounds3<float>(min, max), std::move(material_index));
}

bool Mesh::Hit(const Ray& r, float tMin, float tMax, float& hitDistance) const
{
	if (!BoundingBox.Hit(r, tMin, tMax, hitDistance))
		return false;

	hitDistance = 0;
	for (const auto& tri : MeshTriangles) {
		float tempHitDistance;
		if (tri.Hit(r, tMin, tMax, tempHitDistance)) {
			if (hitDistance == 0 || tempHitDistance < hitDistance) {
				hitDistance = tempHitDistance;
				return true;
			}
		}
	}
	return false;
}

bool Mesh::LoadFromOBJ(const std::string& filename) {
	std::chrono::time_point <std::chrono::high_resolution_clock> start, end;
	start = std::chrono::high_resolution_clock::now();
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open " << filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v") {
			glm::vec3 vertex;
			iss >> vertex.x >> vertex.y >> vertex.z;
			Vertices.push_back(vertex);
		} else if (type == "f") {
			glm::vec3 triangle;
			std::string v1, v2, v3;
			iss >> v1 >> v2 >> v3;

			// Extract just the vertex indices (before the first '/')
                	triangle.x = std::stoi(v1.substr(0, v1.find('/'))) - 1;
                	triangle.y = std::stoi(v2.substr(0, v2.find('/'))) - 1;
                	triangle.z = std::stoi(v3.substr(0, v3.find('/'))) - 1;

			Triangles.push_back(triangle);
		}
	}
	file.close();

	for (const auto& tri : Triangles) {
		if (tri.x >= Vertices.size() || tri.y >= Vertices.size() || tri.z >= Vertices.size()) {
			fprintf(stderr, "Index out of bounds in OBJ file: Vertices[%d], Vertices[%d], Vertices[%d] | Vertices.size() == %zu", 
				(int)tri.x, (int)tri.y, (int)tri.z, Vertices.size());
			return false;
		}
		glm::vec3 v[3] = { Vertices[(int)tri.x], Vertices[(int)tri.y], Vertices[(int)tri.z] };
		MeshTriangles.emplace_back(v, MaterialIndex);
	}
	end = std::chrono::high_resolution_clock::now();
	long duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	printf("Loaded %zu vertices and %zu triangles from %s in %ldus\n", Vertices.size(), Triangles.size(), filename.c_str(), duration);
	return true;
}
