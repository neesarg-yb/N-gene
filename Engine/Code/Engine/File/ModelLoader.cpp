#pragma once
#include "Engine/Math/MathUtil.hpp"
#include "Engine/File/File.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

struct VUNpair {
	Vector3 vertex;
	Vector2 UV;
	Vector3 normal;
};

bool ModelLoader::LoadObjectModelFromPath( std::string path, Renderable &newRenderable )
{

	// To store all the data..
	std::vector< Mesh* >		allMeshes;
	std::vector< Material* >	allMaterials;
	MeshBuilder					theMeshBuilder;
	theMeshBuilder.Begin( PRIMITIVE_TRIANGES, false );

	// See if you can successfully load the file
	const char *buffer = (const char *) FileReadToNewBuffer( path.c_str() );
	// Failed to load the file
	if( buffer == nullptr )
		return false;

	std::vector< Vector3 >		allVertices;
	std::vector< Vector3 >		allNormals;
	std::vector< Vector2 >		allUVs;

	std::vector< std::string >	allNewLines = SplitIntoStringsByDelimiter( buffer, '\n' );
	// Start reading it line-by-line..
	for each ( std::string thisLine in allNewLines )
	{
		std::vector< std::string > elementInThisLine = SplitIntoStringsByDelimiter( thisLine, ' ' );

		if( elementInThisLine.size() <= 0 )
			continue;

		std::string &firstElement = elementInThisLine[0];
		// Add Vertex: starts with "v"
		if ( firstElement == "v" )
		{
			Vector3 newVertex;
			newVertex.x = -1.f * (float) atof( elementInThisLine[1].c_str() );		// Flip x value, b/c Obj files' positive x-axis is on left (ours is on right)
			newVertex.y =		 (float) atof( elementInThisLine[2].c_str() );
			newVertex.z =		 (float) atof( elementInThisLine[3].c_str() );

			allVertices.push_back ( newVertex );
			continue;
		}
		// Add Normal: starts with "vn"
		if( firstElement == "vn" )
		{
			Vector3 newNormal;
			newNormal.x = -1.f * (float) atof( elementInThisLine[1].c_str() );		// Flip x value, b/c Obj files' positive x-axis is on left (ours is on right)
			newNormal.y =		 (float) atof( elementInThisLine[2].c_str() );
			newNormal.z =		 (float) atof( elementInThisLine[3].c_str() );

			allNormals.push_back ( newNormal );
			continue;
		}
		// Add UVs	 : starts with "vt"
		if( firstElement == "vt" )
		{
			Vector2 newUV;
			newUV.x = (float) atof( elementInThisLine[1].c_str() );
			newUV.y = (float) atof( elementInThisLine[2].c_str() );

			allUVs.push_back ( newUV );
			continue;
		}
		// Look for usemtl
		if( firstElement == "usemtl" )
		{
			// Flush out all data from MeshBuilder to allMeshes
			theMeshBuilder.End();
			if( theMeshBuilder.m_vertices.size() != 0 )
				allMeshes.push_back( theMeshBuilder.ConstructMesh<Vertex_Lit>() );

			// Reset the MeshBuilder
			theMeshBuilder = MeshBuilder();
			theMeshBuilder.Begin( PRIMITIVE_TRIANGES, false );

			// Construct and push new material
			std::string materialName	= elementInThisLine[1];
			std::string materialPath	= "Data//Materials//" + materialName + ".material";
			Material* newMaterial		= Material::CreateNewFromFile( materialPath );
			allMaterials.push_back( newMaterial );
		}
		// Add Faces : starts with "f"
		if( firstElement == "f" )
		{
			bool normalsAreProvided = true;
			std::vector< VUNpair > newFace;

			for (unsigned int i = 1; i < elementInThisLine.size(); i++)
			{
				std::string pairString = elementInThisLine[i];
				std::vector< std::string > vunStrings = SplitIntoStringsByDelimiter( pairString, '/' );

				// Check if normals are in face data or not
				if( vunStrings.size() == 3 )
					normalsAreProvided = true;
				else
					normalsAreProvided = false;

				VUNpair thisPair;
				thisPair.vertex = allVertices	[ atoi( vunStrings[0].c_str() ) - 1 ];
				thisPair.UV		= allUVs		[ atoi( vunStrings[1].c_str() ) - 1 ];
				// Add normal, if provided
				if( normalsAreProvided )
					thisPair.normal = allNormals	[ atoi( vunStrings[2].c_str() ) - 1 ];

				newFace.push_back( thisPair );
			}

			// If normals were not provided, construct em using cross-product
			if( normalsAreProvided == false )
			{
				Vector3 faceSide1 = newFace[2].vertex - newFace[1].vertex;
				Vector3 faceSide2 = newFace[1].vertex - newFace[0].vertex;

				Vector3 normalVec = Vector3::CrossProduct( faceSide1, faceSide2 );
				normalVec		  = normalVec.GetNormalized();

				// Assign calculated Normal
				for( unsigned int faceIdx = 0; faceIdx < newFace.size(); faceIdx++ )
					newFace[ faceIdx ].normal = normalVec;
			}

			// Add this face to meshBuilder
			if( newFace.size() == 3 )
				AddTriangleFaceToMeshBuilder( newFace, theMeshBuilder );
			else
				AddQuadFaceToMeshBuilder( newFace, theMeshBuilder );

			continue;
		}
	}

	// Add last mesh from theMeshBuilder
	theMeshBuilder.End();
	// It should not be empty
	if( theMeshBuilder.m_vertices.size() != 0 )
		allMeshes.push_back( theMeshBuilder.ConstructMesh<Vertex_Lit>() );
	else
		return false;

	// Copy all the data to Renderable
	for each( Mesh *mesh in allMeshes )
		newRenderable.AddSubMesh( mesh );

	for each( Material *material in allMaterials )
		newRenderable.AddSubMaterial( material );

	return true;
}

