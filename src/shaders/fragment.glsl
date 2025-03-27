#version 460

out vec4 FragColor;
in vec2 aFragCoord; // computed here

uniform sampler2D RandomTexture;
uniform vec3 CameraPosition;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform int NumberOfSamples;
uniform int NumberOfBounces;

struct Triangle {
	vec3 v0, v1, v2;  // Triangle vertices
	vec3 normal;
	int materialIndex;
};

struct Mesh {
	vec3 minBounds;         // Bounding box min corner
	vec3 maxBounds;         // Bounding box max corner
	int startTriangleIndex; // Index of first triangle in global triangle array
	int triangleCount;      // Number of triangles in this mesh
};

struct Ray {
	vec3 origin;
	vec3 direction;
};

struct Material {
	vec3 albedo;
	vec3 emissionColor;
	float emissionStrength;
	float roughness;
};

struct HitPayload {
	float hitDistance;
	vec3 worldPosition;
	vec3 worldNormal;
	int materialIndex;
};

layout(std430, binding = 0) buffer TrianglesBuffer {
	Triangle Triangles[];
};

layout(std430, binding = 1) buffer MeshesBuffer {
	Mesh Meshes[];
};

uniform Material Materials[128]; // Assume we have up to 128 materials
uniform int ObjectCount;

// Function prototypes
Ray GenerateRay();
HitPayload TraceRay(Ray r);
bool TriangleHit(Ray r, Triangle tri, float tMin, float tMax, out float hitDist);
bool AABBHit(Ray r, vec3 minBounds, vec3 maxBounds, float tMin, float tMax);
HitPayload Miss(Ray r);
vec3 RandomVector(vec2 uv);

void main() {
	vec3 res = vec3(0.0);

	for (int s = 0; s < NumberOfSamples; s++) {
		vec3 bounce_res = vec3(0.0);
		vec3 ray_color = vec3(1.0);

		Ray r = GenerateRay();

		for (int b = 0; b <= NumberOfBounces; b++) {
			HitPayload payload = TraceRay(r);
			if (payload.hitDistance > 0.0) {
				// If a hit occurs, visualize the intersection point as a red dot
				vec3 hitColor = vec3(1.0, 0.0, 0.0); // Red for hit
				FragColor = vec4(hitColor, 1.0);
				return;
			} else {
				// Otherwise, visualize ray direction as background color (like before)
				FragColor = vec4(abs(r.direction), 1.0);
				return;
			}
			//if (payload.hitDistance < 0.0) break; // No hit

			//Material material = Materials[payload.materialIndex];

			//vec3 emitted_light = material.emissionColor * material.emissionStrength;
			vec3 emitted_light = vec3(1.0);
			bounce_res += emitted_light * ray_color; // Add emission
								 //ray_color *= material.albedo; // Light color attenuation
			ray_color *= vec3(0.4, 0.5, 0.7); // Light color attenuation

			// Apply roughness perturbation
			//vec3 perturbedNormal = normalize(payload.worldNormal + material.roughness * RandomVector(uv));
			vec3 perturbedNormal = normalize(payload.worldNormal + 0.1);
			r.origin = payload.worldPosition + perturbedNormal * 0.0001;
			r.direction = reflect(r.direction, perturbedNormal);
		}

		res += clamp(bounce_res, vec3(0.0), vec3(1.0));
	}

	FragColor = vec4(res / float(NumberOfSamples), 1.0);
}

// Generate a camera ray from screen coordinates
Ray GenerateRay() {
	// Map screen space to NDC
	vec4 ndc = vec4(aFragCoord * 2.0 - 1.0, 0.0, 1.0); // -1 to 1 space

	// Convert to view space using the inverse projection matrix
	vec4 viewSpace = inverse(ProjectionMatrix) * ndc;

	// Normalize the resulting 4D homogeneous coordinates
	viewSpace /= viewSpace.w;

	// Convert to world space (direction vector)
	vec3 rayDirection = normalize((inverse(ViewMatrix) * viewSpace).xyz);

	return Ray(CameraPosition, rayDirection);
}

HitPayload TraceRay(Ray r) {
	HitPayload closestHit;
	closestHit.hitDistance = 1e10;
	closestHit.materialIndex = -1;

	for (int i = 0; i < Meshes.length(); i++) {
		Mesh mesh = Meshes[i];

		// Step 1: AABB Culling
		if (!AABBHit(r, mesh.minBounds, mesh.maxBounds, 0.001, closestHit.hitDistance)) {
			continue;
		}


		closestHit.hitDistance = 5;
		closestHit.materialIndex = 0;
		closestHit.worldPosition = vec3(0, 0, 0);
		closestHit.worldNormal = vec3(0, 0, 0);
		return closestHit;
		
		// Step 2: Iterate through triangles in the mesh
		for (int j = 0; j < mesh.triangleCount; j++) {
			int triIndex = mesh.startTriangleIndex + j;
			float hitDist;
			if (TriangleHit(r, Triangles[triIndex], 0.001, closestHit.hitDistance, hitDist)) {
				if (hitDist > 0.0 && hitDist < closestHit.hitDistance) {
					closestHit.hitDistance = hitDist;
					closestHit.materialIndex = Triangles[triIndex].materialIndex;
					closestHit.worldPosition = r.origin + r.direction * hitDist;
					closestHit.worldNormal = Triangles[triIndex].normal;
				}
			}
		}
	}

	return (closestHit.materialIndex < 0) ? Miss(r) : closestHit;
}

bool TriangleHit(Ray r, Triangle tri, float tMin, float tMax, out float hitDist) {
	vec3 edge1 = tri.v1 - tri.v0;
	vec3 edge2 = tri.v2 - tri.v0;
	vec3 h = cross(r.direction, edge2);
	float a = dot(edge1, h);

	if (abs(a) < 0.0001) return false; // Parallel

	float f = 1.0 / a;
	vec3 s = r.origin - tri.v0;
	float u = f * dot(s, h);
	if (u < 0.0 || u > 1.0) return false;

	vec3 q = cross(s, edge1);
	float v = f * dot(r.direction, q);
	if (v < 0.0 || u + v > 1.0) return false;

	hitDist = f * dot(edge2, q);
	return (hitDist > tMin && hitDist < tMax);
}

bool AABBHit(Ray r, vec3 minBounds, vec3 maxBounds, float tMin, float tMax) {
	vec3 invDir = 1.0 / r.direction;
	vec3 t0 = (minBounds - r.origin) * invDir;
	vec3 t1 = (maxBounds - r.origin) * invDir;
	vec3 tMinVec = min(t0, t1);
	vec3 tMaxVec = max(t0, t1);
	float tNear = max(max(tMinVec.x, tMinVec.y), tMinVec.z);
	float tFar  = min(min(tMaxVec.x, tMaxVec.y), tMaxVec.z);
	return (tNear < tFar) && (tFar > 0.0);
}

HitPayload Miss(Ray r) {
	HitPayload missPayload;
	missPayload.hitDistance = -1.0;
	missPayload.materialIndex = -1;
	return missPayload;
}

// Generate a pseudo-random vector using a noise texture
vec3 RandomVector(vec2 uv) {
	return texture(RandomTexture, uv).rgb * 2.0 - 1.0;
}
