#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Core.h"
#include "GEMLoader.h"
#include <vector>
#include <string>
#include <functional>
#include <fstream>

class StaticModel;
class AnimatedModel;

STATIC_VERTEX createStaticVertex(Vec3 pos, Vec3 normal, float tu, float tv);

void loadTexturesForModel(AnimatedModel& model, Core* core, 
	std::vector<GEMLoader::GEMMesh>& gemmeshes, 
	std::function<std::string(int meshIndex, const std::string& defaultName)> textureResolver);

void loadTexturesForModel(StaticModel& model, Core* core, 
	std::vector<GEMLoader::GEMMesh>& gemmeshes, 
	std::function<std::string(int meshIndex, const std::string& defaultName)> textureResolver);