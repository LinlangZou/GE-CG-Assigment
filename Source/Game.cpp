#include "Core.h"
#include "Window.h"
#include "Timer.h"
#include "Maths.h"
#include "Shaders.h"
#include "Mesh.h"
#include "PSO.h"
#include "Texture.h"
#include "GEMLoader.h"
#include "Animation.h"
#include "Gameobject.h"
#include "Collision.h"
#include "Particles.h"
#include "Camera.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <algorithm>
#include <string>
#include <functional>
#include <fstream>
#include <cmath>

#define WIDTH 1920
#define HEIGHT 1080



int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	// 系统初始化 
	// System Initialization
	Window window;
	window.create(WIDTH, HEIGHT, "My Window");
	Core core;
	core.init(window.hwnd, WIDTH, HEIGHT);
	Shaders shaders;
	PSOManager psos;

	// 游戏对象初始化 
	// Game Object Initialization
	Crosshair crosshair;
	crosshair.init(&core, &psos, &shaders);
	Sphere sphere;
	sphere.init(&core, &psos, &shaders);
	Cube cube;
	cube.init(&core, &psos, &shaders);
	Acacia tree;
	tree.init(&core, &psos, &shaders);
	Banana bananaTree;
	bananaTree.init(&core, &psos, &shaders);
	TRexEntity trex;
	trex.init(&core, &psos, &shaders);
	SoldierEntity soldier;
	soldier.init(&core, &psos, &shaders);
	UziEntity uzi;
	uzi.init(&core, &psos, &shaders);
	Particles leaves;
	leaves.init(&core, &psos, &shaders, 1000, Vec3(500.0f, 50.0f, 500.0f), 1.2f);
	ShowCursor(FALSE);


	Timer timer;
	float t = 0;
	Camera camera;
	camera.setCollisionBox(&trex.collisionBox);
	while (1)
	{
		core.beginFrame();
		ID3D12DescriptorHeap* heaps[] = { core.srvHeap.heap };
		core.getCommandList()->SetDescriptorHeaps(1, heaps);
		core.beginRenderPass();

		// 输入处理和时间更新 
		// Input Processing and Time Update
		float dt = timer.dt();
		window.checkInput();
		if (window.keys[VK_ESCAPE] == 1)
		{
			break;
		}
		t += dt;

		// 摄像机更新和矩阵计算 
		// Camera Update and Matrix Calculation
		camera.update(dt, &window);
		Matrix v = camera.getViewMatrix();
		Matrix p = Matrix::perspective(0.01f, 10000.0f, (float)WIDTH / (float)HEIGHT, 60.0f);
		Matrix vp = v * p;

		// 游戏对象更新 
		// Game Object Update
		leaves.update(dt);
		
		// 静态对象渲染 
		// Static Object Rendering
		sphere.draw(&core, &psos, &shaders, vp);
		cube.draw(&core, &psos, &shaders, vp);
		
		// 实体对象更新 
		// Entity Object Update
		trex.update(dt, &window, camera.cameraPos);
		soldier.update(dt, &window);
		uzi.update(dt, &window);


		// 实体对象渲染 
		// Entity Object Rendering
		Matrix cameraWorld = v.invert();
		shaders.updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "VP", &vp);
		trex.draw(&core, &psos, &shaders, vp, camera.cameraPos);
		uzi.draw(&core, &psos, &shaders, vp, cameraWorld);
		
		// 环境对象渲染 
		// Environment Object Rendering
		tree.draw(&core, &psos, &shaders, vp);
		bananaTree.draw(&core, &psos, &shaders, vp);
		
		// 碰撞检测和命中检测 
		// Collision Detection and Hit Detection
		Vec3 rayOrigin = camera.cameraPos;
		Vec3 rayDirection = camera.cameraFront;
		bool isShooting = window.mouseButtons[0];
		trex.updateHitDetection(rayOrigin, rayDirection, isShooting, dt);
	
		
		// 特效渲染 
		// Effect Rendering
		leaves.draw(&core, &psos, &shaders, vp, v);
		
		// UI 渲染 
		// UI Rendering
		soldier.draw(&core, &psos, &shaders, vp, cameraWorld);
		crosshair.draw(&core, &psos, &shaders);
	
		
		core.finishFrame();
	}
	
	// 清理和退出 
	// Cleanup and Exit
	core.flushGraphicsQueue();
}