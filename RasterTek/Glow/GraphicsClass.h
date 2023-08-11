

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightclass.h"

#include "TextureClass.h"

#include "ProjectionShaderClass.h"
#include "RenderToTextureClass.h"
#include "TextureShaderClass.h"
#include "OrthowindowClass.h"
#include "viewPointClass.h"

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

	bool RenderGlowMapToTexture();

	bool DownSampleTexture();
	bool RenderVerticalBlurTexture();
	bool RenderHorizontalBlurTexture();
	bool UpsampleTexture();

	bool Render();

private:

	D3DClass* m_D3D;
	CameraClass* m_Camera;

	ModelClass* m_CubeModel, * m_GroundModel, * m_SphereModel;
	LightClass* m_Light;

	XMMATRIX baseViewMatrix;

	ProjectionShaderClass* m_ProjectionShader;
	TextureShaderClass* m_TextureShader;
	TextureClass* m_ProjectionTexture;
	ViewPointClass* m_ViewPoint;

	OrthoWindowClass* m_SmallWindow, *m_FullScreenWindow;

	RenderToTextureClass* m_GlowTexture, *m_DownSampleTexture, *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_UpsampleTexture;


};

