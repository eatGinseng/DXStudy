#include <DirectXMath.h>
using namespace DirectX;

struct ParticleType
{
	float positionX, positionY, positionZ;
	float red, green, blue;
	float velocity;
	bool active;
};

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT3 texture;
	XMVECTOR color;
};

public:
	ParticleSystemClass();
	ParticleSystemClass(const ParticleSystemClass&);
	~ParticleSystemClass();

