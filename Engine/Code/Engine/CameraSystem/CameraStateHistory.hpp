#pragma once
#include <vector>
#include "Engine/CameraSystem/CameraState.hpp"

class CameraStateHistoy
{
public:
	 CameraStateHistoy( int maxHistoryLength );
	~CameraStateHistoy();

public:
	int const m_maxHistoryLength = -1;

private:
	std::vector< CameraState > m_history;
	int m_lastAddedEntryIndex = -1;

public:
	void			AddNewEntry( CameraState const &newState );
	CameraState		GetRecentEntry( int numEntriesToSkip ) const;
	CameraState		GetAverageOfRecentEntries( int numEntries ) const;
};