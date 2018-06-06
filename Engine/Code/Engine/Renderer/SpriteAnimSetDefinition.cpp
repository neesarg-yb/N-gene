#include "SpriteAnimSetDefinition.hpp"

SpriteAnimSetDefinition::SpriteAnimSetDefinition( const XMLElement& animSetElement, Renderer& renderer )
{
	// Check for <SpriteAnimSet ..> element
	std::string rootName = animSetElement.Name();
	GUARANTEE_RECOVERABLE( rootName == "SpriteAnimSet", "XML Element named wrong( " + rootName + " ). It should be named SpriteAnimSet..!" );

	std::string		spriteSheetName = ParseXmlAttribute( animSetElement, "spriteSheet", std::string("No spriteSheetName set") );
	IntVector2		spriteLayout	= ParseXmlAttribute( animSetElement, "spriteLayout", spriteLayout ); 
	float			defaultFps		= ParseXmlAttribute( animSetElement, "fps", 1.f );

	std::string pathToTexture = std::string("Data\\Images\\" + spriteSheetName);
	Texture* newTexture = renderer.CreateOrGetTexture( pathToTexture );
	const SpriteSheet* newSpriteSheet = new SpriteSheet( *newTexture, spriteLayout.x, spriteLayout.y );

	// For each <SpiteAnim ../ > elements
	for( XMLElement* thisSpriteAnimDef = (XMLElement*) animSetElement.FirstChildElement(); thisSpriteAnimDef != nullptr; thisSpriteAnimDef = thisSpriteAnimDef->NextSiblingElement() )
	{
		// Check if current element is TileDefinition or not
		GUARANTEE_RECOVERABLE( std::string( thisSpriteAnimDef->Name() ) == "SpriteAnim", "XML Element named wrong( " + std::string( thisSpriteAnimDef->Name() ) + " ). It should be named SpriteAnim..!" );

		// Fetch data of the animation
		std::string	thisAnimDefName = ParseXmlAttribute( *thisSpriteAnimDef, "name", std::string("AnimDefName is not set") );

		// Construct SpriteAnim
		SpriteAnimDefinition* thisAnimDef = new SpriteAnimDefinition( *thisSpriteAnimDef, *newSpriteSheet, defaultFps/*, renderer*/ );

		// Add it to map
		m_namedAnimDefs[ thisAnimDefName ] = thisAnimDef;
	}
}

SpriteAnimSetDefinition::~SpriteAnimSetDefinition()
{

}