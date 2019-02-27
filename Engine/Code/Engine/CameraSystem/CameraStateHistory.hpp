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
	bool			IsNotEmpty() const;
	int				GetCurrentCountOfEntries() const;
	void			AddNewEntry( CameraState const &newState );
	CameraState		GetRecentEntry( int numEntriesToSkip ) const;
	CameraState		GetAverageOfRecentEntries( int numEntries ) const;
	CameraState		GetProgressiveAverageOfRecentEntries( int numEntries ) const;
};


//-------------------
// Inline Definitions
inline int CameraStateHistoy::GetCurrentCountOfEntries() const
{
	return (int)m_history.size();
}

inline bool CameraStateHistoy::IsNotEmpty() const
{
	return (m_history.size() > 0U);
}
