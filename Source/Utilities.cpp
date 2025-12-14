#include "Utilities.h"
#include "Gameobject.h"

STATIC_VERTEX createStaticVertex(Vec3 pos, Vec3 normal, float tu, float tv)
{
	STATIC_VERTEX v;
	v.pos = pos;
	v.normal = normal;
	Frame frame;
	frame.fromVector(normal);
	v.tangent = frame.u;
	v.tu = tu;
	v.tv = tv;
	return v;
}

void loadTexturesForModel(AnimatedModel& model, Core* core, 
	std::vector<GEMLoader::GEMMesh>& gemmeshes, 
	std::function<std::string(int meshIndex, const std::string& defaultName)> textureResolver)
{
	for (int i = 0; i < gemmeshes.size(); i++)
	{
		std::string defaultTextureName = gemmeshes[i].material.find("albedo").getValue("");
		std::string textureName = textureResolver(i, defaultTextureName);
		
		Texture* tex = new Texture();
		tex->load(core, textureName);
		model.textures.push_back(tex);
		
		std::string normalMapName = textureName;
		size_t pos = normalMapName.find("_alb.png");
		if (pos != std::string::npos)
		{
			normalMapName.replace(pos, 8, "_nh.png");
		}
		else
		{
			pos = normalMapName.find("_ALB.png");
			if (pos != std::string::npos)
			{
				normalMapName.replace(pos, 8, "_NH.png");
			}
		}
		
		Texture* normalTex = new Texture();
		normalTex->heapOffset = -1;
		
		std::ifstream file(normalMapName);
		if (file.good())
		{
			file.close();
			normalTex->load(core, normalMapName);
		}
		
		model.normalTextures.push_back(normalTex);
	}
}

void loadTexturesForModel(StaticModel& model, Core* core, 
	std::vector<GEMLoader::GEMMesh>& gemmeshes, 
	std::function<std::string(int meshIndex, const std::string& defaultName)> textureResolver)
{
	for (int i = 0; i < gemmeshes.size(); i++)
	{
		std::string defaultTextureName = gemmeshes[i].material.find("albedo").getValue("");
		std::string textureName = textureResolver(i, defaultTextureName);
		
		Texture* tex = new Texture();
		tex->load(core, textureName);
		model.textures.push_back(tex);
		
		std::string normalMapName = textureName;
		size_t pos = normalMapName.find("_alb.png");
		if (pos != std::string::npos)
		{
			normalMapName.replace(pos, 8, "_nh.png");
		}
		else
		{
			pos = normalMapName.find("_ALB.png");
			if (pos != std::string::npos)
			{
				normalMapName.replace(pos, 8, "_NH.png");
			}
		}
		
		Texture* normalTex = new Texture();
		normalTex->heapOffset = -1;
		
		std::ifstream file(normalMapName);
		if (file.good())
		{
			file.close();
			normalTex->load(core, normalMapName);
		}
		
		model.normalTextures.push_back(normalTex);
	}
}