void ModelLoader::AddTriangleFaceToMeshBuilder( std::vector< VUNpair > &triangleFace, MeshBuilder &mb )
{
	for (unsigned int i = 0; i < 3; i++)
	{
		Vector3 tangent;
		triangleFace[i].normal.GetTangentAndBitangent( &tangent, nullptr );

		mb.SetColor( RGBA_WHITE_COLOR );
		mb.SetUV( triangleFace[i].UV );
		mb.SetNormal( triangleFace[i].normal );
		mb.SetTangent4( tangent.x, tangent.y, tangent.z, 1.f );
		mb.PushVertex( triangleFace[i].vertex );
	}
}

void ModelLoader::AddQuadFaceToMeshBuilder( std::vector< VUNpair > &quadFace, MeshBuilder &mb )
{
	// Push quad as two triangles..

	// Triangle I - 0, 1, 2
	for (unsigned int i = 0; i <= 2; i++)
	{
		Vector3 tangent;
		quadFace[i].normal.GetTangentAndBitangent( &tangent, nullptr );

		mb.SetColor( RGBA_WHITE_COLOR );
		mb.SetUV( quadFace[i].UV );
		mb.SetNormal( quadFace[i].normal );
		mb.SetTangent4( tangent.x, tangent.y, tangent.z, 1.f );
		mb.PushVertex( quadFace[i].vertex );
	}

	// Triangle II - 2, 3, 0
	for (unsigned int i = 2; i <= 3; i++)
	{
		Vector3 tangent;
		quadFace[i].normal.GetTangentAndBitangent( &tangent, nullptr );

		mb.SetColor( RGBA_WHITE_COLOR );
		mb.SetUV( quadFace[i].UV );
		mb.SetNormal( quadFace[i].normal );
		mb.SetTangent4( tangent.x, tangent.y, tangent.z, 1.f );
		mb.PushVertex( quadFace[i].vertex );
	}
		Vector3 tangent;
		quadFace[0].normal.GetTangentAndBitangent( &tangent, nullptr );

		mb.SetColor( RGBA_WHITE_COLOR );
		mb.SetUV( quadFace[0].UV );
		mb.SetNormal( quadFace[0].normal );
		mb.SetTangent4( tangent.x, tangent.y, tangent.z, 1.f );
		mb.PushVertex( quadFace[0].vertex );
}
