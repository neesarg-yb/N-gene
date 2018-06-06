#pragma once
#include "Command.hpp"
#include "Engine/Math/MathUtil.hpp"

// A global-private registry, only to get used by functions of this cpp file 
std::map< std::string, command_cb >	g_registeredCommands;

Command::Command( char const *str )
	: m_commandString( str )
	, m_nextCmdArgumentIndex( 0 )
{
	// Set the m_commandName
	size_t spaceAt = m_commandString.find(' ');
	m_commandName = std::string( m_commandString, 0, spaceAt );

	// Set the m_arguments - without considering \"-acts-as-a-whole-argument-string-\" thing
	m_arguments = SplitIntoStringsByDelimiter( m_commandString, ' ' );
	m_arguments.erase( m_arguments.begin() + 0 );

	// Now fix the \"double quoted with having spaces\" strings
	for( size_t argCount = 0; argCount < m_arguments.size(); argCount++ )
	{
		// If starts with \"
			// create a new string without \"
				// Loop through next arguments..
				// keep appending ( " " + nextArg ) to the new string
				// erase that arguments from m_arguments
				// End this operation if you find \" as end-substring
		std::string firstChar = std::string( m_arguments[ argCount ], 0, 1 );
		if( firstChar == "\"" )
		{
			std::string newArgument = std::string( m_arguments[ argCount ], 1 );		// Removing \" from that argument
			std::string strToAppend = "";
			
			for( size_t nextNestedArg = argCount+1; nextNestedArg < m_arguments.size(); nextNestedArg++ )
			{
				size_t strLength = m_arguments[ nextNestedArg ].length();
				std::string lastChar = std::string( m_arguments[ nextNestedArg ], strLength-1, 1 );

				strToAppend += " ";

				if( lastChar == "\"" )
				{
					strToAppend += std::string( m_arguments[ nextNestedArg ], 0, strLength-1 );
					m_arguments[ argCount ] = newArgument + strToAppend;
					m_arguments.erase( m_arguments.begin() + nextNestedArg );
					
					break;
				}
				else
				{
					strToAppend += m_arguments[ nextNestedArg ];
					m_arguments.erase( m_arguments.begin() + nextNestedArg );
					nextNestedArg--;
				}
			}
		}
	}
}

Command::~Command()
{

}

std::string Command::GetName()
{
	return m_commandName;
}

std::string Command::GetNextString()
{
	return static_cast<unsigned int>(m_nextCmdArgumentIndex) < m_arguments.size() ? m_arguments[ m_nextCmdArgumentIndex++ ] : std::string("");
}

Rgba Command::GetNextColor()
{
	std::string rgbaStr = GetNextString();
	if( rgbaStr.length() > 2 )
	{
		rgbaStr.erase( 0, 1 );
		rgbaStr.pop_back();
	}
	else
	{
		DebuggerPrintf( "\n\nError: Parsing of RGBA for Console, failed!\n\n" );
	}

	Rgba color = RGBA_WHITE_COLOR;
	if( rgbaStr != "" )
		color.SetFromText( rgbaStr.c_str() );

	return color;
}

void CommandStartup()
{

}

void CommandShutdown()
{

}

void CommandRegister( char const *name, command_cb cb )
{
	g_registeredCommands[ std::string( name ) ] = *cb;
}

bool CommandRun( char const *command )
{
	Command commandToRun( command );
	std::string commandName = commandToRun.GetName();

	std::map< std::string, command_cb >::iterator it = g_registeredCommands.find( commandName );
	if( it != g_registeredCommands.end() )
	{
		it->second( commandToRun );
		return true;
	}
	else
	{
		DebuggerPrintf( "\nERROR: Command %s not registered..\n", commandName.c_str() );
		return false;
	}
}

std::vector< std::string > GetAllRegisteredCommands()
{
	std::vector< std::string > toReturn;

	std::map< std::string, command_cb >::iterator it;
	for( it = g_registeredCommands.begin(); it != g_registeredCommands.end(); it++ )
	{
		toReturn.push_back( it->first );
	}

	return toReturn;
}