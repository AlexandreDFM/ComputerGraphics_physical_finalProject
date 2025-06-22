#include "tiny_obj_loader.h" // correct

// appel
tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;
std::string warn, err;

bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, "Models/apartment.obj");
