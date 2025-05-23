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
#include "modelclass.h"
#include "lightclass.h"
#include "rendertotextureclass.h"
#include "depthshaderclass.h"
#include "shadowshaderclass.h"

#include "orthowindowclass.h"
#include "textureshaderclass.h"
#include "horizontalblurshaderclass.h"
#include "verticalblurshaderclass.h"
#include "softshadowshaderclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;

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

	bool RenderSceneToTexture();
	bool RenderBlackAndWhiteShadows();
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();

	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	ModelClass* m_CubeModel, * m_GroundModel, * m_SphereModel;
	LightClass* m_Light;

	XMMATRIX baseViewMatrix;

	RenderToTextureClass* m_RenderTexture, * m_BlackWhiteRenderTexture, * m_DownSampleTexure;
	RenderToTextureClass* m_HorizontalBlurTexture, * m_VerticalBlurTexture, * m_UpSampleTexure;
	DepthShaderClass* m_DepthShader;
	ShadowShaderClass* m_ShadowShader;
	OrthoWindowClass* m_SmallWindow, * m_FullScreenWindow;

	HorizontalBlurShaderClass* m_HorizontalBlurShader;
	VerticalBlurShaderClass* m_VerticalBlurShader;
	SoftShadowShaderClass* m_SoftShadowShader;


};

#endif