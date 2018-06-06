#pragma once
#include <map>
#include <string>
#include <vector>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

class Command;

// Command callbacks take a Command.
typedef void (*command_cb)( Command &cmd ); 

// A command is a single submitted command
// NOT the definition (which I hide internally)
// Comments will be using a Command constructed as follows; 
// Command cmd = Command( "echo_with_color (255,255,0) \"Hello \\"World\\"\" ); 
class Command
{
public:
	 Command( char const *str ); 
	~Command();

private:
	const std::string			m_commandString;
	std::string					m_commandName;
	std::vector<std::string>	m_arguments;
	int							m_nextCmdArgumentIndex = 0;

public:
	std::string GetName(); // would return "echo_with_color"

						   // Gets the next string in the argument list.
						   // Breaks on whitespace.  Quoted strings should be treated as a single return 
	std::string GetNextString();   // would return after each call...
								   // first:  "(255,255,0)""
								   // second: "Hello \"world\""
								   // third+: ""

								   // [OPTIONAL] I like to add helpers for getting arguments
								   // and I just add them as I need them.
								   // Each takes the output variable, and returns whether it succeeded
								   // bool GetNextInt( int *out_val ); 
								   // bool GetNextColor( RGBA *out_val );
								   // bool GetNextVector2( Vector2 *out_val ); 
								   // bool GetNextVector3( Vector3 *out_val );
								   // ... 
	Rgba GetNextColor();
};

// Allows for setting up the system and cleaning up.
// Optional, but may help with some tasks. 
void CommandStartup(); 
void CommandShutdown(); 

// Registers a command with the system
// Example, say we had a global function named...
//    void Help( Command &cmd ) { /* ... */ }  
// 
// We then, during some startup, call
//    CommandRegister( "help", Help ); 
void CommandRegister( char const *name, command_cb cb ); 

// Will construct a Command object locally, and if 
// a callback is associated with its name, will call it and 
// return true, otherwise returns false.
// Name is case-insensitive
bool CommandRun( char const *command ); 

std::vector< std::string > GetAllRegisteredCommands();

// *********************************************************************//
// ** EXTRAS ** //
// *********************************************************************//

// [E02.00]
// Returns a list containing the last entered
// commands ordered by how recently they were used.
// If a command is entered twice, it should not appear
// twice, but instead just just move to the front. 
// History length is up to you, but 32 or 64 is good.
std::vector<std::string> GetCommandHistory(); 

// [E02.01]
// Runs a "script", or multiple lines of commands (each line
// is treated as a command). 
// Leading whitespace is ignored, and 
// lines starting with '#' should be ignored. 
void CommandRunScript( char const *script ); 
void CommandRunScriptFile( char const *filename ); 

// Returns a list of commands that start with the root word
// should ignore case. 
std::vector<std::string> CommandAutoComplete( char const *root ); 
