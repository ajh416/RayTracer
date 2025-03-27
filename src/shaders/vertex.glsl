#version 460

out vec2 aFragCoord;

void main() {
    // Define the triangle vertices in normalized device coordinates (NDC)
    vec2 vertices[6] = vec2[](
        vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), // First triangle
        vec2(-1.0, 1.0), vec2(1.0, -1.0), vec2(1.0, 1.0)    // Second triangle
    );

    // Pass the NDC to the fragment shader directly as frag coords
    gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0); // Position vertices in NDC space
    aFragCoord = vertices[gl_VertexID]; // Pass NDC directly to fragment shader
}
