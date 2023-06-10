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
#include "lightshaderclass.h"
#include "PointLightShaderClass.h"
#include "TextureShaderClass.h"
#include "ModelClass.h"
#include "LightClass.h"
#include "PointLightClass.h"
#include "refractionshaderclass.h"
#include "watershaderclass.h"
#include "GlassShaderClass.h"


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
	bool RenderRefractionTexture();
	bool RenderScene(float);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	TextClass* m_Text;
	BitmapClass* m_Cursor;
	ModelClass* m_GroundModel, *m_CubeModel;

	LightClass* m_Light;

	PointLightShaderClass* m_PointLightShader;
	PointLightClass *m_PointLight1, *m_PointLight2, *m_PointLight3, *m_PointLight4;

	RenderToTextureClass *m_RefractionTexture, *m_ReflectionTexture;

	XMMATRIX baseViewMatrix;

	LightShaderClass* m_LightShader;
	RefractionShaderClass* m_RefractionShader;
	WaterShaderClass* m_WaterShader;

	BitmapClass* m_Bitmap;

	float m_waterHeight, m_waterTranslation;

	TextureShaderClass* m_RefractionTextureShader;
	GlassShaderClass* m_GlassShader;

};

#endif