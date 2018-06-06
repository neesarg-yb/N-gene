#include "SpriteAnimSet.hpp"

SpriteAnimSet::SpriteAnimSet( SpriteAnimSetDefinition* animSetDef )
{
	// Set pointer to AnimSetDefinition
	m_animSetDef = animSetDef;

	
	// Create all animations and add it to map
	for( std::map< std::string, SpriteAnimDefinition*>::iterator it = animSetDef->m_namedAnimDefs.begin(); it != animSetDef->m_namedAnimDefs.end(); it++ )
	{
		SpriteAnim *newAnimToAdd = new SpriteAnim( it->second );
		m_namedAnims[ it->first ] = newAnimToAdd;
	}


	// Set pointer to default Idle animation
	std::map< std::string, SpriteAnim* >::iterator it = m_namedAnims.find( m_animSetDef->m_defaultAnimName );
	GUARANTEE_OR_DIE( it != m_namedAnims.end(), std::string("Default animation " + m_animSetDef->m_defaultAnimName + ", not found!") );

	m_currentAnim = it->second;
}

SpriteAnimSet::~SpriteAnimSet()
{

}

void SpriteAnimSet::Update( float deltaSeconds )
{
	m_currentAnim->Update( deltaSeconds );
}

void SpriteAnimSet::StartAnim( const std::string& animName )
{
	std::map< std::string, SpriteAnim* >::iterator it = m_namedAnims.find( animName );
	GUARANTEE_OR_DIE( it != m_namedAnims.end(), std::string("Default animation " + animName + ", not found!") );

	m_currentAnim = it->second;
	m_currentAnim->PlayFromStart();
}

void SpriteAnimSet::SetCurrentAnim( const std::string& animName )
{
	std::map< std::string, SpriteAnim* >::iterator it = m_namedAnims.find( animName );
	GUARANTEE_OR_DIE( it != m_namedAnims.end(), std::string("Default animation " + animName + ", not found!") );

	m_currentAnim = it->second;
}

bool SpriteAnimSet::HasAnimationByName( std::string animName )
{
	std::map< std::string, SpriteAnim* >::iterator it = m_namedAnims.find( animName );

	if( it == m_namedAnims.end() )
		return false;
	else
		return true;
}

const Texture& SpriteAnimSet::GetCurrentTexture() const
{
	return m_currentAnim->GetTexture();
}

AABB2 SpriteAnimSet::GetCurrentUVs() const
{
	return m_currentAnim->GetCurrentUVs();
}