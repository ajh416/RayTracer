#version 460

out vec4 FragColor;

in vec2 aFragCoord; // computed here

uniform sampler2D RandomTexture;
uniform vec3 CameraPosition;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform int NumberOfSamples;
uniform int NumberOfBounces;
uniform int TriangleCount;
uniform int MeshCount;
uniform float Time; // For random seed

// CPU to GPU struct
struct Triangle {
	vec4 v0, v1, v2;  // Use vec4 instead of vec3
	vec4 normal;      // Use vec4 instead of vec3
	int materialIndex;
	int padding[3];   // Add padding for alignment
};

// CPU to GPU struct
struct Mesh {
	vec4 minBounds, maxBounds;  // Use vec4 instead of vec3
	int startTriangleIndex;
	int triangleCount;
	int padding[2];  // Add padding for alignment
};

// CPU to GPU struct
struct Material {
	vec3 albedo;
	float roughness;
	vec3 emissionColor;
	float emissionStrength;
	float metallic;
	float padding[3];  // Add padding for alignment
};

struct Ray {
	vec3 origin;
	vec3 direction;
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

layout(std430, binding = 2) buffer MaterialsBuffer {
	Material Materials[];
};

// Function prototypes
Ray GenerateRay();
HitPayload TraceRay(Ray r);
bool TriangleHit(Ray r, Triangle tri, float tMin, float tMax, out float hitDist);
bool AABBHit(Ray r, vec3 minBounds, vec3 maxBounds, float tMin, float tMax, out float hitDist);
HitPayload Miss(Ray r);
vec3 RandomVector(vec2 uv);
uint pcg_hash(uint seed);
float RandomFloat(inout uint state);
vec3 RandomInUnitSphere(inout uint state);
vec3 GetEnvironmentColor(vec3 dir);

void main() {
	// Initialize RNG state based on fragment coordinate and time
	uint state = uint(aFragCoord.x * 1973.0 + aFragCoord.y * 9277.0 + Time * 26699.0) | 1u;

	vec3 finalColor = vec3(0.0);

	// Debug visualization - remove when path tracing is confirmed working
	if (NumberOfSamples == 1 && NumberOfBounces == 0) {
		Ray ray = GenerateRay();
		HitPayload payload = TraceRay(ray);

		if (payload.hitDistance > 0.0) {
			if (payload.materialIndex >= 0 && payload.materialIndex < 128) {
				// Use actual material albedo
				Material mat = Materials[payload.materialIndex];
				FragColor = vec4(mat.albedo, 1.0);
			} else {
				// Other object - red
				FragColor = vec4(1.0, 0.0, 0.0, 1.0);
			}
		} else {
			// Miss - sky color
			FragColor = vec4(GetEnvironmentColor(ray.direction), 1.0);
		}
		return;
	}

	// Full path tracing with multiple bounces
	for (int s = 0; s < NumberOfSamples; s++) {
		vec3 throughput = vec3(1.0);  // Light attenuation
		Ray ray = GenerateRay();

		for (int b = 0; b < NumberOfBounces; b++) {
			HitPayload payload = TraceRay(ray);

			if (payload.hitDistance < 0.0) {
				// No hit - add environment contribution
				finalColor += throughput * GetEnvironmentColor(ray.direction);
				break;
			}

			// Get material of hit object
			Material material;
			if (payload.materialIndex >= 0 && payload.materialIndex < 128) {
				material = Materials[payload.materialIndex];
			} else {
				// Default material if index is invalid
				material.albedo = vec3(0.8, 0.8, 0.8);
				material.roughness = 0.5;
				material.emissionColor = vec3(0.0);
				material.emissionStrength = 0.0;
				material.metallic = 0.0;
			}

			// Handle light emission
			finalColor += throughput * material.emissionColor * material.emissionStrength;

			// Update throughput with material albedo
			throughput *= material.albedo;

			// Russian Roulette termination
			float p = max(throughput.r, max(throughput.g, throughput.b));
			if (b > 3 && RandomFloat(state) > p) {
				break; // Terminate path
			}
			if (p < 1.0) throughput /= p; // Compensate for terminated paths

			// Prepare for next bounce
			vec3 normal = normalize(payload.worldNormal);

			// Calculate new ray direction based on material properties
			vec3 reflectDir = reflect(ray.direction, normal);
			vec3 diffuseDir = normalize(normal + RandomInUnitSphere(state));

			// Use metallic to choose between reflection and diffuse
			vec3 newDir;
			if (RandomFloat(state) < material.metallic) {
				// Reflective path with roughness perturbation
				newDir = normalize(mix(reflectDir, normalize(reflectDir + material.roughness * RandomInUnitSphere(state)), material.roughness));
			} else {
				// Diffuse path
				newDir = diffuseDir;
			}

			// Set up new ray for next bounce
			ray.origin = payload.worldPosition + normal * 0.001; // Offset to avoid self-intersection
			ray.direction = newDir;
		}
	}

	// Average samples and apply simple tone mapping
	finalColor = finalColor / float(NumberOfSamples);

	// Simple Reinhard tone mapping
	finalColor = finalColor / (finalColor + vec3(1.0));

	// Gamma correction
	finalColor = pow(finalColor, vec3(1.0 / 2.2));

	FragColor = vec4(finalColor, 1.0);
}

Ray GenerateRay() {
	// aFragCoord is already in NDC space (-1 to 1) from the vertex shader
	vec4 ndc = vec4(aFragCoord, -1.0, 1.0); // Using -1 for z (near plane)

	// Transform to view space using inverse projection 
	vec4 viewSpace = ProjectionMatrix * ndc;

	// Perform perspective division to get view-space position
	viewSpace /= viewSpace.w;

	// Get ray direction in view space (from origin to this point)
	vec3 viewRayDir = normalize(viewSpace.xyz);

	// Transform direction to world space
	vec3 worldRayDir = normalize((ViewMatrix * vec4(viewRayDir, 0.0)).xyz);

	return Ray(CameraPosition, worldRayDir);
}

HitPayload TraceRay(Ray r) {
	HitPayload closestHit;
	closestHit.hitDistance = 1e10;
	closestHit.materialIndex = -1;

	// Test mesh bounding boxes first for optimization
	for (int i = 0; i < MeshCount; i++) {
		Mesh mesh = Meshes[i];
		float hitDist = 0.0;

		if (!AABBHit(r, mesh.minBounds.xyz, mesh.maxBounds.xyz, 0.001, closestHit.hitDistance, hitDist)) {
			continue; // Skip if ray doesn't hit bounding box
		}

		// Test triangles in this mesh
		for (int j = 0; j < mesh.triangleCount; j++) {
			int triIndex = mesh.startTriangleIndex + j;
			if (triIndex >= TriangleCount) continue; // Bounds check

			float dist = 0.0;
			if (TriangleHit(r, Triangles[triIndex], 0.001, closestHit.hitDistance, dist)) {
				if (dist > 0.0 && dist < closestHit.hitDistance) {
					closestHit.hitDistance = dist;
					closestHit.materialIndex = Triangles[triIndex].materialIndex;
					closestHit.worldPosition = r.origin + r.direction * dist;
					closestHit.worldNormal = Triangles[triIndex].normal.xyz;
				}
			}
		}
	}

	return (closestHit.materialIndex < 0) ? Miss(r) : closestHit;
}

bool TriangleHit(Ray r, Triangle tri, float tMin, float tMax, out float hitDist) {
	vec3 edge1 = tri.v1.xyz - tri.v0.xyz;
	vec3 edge2 = tri.v2.xyz - tri.v0.xyz;
	vec3 h = cross(r.direction, edge2);
	float a = dot(edge1, h);

	if (abs(a) < 0.0001) return false; // Parallel

	float f = 1.0 / a;
	vec3 s = r.origin - tri.v0.xyz;
	float u = f * dot(s, h);
	if (u < 0.0 || u > 1.0) return false;

	vec3 q = cross(s, edge1);
	float v = f * dot(r.direction, q);
	if (v < 0.0 || u + v > 1.0) return false;

	hitDist = f * dot(edge2, q);
	return (hitDist > tMin && hitDist < tMax);
}

bool AABBHit(Ray r, vec3 minBounds, vec3 maxBounds, float tMin, float tMax, out float hitDist) {
	// Ultra-simple slab test
	vec3 invDir = 1.0 / r.direction;
	vec3 t0 = (minBounds - r.origin) * invDir;
	vec3 t1 = (maxBounds - r.origin) * invDir;

	vec3 tmin = min(t0, t1);
	vec3 tmax = max(t0, t1);

	float tenter = max(tmin.x, max(tmin.y, tmin.z));
	float texit = min(tmax.x, min(tmax.y, tmax.z));

	if (tenter > texit || texit < 0) return false;

	hitDist = (tenter > 0) ? tenter : texit;
	return (hitDist >= tMin && hitDist <= tMax);
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

// PCG hash function for random number generation
uint pcg_hash(uint seed) {
	uint state = seed * 747796405u + 2891336453u;
	uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

// Generate random float between 0 and 1
float RandomFloat(inout uint state) {
	state = pcg_hash(state);
	return float(state) / 4294967295.0;
}

// Generate random unit vector
vec3 RandomInUnitSphere(inout uint state) {
	float z = RandomFloat(state) * 2.0 - 1.0;
	float t = RandomFloat(state) * 2.0 * 3.14159265;
	float r = sqrt(max(0.0, 1.0 - z * z));
	float x = r * cos(t);
	float y = r * sin(t);
	return vec3(x, y, z);
}

// Simple sky color function for environment lighting
vec3 GetEnvironmentColor(vec3 dir) {
	// Simple gradient sky
	float t = 0.5 * (dir.y + 1.0);
	return mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t) * 0.5;
}
