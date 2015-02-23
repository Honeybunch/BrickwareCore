#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "BrickwareCoreDLL.h"

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>

#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

#include "PrimitiveManager.h"

//Used to avoid warnings about exporting std::vectors
template class BRICKWARE_CORE_API std::vector < Vector3 >;

enum BRICKWARE_CORE_API PrimitiveType
{
	P_POINT,
	P_LINE,
	P_FILL
};

class BRICKWARE_CORE_API Primitive
{
	friend class PrimitiveManager;

public:
	static void SetColor(Vector4 color);
	static void SetPointSize(float pointSize); //Really only going to work with points
	static void SetLineWidth(float lineWidth);

	static void DrawPoint(Vector3 point);
	static void DrawLine(Vector3 start, Vector3 end);
	static void DrawQuad(Vector3 topLeft, Vector3 topRight, Vector3 bottomRight, Vector3 bottomLeft, Vector3 rotation);
	static void DrawCircle(Vector3 center, float radius, Vector3 rotation);

	static void FillQuad(Vector3 topLeft, Vector3 topRight, Vector3 bottomRight, Vector3 bottomLeft, Vector3 rotation);
	static void FillCircle(Vector3 center, float radius, Vector3 rotation);

private:
#ifdef GL_SUPPORT
	Primitive(GLuint vbo, GLuint ibo, Vector3 translation, Vector3 scale, Quaternion rotation, PrimitiveType drawType);
#endif
	
#ifdef D3D_SUPPORT
	Primitive(ID3D11Buffer* vbo, ID3D11Buffer* ibo, Vector3 translation, Vector3 scale, Vector3 rotation, PrimitiveType drawType);
#endif
	~Primitive();

#ifdef GL_SUPPORT
	GLuint vbo;
	GLuint ibo;
#endif
#ifdef D3D_SUPPORT
	ID3D11Buffer* positionBuffer;
	ID3D11Buffer* indexBuffer;
#endif

	static Vector4 currentColor;
	static float currentPointSize;
	static float currentLineWidth;

	unsigned int pointCount; //Number of points in this primitive
	Vector4 color;
	float pointSize;
	float lineWidth;
	PrimitiveType drawType;
	
	Matrix4 worldMatrix;
};

#endif