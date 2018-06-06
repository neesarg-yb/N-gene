#include "XMLUtilities.hpp"


int ParseXmlAttribute( const XMLElement& element, const char* attributeName, int defaultValue )
{
	int returnValue = defaultValue;
	const char* fetchedString = element.Attribute(attributeName);
	
	if( fetchedString != NULL )
		   SetFromText( returnValue , fetchedString );

	return returnValue;
}

char ParseXmlAttribute( const XMLElement& element, const char* attributeName, char defaultValue )
{
	char returnValue = defaultValue;
	const char* fetchedString = element.Attribute(attributeName);

	if( fetchedString != NULL )
		returnValue = fetchedString[0];

	return returnValue;
}
 
 bool ParseXmlAttribute( const XMLElement& element, const char* attributeName, bool defaultValue )
 {
	bool returnValue = defaultValue;
	const char* fetchedString = element.Attribute(attributeName);

	if( fetchedString != NULL )
		SetFromText( returnValue , fetchedString );

	return returnValue;
 }
  
 float ParseXmlAttribute( const XMLElement& element, const char* attributeName, float defaultValue )
 {
	 float returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 SetFromText( returnValue , fetchedString );

	 return returnValue;
 }

 Rgba ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Rgba& defaultValue )
 {
	 Rgba returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetFromText( fetchedString );

	 return returnValue;
 }

 Vector2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Vector2& defaultValue )
 {
	 Vector2 returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetFromText( fetchedString );

	 return returnValue;
 }

 Vector3 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Vector3& defaultValue )
 {
	 Vector3 returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetFromText( fetchedString );

	 return returnValue;
 }

 IntRange ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IntRange& defaultValue )
 {
	 IntRange returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetFromText( fetchedString );

	 return returnValue;
 }

 FloatRange ParseXmlAttribute( const XMLElement& element, const char* attributeName, const FloatRange& defaultValue )
 {
	 FloatRange returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetFromText( fetchedString );

	 return returnValue;
 }

 IntVector2 ParseXmlAttribute( const XMLElement& element, const char* attributeName, const IntVector2& defaultValue )
 {
	 IntVector2 returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetFromText( fetchedString );

	 return returnValue;
 }

 char const * ParseXmlAttribute( const XMLElement& element, char const* attributeName, char const* defaultValue )
 {
	 char const *returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue = fetchedString;

	 return returnValue;
 }

 std::vector<int> ParseXmlAttribute( const XMLElement& element, const char* attributeName, const std::vector<int>& defaultValue )
 {
	 std::vector<int> returnVector = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
	 {
		 std::vector<std::string> rowStrings;
		 SetFromText( rowStrings, ",", fetchedString );

		 for( unsigned int i = 0; i<rowStrings.size(); i++ )
		 {
			 int number;
			 SetFromText( number, rowStrings.at(i).c_str() );
			 returnVector.push_back( number );
		 }
	 }

	 return returnVector;
 }

 std::string ParseXmlAttribute( const XMLElement& element, const char* attributeName, const std::string& defaultValue )
 {
	 std::string returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue = std::string(fetchedString);

	 return returnValue;
 }


 AABB2	ParseXmlAttribute( const XMLElement& element, const char* attributeName, const AABB2& defaultValue )
 {
	 AABB2 returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetFromText( fetchedString );

	 return returnValue;
 }

 Tags ParseXmlAttribute( const XMLElement& element, const char* attributeName, const Tags& defaultValue )
 {
	 Tags returnValue = defaultValue;
	 const char* fetchedString = element.Attribute(attributeName);

	 if( fetchedString != NULL )
		 returnValue.SetOrRemoveTags( std::string(fetchedString) );

	 return returnValue;
 }