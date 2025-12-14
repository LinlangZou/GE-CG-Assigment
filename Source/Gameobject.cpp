#include "Gameobject.h"
#include "Utilities.h"
#include <cmath>
#include <cstdlib>
#include <fstream>

// ==================== StaticModel 实现 ====================

void StaticModel::load(Core* core, std::string filename, Shaders* shaders, PSOManager* psos, std::string pixelShaderFile)
{
	GEMLoader::GEMModelLoader loader;
	loader.load(filename, gemmeshes);

	for (int i = 0; i < gemmeshes.size(); i++)
	{
		Mesh* mesh = new Mesh();
		std::vector<STATIC_VERTEX> vertices;
		for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++)
		{
			STATIC_VERTEX v;
			memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
			vertices.push_back(v);

		}
		mesh->init(core, vertices, gemmeshes[i].indices);
		meshes.push_back(mesh);
	}

	if (pixelShaderFile == "PSTree.txt")
	if (pixelShaderFile == "PSTree.txt")
	{
		shaderName = "StaticModelTree";
		shaders->load(core, "StaticModelTree", "shaders/VS.txt", "shaders/PSTree.txt");
		psos->createPSO(core, "StaticModelTreePSO",
			shaders->find("StaticModelTree")->vs,
			shaders->find("StaticModelTree")->ps,
			VertexLayoutCache::getStaticLayout(),
			true);
	}
	else if (pixelShaderFile == "PSNormalMapped.txt")
	{
		shaderName = "StaticModelNormalMapped";
		shaders->load(core, "StaticModelNormalMapped", "shaders/VS.txt", "shaders/PSNormalMapped.txt");
		psos->createPSO(core, "StaticModelNormalMappedPSO",
			shaders->find("StaticModelNormalMapped")->vs,
			shaders->find("StaticModelNormalMapped")->ps,
			VertexLayoutCache::getStaticLayout(),
			true);
	}
	else
	{
		shaderName = "StaticModelTextured";
		shaders->load(core, "StaticModelTextured", "shaders/VS.txt", "shaders/PSTextured.txt");
		psos->createPSO(core, "StaticModelTexturedPSO",
			shaders->find("StaticModelTextured")->vs,
			shaders->find("StaticModelTextured")->ps,
			VertexLayoutCache::getStaticLayout(),
			true);
	}
}

void StaticModel::updateWorld(Shaders* shaders, Matrix& w)
{
	shaders->updateConstantVS(shaderName, "staticMeshBuffer", "W", &w);
}

void StaticModel::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp)
{
	shaders->updateConstantVS(shaderName, "staticMeshBuffer", "VP", &vp);
	shaders->apply(core, shaderName);
	
	if (shaderName == "StaticModelTree")
	{
		psos->bind(core, "StaticModelTreePSO");
	}
	else if (shaderName == "StaticModelNormalMapped")
	{
		psos->bind(core, "StaticModelNormalMappedPSO");
	}
	else
	{
		psos->bind(core, "StaticModelTexturedPSO");
	}

	for (int i = 0; i < meshes.size(); i++)
	{
		if (i < textures.size())
		{
			shaders->updateTexturePS(core, shaderName, "tex", textures[i]->heapOffset);
		}
		
		if (shaderName == "StaticModelNormalMapped" && i < normalTextures.size() && normalTextures[i]->heapOffset >= 0)
		{
			shaders->updateTexturePS(core, shaderName, "normalMap", normalTextures[i]->heapOffset);
		}

		meshes[i]->draw(core);
	}
}

// ==================== AnimatedModel 实现 ====================

