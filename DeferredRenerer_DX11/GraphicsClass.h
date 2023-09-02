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
#include "DeferredBuffersClass.h"
#include "OrthowindowClass.h"
#include "DeferredShaderClass.h"
#include "LightShaderClass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f;

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
	bool RenderShadowDepth();
	bool Render();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;

	ModelClass* m_CubeModel, * m_GroundModel, * m_SphereModel;
	LightClass* m_Light;

	XMMATRIX baseViewMatrix;

	OrthoWindowClass* m_FullScreenWindow;
	DeferredBuffersClass* m_DeferredBuffers;

	RenderToTextureClass* m_ShadowDepthTexture;
	DeferredShaderClass* m_DeferredShader;
	LightShaderClass* m_LightShader;
	DepthShaderClass* m_DepthShader;

};
