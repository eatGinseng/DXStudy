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
#include "TextureShaderClass.h"
#include "ModelClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "ModelListClass.h"
#include "FrustumClass.h"


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
	bool Frame(int, int, float, int, int, float);
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;

	TextClass* m_Text;
	BitmapClass* m_Cursor;
	XMMATRIX baseViewMatrix;

};

#endif