void AnimatedModel::load(Core* core, std::string filename, PSOManager* psos, Shaders* shaders)
{
	GEMLoader::GEMModelLoader loader;
	GEMLoader::GEMAnimation gemanimation;
	loader.load(filename, gemmeshes, gemanimation);
	
	for (int i = 0; i < gemmeshes.size(); i++)
	{
		Mesh *mesh = new Mesh();
		std::vector<ANIMATED_VERTEX> vertices;
		for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++)
		{
			ANIMATED_VERTEX v;
			memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
			vertices.push_back(v);
		}
		mesh->init(core, vertices, gemmeshes[i].indices);
		meshes.push_back(mesh);
	}
	
	shaders->load(core, "AnimatedTextured", "shaders/VSAnim.txt", "shaders/PSAnimTextured.txt");
	
	psos->createPSO(core, "AnimatedModelTexturedPSO",
		shaders->find("AnimatedTextured")->vs,
		shaders->find("AnimatedTextured")->ps,
		VertexLayoutCache::getAnimatedLayout());
	
	shaders->load(core, "AnimatedNormalMapped", "shaders/VSAnim.txt", "shaders/PSAnimNormalMapped.txt");
	
	psos->createPSO(core, "AnimatedModelNormalMappedPSO",
		shaders->find("AnimatedNormalMapped")->vs,
		shaders->find("AnimatedNormalMapped")->ps,
		VertexLayoutCache::getAnimatedLayout());

	shaders->load(core, "AnimatedUntextured", "shaders/VSAnim.txt", "shaders/PSUntextured.txt");
	psos->createPSO(core, "AnimatedModelPSO", shaders->find("AnimatedUntextured")->vs, shaders->find("AnimatedUntextured")->ps, VertexLayoutCache::getAnimatedLayout());
	memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));
	for (int i = 0; i < gemanimation.bones.size(); i++)
	{
		Bone bone;
		bone.name = gemanimation.bones[i].name;
		memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
		bone.parentIndex = gemanimation.bones[i].parentIndex;
		animation.skeleton.bones.push_back(bone);
	}
	for (int i = 0; i < gemanimation.animations.size(); i++)
	{
		std::string name = gemanimation.animations[i].name;
		AnimationSequence aseq;
		aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
		for (int j = 0; j < gemanimation.animations[i].frames.size(); j++)
		{
			AnimationFrame frame;
			for (int index = 0; index < gemanimation.animations[i].frames[j].positions.size(); index++)
			{
				Vec3 p;
				Quaternion q;
				Vec3 s;
				memcpy(&p, &gemanimation.animations[i].frames[j].positions[index], sizeof(Vec3));
				frame.positions.push_back(p);
				memcpy(&q, &gemanimation.animations[i].frames[j].rotations[index], sizeof(Quaternion));
				frame.rotations.push_back(q);
				memcpy(&s, &gemanimation.animations[i].frames[j].scales[index], sizeof(Vec3));
				frame.scales.push_back(s);
			}
			aseq.frames.push_back(frame);
		}
		animation.animations.insert({ name, aseq });
	}
}

void AnimatedModel::updateWorld(Shaders* shaders, Matrix& w)
{
	shaders->updateConstantVS("AnimatedTextured", "staticMeshBuffer", "W", &w);
}

void AnimatedModel::draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w)
{
	bool useNormalMap = false;
	if (normalTextures.size() == meshes.size())
	{
		for (int i = 0; i < normalTextures.size(); i++)
		{
			if (normalTextures[i] != nullptr && normalTextures[i]->heapOffset >= 0)
			{
				useNormalMap = true;
			}
			else
			{
				useNormalMap = false;
				break;
			}
		}
	}
	
	std::string shaderName = useNormalMap ? "AnimatedNormalMapped" : "AnimatedTextured";
	std::string psoName = useNormalMap ? "AnimatedModelNormalMappedPSO" : "AnimatedModelTexturedPSO";
	
	shaders->updateConstantVS(shaderName, "staticMeshBuffer", "W", &w);
	shaders->updateConstantVS(shaderName, "staticMeshBuffer", "VP", &vp);
	shaders->updateConstantVS(shaderName, "staticMeshBuffer", "bones", instance->matrices);
	shaders->apply(core, shaderName);
	psos->bind(core, psoName);

	for (int i = 0; i < meshes.size(); i++)
	{
		if (i < textures.size() && textures[i] != nullptr)
		{
			shaders->updateTexturePS(core, shaderName, "tex", textures[i]->heapOffset);
		}
		
		if (useNormalMap && i < normalTextures.size() && normalTextures[i] != nullptr && normalTextures[i]->heapOffset >= 0)
		{
			shaders->updateTexturePS(core, shaderName, "normalMap", normalTextures[i]->heapOffset);
		}

		meshes[i]->draw(core);
	}
}

