#pragma once

class	Renderer;
class	Renderable;
class	MeshBuilder;
struct	VUNpair;

class ModelLoader
{
public:
	static bool LoadObjectModelFromPath( std::string path, Renderable &newRenderable );

private:
	static void AddTriangleFaceToMeshBuilder( std::vector< VUNpair > &triangleFace, MeshBuilder &mb );
	static void AddQuadFaceToMeshBuilder( std::vector< VUNpair > &quadFace, MeshBuilder &mb );
};