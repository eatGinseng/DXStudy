#pragma once
///////////////////////////////////////////////////////////////////////////////
// Filename: modellistclass.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _MODELLISTCLASS_H_
#define _MODELLISTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>
#include <stdlib.h>
#include <time.h>
using namespace DirectX;

class ModelListClass
{
private:
	struct ModelInfoType
	{
		XMVECTOR color;
		float positionX, positionY, positionZ;
	};

public:
	ModelListClass();
	ModelListClass(const ModelListClass&);
	~ModelListClass();

	bool Initialize(int);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, XMVECTOR&);

private:
	int m_modelCount;
	ModelInfoType* m_ModelInfoList;


};

#endif