// ==================== Tree 实现 ====================

void Tree::generatePositions(int count, float minX, float maxX, float minZ, float maxZ, int seed)
{
	positions.clear();
	collisionBoxes.clear();
	
	static bool seeded = false;
	static int currentSeed = seed;
	if (!seeded || currentSeed != seed)
	{
		srand(seed);
		currentSeed = seed;
		seeded = true;
	}
	
	for (int i = 0; i < count; i++)
	{
		float rangeX = maxX - minX;
		float rangeZ = maxZ - minZ;
		float x = minX + (float)rand() / (float)RAND_MAX * rangeX;
		float z = minZ + (float)rand() / (float)RAND_MAX * rangeZ;
		float y = 0.0f;
		
		positions.push_back(Vec3(x, y, z));
	}
}

void Tree::init(Core* core, PSOManager* psos, Shaders* shaders, int treeCount)
{
	model.load(core, getModelPath(), shaders, psos, "PSTree.txt");
	loadTextures(core);
	
	scale = getScale();
	numInstances = treeCount;
	timer = 0.0f;
	
	generatePositions(treeCount, -70.0f, 70.0f, -70.0f, 70.0f);
	
	worldMatrices.clear();
	worldMatrices.reserve(numInstances);
	
	for (int i = 0; i < numInstances; i++)
	{
		Matrix W = Matrix::scaling(scale) * Matrix::translation(positions[i]);
		worldMatrices.push_back(W);
		
		Vec3 collisionSize = Vec3(2.0f, 5.0f, 2.0f);
		CollisionBox box(positions[i], collisionSize);
		collisionBoxes.push_back(box);
	}
}

void Tree::update(float dt)
{
	timer += dt;
	
	for (size_t i = 0; i < collisionBoxes.size() && i < positions.size(); i++)
	{
		collisionBoxes[i].setPosition(positions[i]);
	}
}

void Tree::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp)
{
	for (int i = 0; i < numInstances; i++)
	{
		model.updateWorld(shaders, worldMatrices[i]);
		model.draw(core, psos, shaders, vp);
	}
}

// ==================== Acacia 实现 ====================

void Acacia::loadTextures(Core* core)
{
	loadTexturesForModel(model, core, model.gemmeshes,
		[](int meshIndex, const std::string& defaultName) {
			if (meshIndex == 0)
			{
				return std::string("Models/Textures/bark09_ALB.png");
			}
			return defaultName;
		});
}

void Acacia::generatePositions(int count, float minX, float maxX, float minZ, float maxZ, int seed)
{
	Tree::generatePositions(count, minX, maxX, minZ, maxZ, 54321);
}

// ==================== Banana 实现 ====================

void Banana::loadTextures(Core* core)
{
	loadTexturesForModel(model, core, model.gemmeshes,
		[](int meshIndex, const std::string& defaultName) {
			return std::string("Models/Textures/banana1_LOD5_ALB.png");
		});
}

void Banana::generatePositions(int count, float minX, float maxX, float minZ, float maxZ, int seed)
{
	Tree::generatePositions(count, minX, maxX, minZ, maxZ, 98765);
}

// ==================== TRexEntity 实现 ====================

void TRexEntity::loadTextures(Core* core)
{
	loadTexturesForModel(model, core, model.gemmeshes, 
		[](int meshIndex, const std::string& defaultName) { return defaultName; });
}

void TRexEntity::init(Core* core, PSOManager* psos, Shaders* shaders)
{
	model.load(core, "Models/TRex.gem", psos, shaders);
	loadTextures(core);
	
	instance.init(&model.animation, 0);
	
	position = Vec3(5, 1, 20);
	scale = Vec3(0.01f, 0.01f, 0.01f);
	
	Vec3 collisionSize = Vec3(5.0f, 8.0f, 5.0f);
	collisionBox = CollisionBox(position, collisionSize);
	
	isDying = false;
	deathAnimationFinished = false;
	deathAngleY = 0.0f;
	
	moveSpeed = 3.0f;
	attackRange = 15.0f;
}

