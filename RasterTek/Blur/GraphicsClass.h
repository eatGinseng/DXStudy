////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include <DirectXMath.h>
#include "cameraclass.h"
#include "BitmapClass.h"
#include "rendertotextureclass.h"

#include "TextureShaderClass.h"
#include "ModelClass.h"

#include "HorizontalBlurShaderClass.h"
#include "VerticalBlurShaderClass.h"
#include "OrthowindowClass.h"

using namespace DirectX;

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


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
	bool Frame(int, int, float, int, int);


private:

	bool Render(float);
	bool RenderSceneToTexture(float);
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	BitmapClass* m_Cursor;
	ModelClass* m_Model;

	XMMATRIX baseViewMatrix;
	HorizontalBlurShaderClass* m_HorizontalBlurShader;
	VerticalBlurShaderClass* m_VerticalBlurShader;

	RenderToTextureClass* m_RenderTexture, *m_DownSampleTexture, *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_UpSampleTexture;
	OrthoWindowClass* m_SmallWindow, *m_FullScreenWindow;

	BitmapClass* m_Bitmap;

};

#endif