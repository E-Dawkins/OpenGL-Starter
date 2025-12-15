#pragma once
#include <vector>
#include <array>

#include "core/camera.h"
#include "core/window.h"

class Mesh;
class Material;

struct RenderObject
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	glm::mat4 transform = glm::mat4(1);
};

struct FrameBufferObject
{
	GLuint frameBuffer, colorTexture, depthTexture;
};

class Renderer
{
public:
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Window> window;

	int peelFirstRender = 3, peelLastRender = 0;

private:
	typedef void (*UpdateCallback)(Renderer* _r, float _dt);
	UpdateCallback mUpdateCallback;

	std::vector<RenderObject> mOpaqueObjects = {};
	std::vector<RenderObject> mTranslucentObjects = {};

	FrameBufferObject mOpaqueFbo = {};
	std::array<FrameBufferObject, 4> mDepthPeelFbos = {};

public:
	Renderer();
	~Renderer();

	void RenderLoop();
	void BindToUpdateCallback(UpdateCallback _callback);

	void RegisterRenderObject(const RenderObject& _object);

private:
	FrameBufferObject GenerateFbo() const;

	void Pass_Opaque(const glm::mat4& _viewProjection);
	void Pass_DepthPeelLayers(const glm::mat4& _viewProjection);
	void Pass_Composite();
};