void TRexEntity::update(float dt, Window* window, const Vec3& cameraPos)
{
	collisionBox.setPosition(position);
	
	if (collisionBox.getHitTime() >= 1.5f && !deathAnimationFinished)
	{
		if (!isDying)
		{
			isDying = true;
			instance.resetAnimationTime();
		}
	}
	
	if (isDying && !deathAnimationFinished)
	{
		instance.update("death", dt);
		if (instance.animationFinished())
		{
			isDying = false;
			deathAnimationFinished = true;
		}
	}
	else if (!deathAnimationFinished)
	{
		Vec3 toCamera = cameraPos - position;
		toCamera.y = 0.0f;
		float distance = toCamera.length();
		
		if (distance > attackRange)
		{
			instance.update("run", dt);
			if (instance.animationFinished())
			{
				instance.resetAnimationTime();
			}
			
			if (distance > 0.001f)
			{
				Vec3 moveDir = toCamera.normalize();
				position = position + moveDir * moveSpeed * dt;
			}
		}
		else
		{
			instance.update("roar", dt);
			if (instance.animationFinished())
			{
				instance.resetAnimationTime();
			}
		}
	}
}

void TRexEntity::updateHitDetection(const Vec3& rayOrigin, const Vec3& rayDirection, bool isShooting, float dt)
{
	collisionBox.updateHitTime(rayOrigin, rayDirection, isShooting, dt);
}

void TRexEntity::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, const Vec3& cameraPos)
{
	Matrix rotation;
	float angleY = 0.0f;
	
	if (deathAnimationFinished)
	{
		angleY = deathAngleY;
	}
	else
	{
		Vec3 toCamera = cameraPos - position;
		toCamera.y = 0.0f;
		
		if (toCamera.lengthSq() > 0.0001f)
		{
			toCamera = toCamera.normalize();
			angleY = atan2f(toCamera.z, toCamera.x);
			angleY -= 3.14159f / 2.0f;
		}
		
		if (isDying)
		{
			deathAngleY = angleY;
		}
	}
	
	rotation = Matrix::rotateY(angleY);
	Matrix W = Matrix::scaling(scale) * rotation * Matrix::translation(position);
	model.draw(core, psos, shaders, &instance, vp, W);
}

// ==================== UziEntity 实现 ====================

void UziEntity::loadTextures(Core* core)
{
	loadTexturesForModel(model, core, model.gemmeshes,
		[](int meshIndex, const std::string& defaultName) {
			if (meshIndex == 0)
			{
				return std::string("Models/Textures/arms_2_Albedo_alb.png");
			}
			return defaultName;
		});
}

void UziEntity::init(Core* core, PSOManager* psos, Shaders* shaders)
{
	model.load(core, "Models/Uzi.gem", psos, shaders);
	loadTextures(core);
	
	instance.init(&model.animation, 0);
	
	rKeyPressedLastFrame = false;
	isReloading = false;
}

void UziEntity::update(float dt, Window* window)
{
	bool rKeyPressedThisFrame = window->keys['R'];
	bool rKeyJustPressed = rKeyPressedThisFrame && !rKeyPressedLastFrame;
	
	if (rKeyJustPressed)
	{
		isReloading = true;
		instance.resetAnimationTime();
	}
	
	rKeyPressedLastFrame = rKeyPressedThisFrame;
	
	if (window->mouseButtons[0])
	{
		instance.update("08 fire", dt);
		isReloading = false;
	}
	else if (isReloading)
	{
		instance.update("01 select", dt);
		if (instance.animationFinished())
		{
			isReloading = false;
		}
	}
	else if(window->keys['W']||window->keys['A']||window->keys['S']||window->keys['D'])
	{
		instance.update("07 run", dt);
	}
	else
	{
		instance.update("04 idle", dt);
	}
	
	if (instance.animationFinished() && !isReloading)
	{
		instance.resetAnimationTime();
	}
}

void UziEntity::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix& cameraWorld)
{
	Matrix uziOffset = Matrix::scaling(Vec3(0.03f, 0.03f, 0.05f)) * 
		Matrix::rotateY(3.14159f) * 
		Matrix::translation(Vec3(0.2f, 0.2f, -0.3f)); 
	Matrix W_uzi = uziOffset * cameraWorld;
	model.draw(core, psos, shaders, &instance, vp, W_uzi);
}

