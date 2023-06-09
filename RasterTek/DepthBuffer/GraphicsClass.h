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
#include "DepthShaderClass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;


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
	bool RenderScene();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	TextClass* m_Text;
	BitmapClass* m_Cursor;
	ModelClass* m_Model;
	ModelClass* m_Plane;

	LightClass* m_Light;

	XMMATRIX baseViewMatrix;
	DepthShaderClass* m_DepthShader;

	BitmapClass* m_Bitmap;


};

#endif