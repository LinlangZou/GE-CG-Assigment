#include "Particles.h"
#include "Gameobject.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

static D3D12_INPUT_LAYOUT_DESC getLeavesInputLayout()
{
	static const D3D12_INPUT_ELEMENT_DESC inputLayoutLeaves[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INSTANCE_POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_OFFSET", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};
	static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutLeaves, 6 };
	return desc;
}

void Particles::init(Core* core, PSOManager* psos, Shaders* shaders,
	int leafCount,
	Vec3 distributionVolume,
	float billboardSize)
{
	numLeaves = leafCount;
	this->billboardSize = billboardSize;
	time = 0.0f;
	fallHeight = distributionVolume.y;
	
	std::vector<STATIC_VERTEX> vertices;
	std::vector<unsigned int> indices;
	
	vertices.push_back(createStaticVertex(Vec3(-0.5f, -0.5f, 0), Vec3(0, 0, 1), 0, 1));
	vertices.push_back(createStaticVertex(Vec3(0.5f, -0.5f, 0), Vec3(0, 0, 1), 1, 1));
	vertices.push_back(createStaticVertex(Vec3(-0.5f, 0.5f, 0), Vec3(0, 0, 1), 0, 0));
	vertices.push_back(createStaticVertex(Vec3(0.5f, 0.5f, 0), Vec3(0, 0, 1), 1, 0));
	
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);
	
	billboardMesh.init(core, vertices, indices);
	
	instanceData.clear();
	instanceData.reserve(numLeaves);
	
	srand((unsigned int)::time(nullptr));
	
	for (int i = 0; i < numLeaves; i++)
	{
		LEAF_INSTANCE_DATA leaf;
		leaf.position.x = ((float)rand() / RAND_MAX - 0.5f) * distributionVolume.x;
		leaf.position.y = ((float)rand() / RAND_MAX) * distributionVolume.y;
		leaf.position.z = ((float)rand() / RAND_MAX - 0.5f) * distributionVolume.z;
		leaf.offset = (float)rand() / RAND_MAX * fallHeight;
		instanceData.push_back(leaf);
	}
	
	D3D12_HEAP_PROPERTIES heapProps;
	memset(&heapProps, 0, sizeof(D3D12_HEAP_PROPERTIES));
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;
	
	D3D12_RESOURCE_DESC instanceBufferDesc;
	memset(&instanceBufferDesc, 0, sizeof(D3D12_RESOURCE_DESC));
	instanceBufferDesc.Width = numLeaves * sizeof(LEAF_INSTANCE_DATA);
	instanceBufferDesc.Height = 1;
	instanceBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	instanceBufferDesc.DepthOrArraySize = 1;
	instanceBufferDesc.MipLevels = 1;
	instanceBufferDesc.SampleDesc.Count = 1;
	instanceBufferDesc.SampleDesc.Quality = 0;
	instanceBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	
	HRESULT hr = core->device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&instanceBufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		NULL,
		IID_PPV_ARGS(&instanceBuffer));
	
	core->uploadResource(instanceBuffer, &instanceData[0], numLeaves * sizeof(LEAF_INSTANCE_DATA),
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	
	instanceBufferView.BufferLocation = instanceBuffer->GetGPUVirtualAddress();
	instanceBufferView.StrideInBytes = sizeof(LEAF_INSTANCE_DATA);
	instanceBufferView.SizeInBytes = numLeaves * sizeof(LEAF_INSTANCE_DATA);
	
	texture = new Texture();
	texture->heapOffset = -1;
	texture->load(core, "Models/Textures/banana1_LOD5_ALB.png");
	
	shaderName = "Leaves";
	shaders->load(core, shaderName, "shaders/VSLeaves.txt", "shaders/PSLeaves.txt");
	
	psos->createPSO(core, "LeavesPSO",
		shaders->find(shaderName)->vs,
		shaders->find(shaderName)->ps,
		getLeavesInputLayout(),
		true);
}

void Particles::update(float dt)
{
	time += dt;
}

void Particles::draw(Core* core, PSOManager* psos, Shaders* shaders,
	Matrix& vp, Matrix& viewMatrix)
{
	Matrix R = viewMatrix.transpose();
	
	shaders->updateConstantVS(shaderName, "leavesBuffer", "T", &time);
	shaders->updateConstantVS(shaderName, "leavesBuffer", "H", &fallHeight);
	shaders->updateConstantVS(shaderName, "leavesBuffer", "R", &R);
	shaders->updateConstantVS(shaderName, "leavesBuffer", "VP", &vp);
	shaders->updateConstantVS(shaderName, "leavesBuffer", "billboardSize", &billboardSize);
	
	shaders->apply(core, shaderName);
	psos->bind(core, "LeavesPSO");
	
	if (texture != nullptr && texture->heapOffset >= 0)
	{
		shaders->updateTexturePS(core, shaderName, "tex", texture->heapOffset);
	}
	
	core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	core->getCommandList()->IASetVertexBuffers(0, 1, &billboardMesh.vbView);
	core->getCommandList()->IASetVertexBuffers(1, 1, &instanceBufferView);
	core->getCommandList()->IASetIndexBuffer(&billboardMesh.ibView);
	core->getCommandList()->DrawIndexedInstanced(billboardMesh.numMeshIndices, numLeaves, 0, 0, 0);
}

void Particles::cleanup()
{
	if (instanceBuffer != nullptr)
	{
		instanceBuffer->Release();
		instanceBuffer = nullptr;
	}
	if (texture != nullptr)
	{
		delete texture;
		texture = nullptr;
	}
	billboardMesh.cleanUp();
}