// ==================== SoldierEntity 实现 ====================

void SoldierEntity::loadTextures(Core* core)
{
	loadTexturesForModel(model, core, model.gemmeshes,
		[](int meshIndex, const std::string& defaultName) {
			return std::string("Models/Textures/MaleDuty_3_OBJ_Happy_Packed0_Diffuse.png");
		});
}

void SoldierEntity::init(Core* core, PSOManager* psos, Shaders* shaders)
{
	model.load(core, "Models/Soldier1.gem", psos, shaders);
	loadTextures(core);
	
	instance.init(&model.animation, 0);
	
	position = Vec3(1, 1, 0);
	scale = Vec3(0.01f, 0.01f, 0.01f);
	rKeyPressedLastFrame = false;
}

void SoldierEntity::update(float dt, Window* window)
{
	
	
	bool isRPressed = (window->keys['R'] );
	bool rKeyJustPressed = isRPressed && !rKeyPressedLastFrame;
	
	bool isReloading = (instance.usingAnimation == "reloading" && !instance.animationFinished());
	
	if (isReloading)
	{
		instance.update("reloading", dt * 3.0f);
		if (dt>2.f)
		{
			instance.resetAnimationTime();
		}
	}
	else if (rKeyJustPressed)
	{
		instance.update("reloading", dt * 3.0f);
	}
	else
	{
		bool isShooting = window->mouseButtons[0];
		bool isMoving = (window->keys['W'] || window->keys['A'] || window->keys['S'] || window->keys['D']);
		
		if (isShooting)
		{
			instance.update("firing rifle", dt);
			if (instance.animationFinished())
			{
				instance.resetAnimationTime();
			}
		}
		else if (isMoving)
		{
			instance.update("run forward", dt);
			if (instance.animationFinished())
			{
				instance.resetAnimationTime();
			}
		}
	}
	
	rKeyPressedLastFrame = isRPressed;
}

void SoldierEntity::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix& cameraWorld)
{
	Matrix soldierOffset = Matrix::scaling(Vec3(0.0005f, 0.0005f, 0.0005)) * 
		Matrix::translation(Vec3(-0.25f, -0.1, 0.3f));
	Matrix W_soldier = soldierOffset * cameraWorld;
	model.draw(core, psos, shaders, &instance, vp, W_soldier);
}

// ==================== Sphere 实现 ====================

void Sphere::init(Core* core, PSOManager *psos, Shaders* shaders)
{
	std::vector<STATIC_VERTEX> vertices;
	std::vector<unsigned int> indices;
	
	int segments = 32;
	int rings = 32;
	float radius = 100.0f;
	
	for (int i = 0; i <= rings; i++)
	{
		float theta = 3.14159f * i / rings;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);
		
		for (int j = 0; j <= segments; j++)
		{
			float phi = 2.0f * 3.14159f * j / segments;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);
			
			Vec3 position;
			position.x = radius * sinTheta * cosPhi;
			position.y = radius * cosTheta;
			position.z = radius * sinTheta * sinPhi;
			
			Vec3 normal = position;
			float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			if (length > 0.0001f)
			{
				normal.x /= length;
				normal.y /= length;
				normal.z /= length;
			}
			
			float u = (float)j / segments;
			float v = (float)i / rings;
			
			vertices.push_back(createStaticVertex(position, normal, u, v));
		}
	}
	
	for (int i = 0; i < rings; i++)
	{
		for (int j = 0; j < segments; j++)
		{
			int first = i * (segments + 1) + j;
			int second = first + segments + 1;
			
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);
			
			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}
	
	mesh.init(core, vertices, indices);
	
	texture = new Texture();
	texture->heapOffset = -1;
	texture->load(core, "Models/Textures/kloppenheim_06_puresky.png");
	
	shaders->load(core, "StaticModelTextured", "shaders/VS.txt", "shaders/PSTextured.txt");
	shaderName = "StaticModelTextured";
	psos->createPSO(core, "StaticModelTexturedPSO",
		shaders->find("StaticModelTextured")->vs,
		shaders->find("StaticModelTextured")->ps,
		VertexLayoutCache::getStaticLayout(),
		true);
}

