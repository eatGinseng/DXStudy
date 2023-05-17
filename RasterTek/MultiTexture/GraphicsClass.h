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
#include "TextClass.h"
#include "BitmapClass.h"
#include "rendertotextureclass.h"
#include "debugwindowclass.h"
#include "TextureShaderClass.h"
#include "ModelClass.h"
#include "MultiTextureShaderClass.h"
#include "LightClass.h"
#include "FadeShaderClass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = false;
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
	bool Render();

private:
	bool RenderToTexture(float rotation);
	bool RenderScene();
	bool RenderFadingScene();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	TextClass* m_Text;
	BitmapClass* m_Cursor;
	ModelClass* m_Model;
	ModelClass* m_Plane;

	LightClass* m_Light;

	RenderToTextureClass* m_RenderTexture;

	XMMATRIX baseViewMatrix;
	MultiTextureShaderClass* m_MultiTextureShader;

	float m_fadeInTime, m_accumulatedTime, m_fadePercentage;
	bool m_fadeDone;

	BitmapClass* m_Bitmap;
	FadeShaderClass* m_FadeShader;

};

#endif