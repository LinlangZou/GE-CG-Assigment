#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include "Core.h"
#include <iostream>

void Texture::load(Core* core, std::string filename)
{
    int width, height, channels;
    unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    unsigned char* data = texels;
    if (channels == 3) {
        channels = 4;
        unsigned char* texelsWithAlpha = new unsigned char[width * height * 4];
        for (int i = 0; i < width * height; i++) {
            texelsWithAlpha[i * 4] = texels[i * 3];
            texelsWithAlpha[i * 4 + 1] = texels[i * 3 + 1];
            texelsWithAlpha[i * 4 + 2] = texels[i * 3 + 2];
            texelsWithAlpha[i * 4 + 3] = 255;
        }
        stbi_image_free(texels);
        data = texelsWithAlpha;
    }

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    core->device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&textureBuffer));

    UINT64 size;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    core->device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &footprint, nullptr, nullptr, &size);

    core->uploadResource(textureBuffer, data, size, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &footprint);

    if (data != texels) delete[] data; else stbi_image_free(texels);

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = core->srvHeap.getNextCPUHandle();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    core->device->CreateShaderResourceView(textureBuffer, &srvDesc, cpuHandle);

    heapOffset = core->srvHeap.used - 1;
}

