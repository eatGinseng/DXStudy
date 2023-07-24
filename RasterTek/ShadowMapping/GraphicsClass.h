////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"

#include "TextureShaderClass.h"
#include "ModelClass.h"

#include "ColorShaderClass.h"

#include "RenderToTextureClass.h"
#include "DepthShaderClass.h"
#include "LightClass.h"
#include "ShadowShaderClass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 30.0f;
const float SCREEN_NEAR = 0.1f;

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool Render();
	bool RenderSceneToTexture();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	ModelClass* m_CubeModel, * m_GroundModel, * m_SphereModel;
	LightClass* m_Light;

	XMMATRIX baseViewMatrix;

	ColorShaderClass* m_ColorShader;

	RenderToTextureClass* m_RenderTexture;

	DepthShaderClass* m_DepthShader;
	ShadowShaderClass* m_ShadowShader;

};

#endif