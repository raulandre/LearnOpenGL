#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <string>
struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

static std::vector<Texture> textures_loaded;
#endif