void Sphere::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp)
{
	static float rotationAngle = 0.0f;
	rotationAngle += 0.0005f;
	if (rotationAngle > 2.0f * 3.14159f) rotationAngle -= 2.0f * 3.14159f;
	
	Matrix rotation = Matrix::rotateY(rotationAngle);
	Matrix world = Matrix::scaling(Vec3(1.0f, 1.0f, 1.0f)) * rotation * Matrix::translation(Vec3(0, 1, 0));
	
	shaders->updateConstantVS("StaticModelTextured", "staticMeshBuffer", "VP", &vp);
	shaders->updateConstantVS("StaticModelTextured", "staticMeshBuffer", "W", &world);
	
	shaders->apply(core, shaderName);
	psos->bind(core, "StaticModelTexturedPSO");
	
	if (texture != nullptr && texture->heapOffset >= 0)
	{
		shaders->updateTexturePS(core, "StaticModelTextured", "tex", texture->heapOffset);
	}
	
	mesh.draw(core);
}

// ==================== Cube 实现 ====================

void Cube::init(Core* core, PSOManager *psos, Shaders* shaders)
{
	std::vector<STATIC_VERTEX> vertices;
	std::vector<unsigned int> indices;
	
	float size = 0.5f;
	
	Vec3 positions[8] = {
		Vec3(-size, -size,  size),
		Vec3( size, -size,  size),
		Vec3(-size,  size,  size),
		Vec3( size,  size,  size),
		Vec3(-size, -size, -size),
		Vec3( size, -size, -size),
		Vec3(-size,  size, -size),
		Vec3( size,  size, -size)
	};
	
	Vec3 normals[6] = {
		Vec3(0, 0, 1),
		Vec3(0, 0, -1),
		Vec3(0, 1, 0),
		Vec3(0, -1, 0),
		Vec3(-1, 0, 0),
		Vec3(1, 0, 0)
	};
	
	vertices.push_back(createStaticVertex(positions[0], normals[0], 0, 1));
	vertices.push_back(createStaticVertex(positions[1], normals[0], 1, 1));
	vertices.push_back(createStaticVertex(positions[2], normals[0], 0, 0));
	vertices.push_back(createStaticVertex(positions[3], normals[0], 1, 0));
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(1); indices.push_back(3); indices.push_back(2);
	
	vertices.push_back(createStaticVertex(positions[5], normals[1], 0, 1));
	vertices.push_back(createStaticVertex(positions[4], normals[1], 1, 1));
	vertices.push_back(createStaticVertex(positions[7], normals[1], 0, 0));
	vertices.push_back(createStaticVertex(positions[6], normals[1], 1, 0));
	indices.push_back(4); indices.push_back(5); indices.push_back(6);
	indices.push_back(5); indices.push_back(7); indices.push_back(6);
	
	vertices.push_back(createStaticVertex(positions[2], normals[2], 0, 1));
	vertices.push_back(createStaticVertex(positions[3], normals[2], 1, 1));
	vertices.push_back(createStaticVertex(positions[6], normals[2], 0, 0));
	vertices.push_back(createStaticVertex(positions[7], normals[2], 1, 0));
	indices.push_back(8); indices.push_back(9); indices.push_back(10);
	indices.push_back(9); indices.push_back(11); indices.push_back(10);
	
	vertices.push_back(createStaticVertex(positions[4], normals[3], 0, 1));
	vertices.push_back(createStaticVertex(positions[5], normals[3], 1, 1));
	vertices.push_back(createStaticVertex(positions[0], normals[3], 0, 0));
	vertices.push_back(createStaticVertex(positions[1], normals[3], 1, 0));
	indices.push_back(12); indices.push_back(13); indices.push_back(14);
	indices.push_back(13); indices.push_back(15); indices.push_back(14);
	
	vertices.push_back(createStaticVertex(positions[4], normals[4], 0, 1));
	vertices.push_back(createStaticVertex(positions[0], normals[4], 1, 1));
	vertices.push_back(createStaticVertex(positions[6], normals[4], 0, 0));
	vertices.push_back(createStaticVertex(positions[2], normals[4], 1, 0));
	indices.push_back(16); indices.push_back(17); indices.push_back(18);
	indices.push_back(17); indices.push_back(19); indices.push_back(18);
	
	vertices.push_back(createStaticVertex(positions[1], normals[5], 0, 1));
	vertices.push_back(createStaticVertex(positions[5], normals[5], 1, 1));
	vertices.push_back(createStaticVertex(positions[3], normals[5], 0, 0));
	vertices.push_back(createStaticVertex(positions[7], normals[5], 1, 0));
	indices.push_back(20); indices.push_back(21); indices.push_back(22);
	indices.push_back(21); indices.push_back(23); indices.push_back(22);
	
	mesh.init(core, vertices, indices);
	
	texture = new Texture();
	texture->heapOffset = -1;
	texture->load(core, "Models/Textures/grass.png");
	
	shaders->load(core, "StaticModelGround", "shaders/VS.txt", "shaders/PSGround.txt");
	shaderName = "StaticModelGround";
	psos->createPSO(core, "StaticModelGroundPSO",
		shaders->find("StaticModelGround")->vs,
		shaders->find("StaticModelGround")->ps,
		VertexLayoutCache::getStaticLayout(),
		true);
}

