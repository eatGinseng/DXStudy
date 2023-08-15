

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"

#include "TextureClass.h"
#include "TextureShaderClass.h"

#include "LightShaderClass.h"

#include "InputClass.h"

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
	bool Frame();

	bool Render();

private:

	D3DClass* m_D3D;
	CameraClass* m_Camera;

	ModelClass* m_CubeModel, * m_GroundModel, * m_SphereModel;
	LightClass* m_Light;

	XMMATRIX baseViewMatrix;

	TextureShaderClass* m_TextureShader;

	LightShaderClass* m_LightShader;


};

