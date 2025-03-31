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

struct Triangle {
	vec4 v0, v1, v2;  // Use vec4 instead of vec3
	vec4 normal;      // Use vec4 instead of vec3
	int materialIndex;
};

struct Mesh {
	vec4 minBounds, maxBounds;  // Use vec4 instead of vec3
	int startTriangleIndex;
	int triangleCount;
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

// Function prototypes
Ray GenerateRay();
HitPayload TraceRay(Ray r);
bool TriangleHit(Ray r, Triangle tri, float tMin, float tMax, out float hitDist);
bool AABBHit(Ray r, vec3 minBounds, vec3 maxBounds, float tMin, float tMax, out float hitDist);
HitPayload Miss(Ray r);
vec3 RandomVector(vec2 uv);

void main() {
	vec3 res = vec3(0.0);
	Ray r = GenerateRay();
	HitPayload payload = TraceRay(r);

	if (payload.hitDistance > 0.0) {
		if (payload.materialIndex == 0) {
			// Debug triangle - green
			FragColor = vec4(0.0, 1.0, 0.0, 1.0);
		}
		else if (payload.materialIndex == 3) {
			// Debug AABB - blue
			FragColor = vec4(0.0, 0.0, 1.0, 1.0);
		}
		else if (payload.materialIndex == 4) {
			// Mesh AABB - purple
			FragColor = vec4(1.0, 0.0, 1.0, 1.0);
		}
		else {
			// Other object - red
			FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		}
	} else {
		// Miss - gray
		FragColor = vec4(0.5, 0.5, 0.5, 1.0);
	}
	return;

	for (int s = 0; s < NumberOfSamples; s++) {
		vec3 bounce_res = vec3(0.0);
		vec3 ray_color = vec3(1.0);

		Ray r = GenerateRay();

		for (int b = 0; b <= NumberOfBounces; b++) {
			HitPayload payload = TraceRay(r);
			if (payload.hitDistance < 0.0) break; // No hit

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

Ray GenerateRay() {
	// Map screen space to NDC
	vec4 ndc = vec4(aFragCoord * 2.0 - 1.0, -1.0, 1.0); // Using -1 for z (near plane)

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

	// Debug: Check our mesh count
	if (MeshCount == 0 || TriangleCount == 0) {
		// Debug info - return a hit with red color
		closestHit.hitDistance = 1.0;
		closestHit.materialIndex = 1; // Use a material index that causes red 
		closestHit.worldPosition = r.origin + r.direction;
		closestHit.worldNormal = vec3(1.0, 0.0, 0.0);
		return closestHit;
	}

	Triangle debugTri;
	debugTri.v0 = vec4(-1.0, -1.0, -3.0, 1.0);
	debugTri.v1 = vec4(1.0, -1.0, -3.0, 1.0);
	debugTri.v2 = vec4(0.0, 1.0, -3.0, 1.0);
	debugTri.normal = vec4(0.0, 0.0, 1.0, 1.0);
	debugTri.materialIndex = 0;

	float debugDist = 0.0;
	if (TriangleHit(r, debugTri, 0.001, closestHit.hitDistance, debugDist)) {
		closestHit.hitDistance = debugDist;
		closestHit.materialIndex = 0;
		closestHit.worldPosition = r.origin + r.direction * debugDist;
		closestHit.worldNormal = debugTri.normal.xyz;
		// Always return the debug triangle if we hit it
		return closestHit;
	}

	for (int i = 0; i < MeshCount; i++) {
		Mesh mesh = Meshes[i];
		float hitDist = 0.0;

		// ONLY test AABB, skip triangle tests
		if (AABBHit(r, mesh.minBounds.xyz, mesh.maxBounds.xyz, 0.001, closestHit.hitDistance, hitDist)) {
			closestHit.hitDistance = hitDist;
			closestHit.materialIndex = 4; // Special index for mesh AABB
			closestHit.worldPosition = r.origin + r.direction * hitDist;

			// Simple normal calculation
			vec3 hitPoint = closestHit.worldPosition;
			vec3 center = (mesh.minBounds.xyz + mesh.maxBounds.xyz) * 0.5;
			closestHit.worldNormal = normalize(hitPoint - center);

			// Found a mesh AABB hit, return immediately
			return closestHit;
		}
	}

	if (TriangleCount > 0) {
		// Sample first 5 triangles at most
		int samplesToCheck = max(0, TriangleCount);

		for (int i = 0; i < samplesToCheck; i++) {
			float hitDist = 0.0;
			if (TriangleHit(r, Triangles[i], 0.001, closestHit.hitDistance, hitDist)) {
				closestHit.hitDistance = hitDist;
				closestHit.materialIndex = 5; // Special index for sample triangle
				closestHit.worldPosition = r.origin + r.direction * hitDist;
				closestHit.worldNormal = Triangles[i].normal.xyz;
				// Found a triangle hit, no need to continue
				return closestHit;
			}
		}
	}
	// Replace your mesh loop using count uniforms instead of .length()
	for (int i = 0; i < MeshCount; i++) {
		Mesh mesh = Meshes[i];

		float hitDist = 0.0;
		if (!AABBHit(r, mesh.minBounds.xyz, mesh.maxBounds.xyz, 0.001, closestHit.hitDistance, hitDist)) {
			continue;
		}

		hitDist = 0.0;
		for (int j = 0; j < mesh.triangleCount; j++) {
			int triIndex = mesh.startTriangleIndex + j;
			if (triIndex >= TriangleCount) continue; // Bounds check

			if (TriangleHit(r, Triangles[triIndex], 0.001, closestHit.hitDistance, hitDist)) {
				if (hitDist > 0.0 && hitDist < closestHit.hitDistance) {
					closestHit.hitDistance = hitDist;
					closestHit.materialIndex = Triangles[triIndex].materialIndex;
					closestHit.worldPosition = r.origin + r.direction * hitDist;
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
	float txmin = (minBounds.x - r.origin.x) / r.direction.x;
	float txmax = (maxBounds.x - r.origin.x) / r.direction.x;
	if (txmin > txmax) { float temp = txmin; txmin = txmax; txmax = temp; }

	float tymin = (minBounds.y - r.origin.y) / r.direction.y;
	float tymax = (maxBounds.y - r.origin.y) / r.direction.y;
	if (tymin > tymax) { float temp = tymin; tymin = tymax; tymax = temp; }

	if (txmin > tymax || tymin > txmax) return false;
	float tmin = max(txmin, tymin);
	float tmax = min(txmax, tymax);

	float tzmin = (minBounds.z - r.origin.z) / r.direction.z;
	float tzmax = (maxBounds.z - r.origin.z) / r.direction.z;
	if (tzmin > tzmax) { float temp = tzmin; tzmin = tzmax; tzmax = temp; }

	if (tmin > tzmax || tzmin > tmax) return false;
	tmin = max(tmin, tzmin);
	tmax = min(tmax, tzmax);

	if (tmin < 0 && tmax < 0) return false;  // Box is behind the ray

	hitDist = (tmin >= 0) ? tmin : tmax;  // Use tmax when inside the box
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
