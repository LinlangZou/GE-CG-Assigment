#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shaders.h"
#include "PSO.h"
#include "Maths.h"
#include <vector>
#include <random>

struct LEAF_INSTANCE_DATA
{
	Vec3 position;
	float offset;
};

class Particles
{
public:
	Mesh billboardMesh;
	ID3D12Resource* instanceBuffer;
	D3D12_VERTEX_BUFFER_VIEW instanceBufferView;
	Texture* texture;
	std::string shaderName;
	
	int numLeaves;
	float time;
	float fallHeight;
	float billboardSize;
	
	std::vector<LEAF_INSTANCE_DATA> instanceData;
	
	void init(Core* core, PSOManager* psos, Shaders* shaders, 
		int leafCount = 1000, 
		Vec3 distributionVolume = Vec3(500.0f, 50.0f, 500.0f),
		float billboardSize = 1.2f);
	void update(float dt);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, 
		Matrix& vp, Matrix& viewMatrix);
	void cleanup();
};

