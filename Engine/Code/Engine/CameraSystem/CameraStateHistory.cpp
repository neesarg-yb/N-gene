#pragma once
#include "CameraStateHistory.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

CameraStateHistoy::CameraStateHistoy( int maxHistoryLength )
	: m_maxHistoryLength( maxHistoryLength )
{

}

CameraStateHistoy::~CameraStateHistoy()
{

}

void CameraStateHistoy::AddNewEntry( CameraState const &newState )
{
	// Increment the last entry index
	m_lastAddedEntryIndex++;
	m_lastAddedEntryIndex = m_lastAddedEntryIndex % m_maxHistoryLength;

	if( m_history.size() < m_maxHistoryLength )
		m_history.push_back( newState );					// If we haven't reached the max limit yet, just push back
	else
		m_history[ m_lastAddedEntryIndex ] = newState;		// Or we'll just replace the content
}

CameraState CameraStateHistoy::GetRecentEntry( int numEntriesToSkip ) const
{
	// Make sure the argument is positive
	GUARANTEE_RECOVERABLE( numEntriesToSkip >= 0, "GetRecentEntry can not take negative number as argument!" );

	// If we haven't filled the full history
	int totalEntries = (int) m_history.size(); 
	if( totalEntries < m_maxHistoryLength )
	{
		// Make sure the entries to skip is not too early
		GUARANTEE_RECOVERABLE( (totalEntries - numEntriesToSkip) >= 1 , "Trying to access out of range CameraStateHistory!!" );
	}
	else
	{
		// Make sure user is not asking for an entry older than max capacity
		GUARANTEE_RECOVERABLE( numEntriesToSkip < m_maxHistoryLength, "Trying to access out of range CameraStateHistory!!" );
	}

	int indexToGet = (m_lastAddedEntryIndex - numEntriesToSkip);
	if( indexToGet < 0 )
		indexToGet += m_maxHistoryLength;

	return m_history[ indexToGet ];
}

CameraState CameraStateHistoy::GetAverageOfRecentEntries( int numEntries ) const
{
	GUARANTEE_RECOVERABLE( numEntries > 0, "CameraStateHistory: Provided invalid numEntries for average calculation!" );

	std::vector< CameraState > fetchedEntries;
	for( int entriesToSkip = 0; entriesToSkip < numEntries; entriesToSkip++ )
		fetchedEntries.push_back( GetRecentEntry( entriesToSkip ) );

	Vector3		avgVelocity		= Vector3::ZERO;
	Vector3		avgPosition		= Vector3::ZERO;
	Quaternion	avgOrientation	= Quaternion::IDENTITY;
	float		avgFOV			= 0.f;
	float const scaleDownBy		= 1.f / numEntries;

	// From most oldest to most recent
	for( int i = numEntries-1; i >= 0; i-- )
	{
		CameraState const &ithState = fetchedEntries[i];
		avgVelocity += (ithState.m_velocity * scaleDownBy);
		avgPosition += (ithState.m_position * scaleDownBy);
		avgFOV		+= (ithState.m_fov		* scaleDownBy);

		Quaternion scaledDownQuaternion = Quaternion::Slerp( Quaternion::IDENTITY, ithState.m_orientation, scaleDownBy );
		avgOrientation = avgOrientation.Multiply( scaledDownQuaternion );
	}

	return CameraState( avgVelocity, avgPosition, avgOrientation, avgFOV );
}

CameraState CameraStateHistoy::GetProgressiveAverageOfRecentEntries( int numEntries ) const
{
	GUARANTEE_RECOVERABLE( numEntries > 0, "CameraStateHistory: Provided invalid numEntries for average calculation!" );

	// Order of entries: [ most recent, .. , most older ]
	std::vector< CameraState > fetchedEntriesNewestToOldest;
	for( int entriesToSkip = 0; entriesToSkip < numEntries; entriesToSkip++ )
		fetchedEntriesNewestToOldest.push_back( GetRecentEntry( entriesToSkip ) );

	// As if we got average of just one entry: the oldest one.
	int const	oldestEntryIndex		= numEntries - 1;
	CameraState progressiveAvgCamState	= fetchedEntriesNewestToOldest[ oldestEntryIndex ];

	// Starting from second entry
	for( int i = 2; i <= numEntries; i++ )
	{
		// Oldest to the most recent
		CameraState const &fetchedCamState	= fetchedEntriesNewestToOldest[ numEntries - i ];
		float const blendTowardsNewEntry	= 1.f / i;

		// Average of Q1, Q2, Q3, Q4 with Quaternions::Slerp( Qa, Qb, t )
		//   _______________________________________________
		//  |  ______________________________               |
		//  | |  _____________               |              |
		//  | | | Q1 ----> Q2 |              |              |
		//  | | |    0.5 t    | ------>  Q3  |  -------> Q4 |
		//  | | |-------------|  0.33 t      |    0.25 t    |
		//  | |------------------------------|              |
		//  |                                               |
		//  |-----------------------------------------------|
		//
		// It works as if we're distributing equal probability, progressively 
		//
		progressiveAvgCamState.m_velocity	 = Interpolate( progressiveAvgCamState.m_velocity, fetchedCamState.m_velocity, blendTowardsNewEntry );
		progressiveAvgCamState.m_position	 = Interpolate( progressiveAvgCamState.m_position, fetchedCamState.m_position, blendTowardsNewEntry );
		progressiveAvgCamState.m_fov		 = Interpolate( progressiveAvgCamState.m_fov, fetchedCamState.m_fov, blendTowardsNewEntry );
		progressiveAvgCamState.m_orientation = Quaternion::Slerp( progressiveAvgCamState.m_orientation, fetchedCamState.m_orientation, blendTowardsNewEntry );
	}

	return progressiveAvgCamState;
}
