#pragma once
#include "MapGenStep.hpp"
#include "Game/FillAndEdge.hpp"
#include "Game/Sprinkle.hpp"
#include "Game/FromFile.hpp"
#include "Game/Mutate.hpp"


MapGenStep::MapGenStep( const XMLElement& genStepXmlElement )
{
	m_name				= std::string( genStepXmlElement.Name() );

	IntRange iterRange	= IntRange( m_iterations );
	iterRange			= ParseXmlAttribute( genStepXmlElement, "iterations",  iterRange );
	m_iterations		= GetRandomIntInRange( iterRange.min, iterRange.max );
	m_chanceToRun		= ParseXmlAttribute( genStepXmlElement, "chanceToRun", m_chanceToRun );
}

MapGenStep::~MapGenStep()
{

}

MapGenStep* MapGenStep::CreateMapGenStep( const XMLElement& genStepXmlElement )
{
	std::string nameOfGenStep = genStepXmlElement.Name();

	if( nameOfGenStep == "FillAndEdge" )	return new FillAndEdge	( genStepXmlElement );
	if( nameOfGenStep == "Sprinkle" )		return new Sprinkle		( genStepXmlElement );
	if( nameOfGenStep == "FromFile" )		return new FromFile		( genStepXmlElement );
	if( nameOfGenStep == "Mutate" )			return new Mutate		( genStepXmlElement );

	return nullptr;
}