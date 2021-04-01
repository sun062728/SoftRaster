#pragma once

#include <memory>
#include <map>
#include <string>

class Material;
typedef std::shared_ptr<Material>				MaterialPtr;
class Mesh;
typedef std::shared_ptr<Mesh>					MeshPtr;
struct MeshData;
class Entity;
typedef std::shared_ptr<Entity>					EntityPtr;
typedef std::map<std::string, EntityPtr>		EntityLibrary;
class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;
struct Light;
typedef std::shared_ptr<Light>					LightPtr;
class GlobalContext;
class Camera;
typedef std::shared_ptr<Camera>					CameraPtr;
typedef std::map<std::string, CameraPtr>		CameraLibrary;
class AppFramework;
class RenderEngine;
class SceneManager;
class Shader;
