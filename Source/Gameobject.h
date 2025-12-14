#pragma once

#include "Core.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"
#include "GEMLoader.h"
#include "Shaders.h"
#include "PSO.h"
#include "Window.h"
#include "Maths.h"
#include "Collision.h"
#include "Utilities.h"
#include <vector>
#include <string>
#include <functional>
#include <fstream>

// ==================== 模型类 ====================

class StaticModel
{
public:
	std::vector<Mesh*> meshes;
	std::vector<Texture*> textures;
	std::vector<Texture*> normalTextures;
	std::vector<GEMLoader::GEMMesh> gemmeshes;
	std::string shaderName;

	void load(Core* core, std::string filename, Shaders* shaders, PSOManager* psos, std::string pixelShaderFile = "PSTextured.txt");
	void updateWorld(Shaders* shaders, Matrix& w);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp);
};

class AnimatedModel
{
public:
	std::vector<Mesh *> meshes;
	Animation animation;
	std::vector<Texture*> textures;
	std::vector<Texture*> normalTextures;
	std::vector<GEMLoader::GEMMesh> gemmeshes;
	
	void load(Core* core, std::string filename, PSOManager* psos, Shaders* shaders);
	void updateWorld(Shaders* shaders, Matrix& w);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w);
};

// ==================== 实体类 ====================

class TRexEntity
{
private:
	void loadTextures(Core* core);
	
public:
	AnimatedModel model;
	AnimationInstance instance;
	Vec3 position;
	Vec3 scale;
	
	bool isDying;
	bool deathAnimationFinished;
	float deathAngleY;
	
	float moveSpeed;
	float attackRange;
	
	CollisionBox collisionBox;
	
	void init(Core* core, PSOManager* psos, Shaders* shaders);
	void update(float dt, Window* window, const Vec3& cameraPos);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, const Vec3& cameraPos);
	
	void updateHitDetection(const Vec3& rayOrigin, const Vec3& rayDirection, bool isShooting, float dt);
};

class UziEntity
{
private:
	void loadTextures(Core* core);
	
public:
	AnimatedModel model;
	AnimationInstance instance;
	
	bool rKeyPressedLastFrame;
	bool isReloading;
	
	void init(Core* core, PSOManager* psos, Shaders* shaders);
	void update(float dt, Window* window);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix& cameraWorld);
};

class SoldierEntity
{
private:
	void loadTextures(Core* core);
	bool rKeyPressedLastFrame;
	
public:
	AnimatedModel model;
	AnimationInstance instance;
	Vec3 position;
	Vec3 scale;
	
	void init(Core* core, PSOManager* psos, Shaders* shaders);
	void update(float dt, Window* window);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix& cameraWorld);
};

// ==================== 树基类 ====================

class Tree
{
protected:
	virtual void loadTextures(Core* core) = 0;
	virtual void generatePositions(int count, float minX, float maxX, float minZ, float maxZ, int seed = 54321);
	virtual std::string getModelPath() const = 0;
	virtual Vec3 getScale() const = 0;

public:
	StaticModel model;
	std::vector<Vec3> positions;
	std::vector<Matrix> worldMatrices;
	Vec3 scale;
	int numInstances;
	
	std::vector<CollisionBox> collisionBoxes;
	
	float timer;

	void init(Core* core, PSOManager* psos, Shaders* shaders, int treeCount = 50);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp);
	void update(float dt);
	
	std::vector<CollisionBox>& getCollisionBoxes() { return collisionBoxes; }
};

// ==================== 树子类 ====================

class Acacia : public Tree
{
protected:
	virtual void loadTextures(Core* core) override;
	virtual void generatePositions(int count, float minX, float maxX, float minZ, float maxZ, int seed = 54321) override;
	virtual std::string getModelPath() const override { return "Models/acacia.gem"; }
	virtual Vec3 getScale() const override { return Vec3(0.01f, 0.01f, 0.01f); }
};

class Banana : public Tree
{
protected:
	virtual void loadTextures(Core* core) override;
	virtual void generatePositions(int count, float minX, float maxX, float minZ, float maxZ, int seed = 98765) override;
	virtual std::string getModelPath() const override { return "Models/banana1_LOD4.gem"; }
	virtual Vec3 getScale() const override { return Vec3(0.02f, 0.02f, 0.02f); }
};

// ==================== 几何体类 ====================

class Sphere
{
public:
	Mesh mesh;
	Texture* texture;
	std::string shaderName;

	void init(Core* core, PSOManager *psos, Shaders* shaders);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp);
};

class Cube
{
public:
	Mesh mesh;
	Texture* texture;
	std::string shaderName;

	void init(Core* core, PSOManager *psos, Shaders* shaders);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp);
};

class Plane
{
public:
	Mesh mesh;
	std::string shaderName;

	void init(Core* core, PSOManager *psos, Shaders* shaders);
	void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, Matrix world = Matrix());
};

// ==================== 准星类 ====================

class Crosshair
{
public:
	Plane plane;

	void init(Core* core, PSOManager* psos, Shaders* shaders);
	void draw(Core* core, PSOManager* psos, Shaders* shaders);
};