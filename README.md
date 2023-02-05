# RayTracer made by Adam Henry [https://github.com/ah416]

## Currently supports spheres and boxes as the only shapes, with support for triangle meshes in the works.
### Usage
The main.cpp file provides an example of the usage of the RayTracer.

### Scene Management
First create a Scene object, which holds the shapes and materials.
Second, add shapes to the vector of shapes. For example: `scene.Shapes.push_back(new Sphere({ 0.0, 0.0, -1.0 }, 0, 0));` where the first argument in the Sphere constructor is the position (Vector3), the second is the radius, and the third is the material index (next section).
Third, add materials using `scene.Materials.push_back(Material({ .Albedo = {0.3, 0.3, 0.8}, .Roughness = 0.1, .Metallic = 0.0 }))`. These materials are accessed using the material index passed to the shapes in their constructor, using zero-based indexing.