void Cube::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp)
{
	Matrix world = Matrix::translation(Vec3(0, -1.f, 0))*Matrix::scaling(Vec3(200.f, 1.f,200.f));
	
	shaders->updateConstantVS("StaticModelGround", "staticMeshBuffer", "VP", &vp);
	shaders->updateConstantVS("StaticModelGround", "staticMeshBuffer", "W", &world);
	
	shaders->apply(core, shaderName);
	psos->bind(core, "StaticModelGroundPSO");
	
	if (texture != nullptr && texture->heapOffset >= 0)
	{
		shaders->updateTexturePS(core, "StaticModelGround", "tex", texture->heapOffset);
	}
	
	mesh.draw(core);
}

// ==================== Plane 实现 ====================

void Plane::init(Core* core, PSOManager *psos, Shaders* shaders)
{
	std::vector<STATIC_VERTEX> vertices;
	vertices.push_back(createStaticVertex(Vec3(-1, 0, -1), Vec3(0, 1, 0), 0, 0));
	vertices.push_back(createStaticVertex(Vec3(1, 0, -1), Vec3(0, 1, 0), 1, 0));
	vertices.push_back(createStaticVertex(Vec3(-1, 0, 1), Vec3(0, 1, 0), 0, 1));
	vertices.push_back(createStaticVertex(Vec3(1, 0, 1), Vec3(0, 1, 0), 1, 1));
	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);
	mesh.init(core, vertices, indices);
	shaders->load(core, "StaticModelUntextured", "shaders/VS.txt", "shaders/PSUntextured.txt");
	shaderName = "StaticModelUntextured";
	psos->createPSO(core, "StaticModelUntexturedPSO", shaders->find("StaticModelUntextured")->vs, shaders->find("StaticModelUntextured")->ps, VertexLayoutCache::getStaticLayout());
}

void Plane::draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix world)
{
	shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "VP", &vp);
	shaders->updateConstantVS("StaticModelUntextured", "staticMeshBuffer", "W", &world);
	shaders->apply(core, shaderName);
	psos->bind(core, "StaticModelUntexturedPSO");
	mesh.draw(core);
}

// ==================== Crosshair 实现 ====================

void Crosshair::init(Core* core, PSOManager* psos, Shaders* shaders)
{
	plane.init(core, psos, shaders);
}

void Crosshair::draw(Core* core, PSOManager* psos, Shaders* shaders)
{
	Matrix identityVP;
	Matrix rotateStandUp = Matrix::rotateX(1.5708f);

	Matrix wHorz = Matrix::scaling(Vec3(0.03f, 1.0f, 0.003f)) * 
		Matrix::translation(Vec3(0, 0, 0)) * rotateStandUp;
	plane.draw(core, psos, shaders, identityVP, wHorz);

	Matrix wVert = Matrix::scaling(Vec3(0.002f, 1.0f, 0.05f)) * 
		Matrix::translation(Vec3(0, 0, 0)) * rotateStandUp;
	plane.draw(core, psos, shaders, identityVP, wVert);
}

