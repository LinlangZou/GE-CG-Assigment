# GE-CG-Assigment



https://github.com/user-attachments/assets/f1e977ce-34dc-49a1-819d-755b356a8d03
# Rasterizer

> A custom DirectX 12 3D game engine featuring skeletal animation, data-driven asset loading, and interactive gameplay mechanics.

## 📖 Description

Developed as a coursework project for the Computer Graphics module, **Rasterizer** is a 3D game engine built from scratch using DirectX 12. It demonstrates a complete, highly optimized rendering pipeline, moving from custom binary asset parsing to advanced vertex and pixel shader techniques. The engine powers a small, interactive playable level featuring a forest environment with trees, an animated dinosaur with AI state transitions, a soldier character, and a first-person weapon. 

## ✨ Key Features

**Rendering & Graphics Pipeline:**
* **DirectX 12 Foundation:** Modern graphics pipeline utilizing Pipeline State Objects (PSOs), depth buffering, and an alpha-test pipeline.
* **Lighting & Texturing:** Implements directional lighting and texture mapping, enhanced with normal mapping for detailed surface lighting.
* **Skeletal Animation:** Skinned mesh rendering driven by vertex-shader bone blending.
* **Instancing:** Highly optimized instanced tree rendering to populate the level environment efficiently.
* **Particle Systems:** Alpha-tested particle billboards to simulate environmental effects like falling leaves.

**Engine Architecture:**
* **Custom Asset Loader:** Data-driven loading pipeline utilizing a custom binary asset format (`.GEM`) for 3D models.
* **Mesh Support:** Full rendering support for both static and complex animated meshes.

**Gameplay & Physics:**
* **First-Person Camera:** Interactive camera with robust Axis-Aligned Bounding Box (AABB) collision detection against the environment.
* **Combat Mechanics:** Ray-hit intersection detection for first-person weapon shooting.
* **Entity State Machine:** Simple AI entity states driving enemy behavior (e.g., a dinosaur capable of chase, attack, and death states).

## 🛠 Tech Stack

* **Language:** C++
* **Shading Language:** HLSL (High-Level Shader Language)
* **Graphics API:** DirectX 12 (D3D1

