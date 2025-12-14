#pragma once
#include "Core.h"
#include <string>

class Texture
{
public:
    ID3D12Resource* textureBuffer;
    int heapOffset;

    void load(Core* core, std::string filename);
};
