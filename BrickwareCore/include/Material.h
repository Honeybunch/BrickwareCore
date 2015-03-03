#ifndef MATERIAL_H
#define MATERIAL_H

#include "BrickwareCoreDLL.h"

#include "Shader.h"

#ifdef GL_SUPPORT
#include <GL/glew.h>

#ifdef _WIN32
#define GLFW_DLL
#endif

#define GLFW_INCLUDE_GLU
#include <glfw3.h>
#endif

#include "Component.h"
#include "Utils.h"
#include "Texture.h"
#include "Camera.h"

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

#include <map>

class Material : public Component
{
public:
	BRICKWARE_CORE_API Material(Shader* shader);

	static BRICKWARE_CORE_API std::vector<Material*> Materials;

	BRICKWARE_CORE_API void bindShader();
	BRICKWARE_CORE_API void freeShader();

#ifdef D3D_SUPPORT
	std::vector<ID3D11Buffer*> getConstantBuffers();
	std::vector<char*> getConstantBufferData();
#endif

	BRICKWARE_CORE_API virtual Component* Clone() override;

	BRICKWARE_CORE_API void setVector4(const char* valueName, Vector4 value);
	BRICKWARE_CORE_API void setVector3(const char* valueName, Vector3 value);
	BRICKWARE_CORE_API void setVector2(const char* valueName, Vector2 value);

	BRICKWARE_CORE_API void setInt    (const char* valueName, int value);
	BRICKWARE_CORE_API void setFloat  (const char* valueName, float value);
	BRICKWARE_CORE_API void setDouble (const char* valueName, double value);

	BRICKWARE_CORE_API void setMatrix4(const char* valueName, Matrix4 value);
	BRICKWARE_CORE_API void setMatrix3(const char* valueName, Matrix3 value);

	BRICKWARE_CORE_API void setTexture(const char* textureName, Texture* texture);

	BRICKWARE_CORE_API ~Material();

private:
	Shader* shader;

	std::map<std::string, Texture*> textureMap;

	//Default texture to be loaded once and passed by default to every material
	static Texture* defaultTexture;

#ifdef GL_SUPPORT
	void setVector4GL(const char* valueName, Vector4 value);
	void setVector3GL(const char* valueName, Vector3 value);
	void setVector2GL(const char* valueName, Vector2 value);

	void setIntGL    (const char* valueName, int value);
	void setFloatGL  (const char* valueName, float value);
	void setDoubleGL (const char* valueName, double value);

	void setMatrix4GL(const char* valueName, Matrix4 value);
	void setMatrix3GL(const char* valueName, Matrix3 value);
#endif

#ifdef D3D_SUPPORT
	void setVector4D3D(const char* valueName, Vector4 value);
	void setVector3D3D(const char* valueName, Vector3 value);
	void setVector2D3D(const char* valueName, Vector2 value);

	void setIntD3D    (const char* valueName, int value);
	void setFloatD3D  (const char* valueName, float value);
	void setDoubleD3D (const char* valueName, double value);

	void setMatrix4D3D(const char* valueName, Matrix4 value);
	void setMatrix3D3D(const char* valueName, Matrix3 value);
#endif

};

#endif