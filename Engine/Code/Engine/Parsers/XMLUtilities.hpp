//================================================================================================================================
// XMLUtilities.hpp
//================================================================================================================================


#pragma once
#ifndef __included_XMLUtilities__
#define __included_XMLUtilities__

#include <string>
#include "ThirdParty/Parsers/XmlParser.h"
#include "Engine/Core/StringUtils.hpp"


//================================================================================================================================
//
//================================================================================================================================
std::string			GetXMLErrorDescForErrorCode( XMLError xmlErrorCode );
bool				GetXMLNodeByNameSearchingFromPosition( const XMLNode& parentNode, const std::string& childName, int& position_inout, XMLNode& childNode_out );
std::string			GetXMLAttributeAsString( const XMLNode& node, const std::string& attributeName, bool& wasAttributePresent_out );
void				DestroyXMLDocument( XMLNode& xmlDocumentToDestroy );


///-----------------------------------------------------------------------------------
///
///-----------------------------------------------------------------------------------
template< typename ValueType >
ValueType GetXMLAttributeOfType( const XMLNode& node, const std::string& propertyName, bool& wasPropertyPresent_out )
{
	ValueType	outValue ;
	std::string	valueAsString	= GetXMLAttributeAsString( node, propertyName, wasPropertyPresent_out );
	SetTypeFromString( outValue, valueAsString );

	return outValue;
}


///-----------------------------------------------------------------------------------
///
///-----------------------------------------------------------------------------------
template< typename ValueType >
ValueType ReadXMLAttribute( const XMLNode& node, const std::string& propertyName, const ValueType& defaultValue )
{
	bool		wasPropertyPresent	= false ;

	ValueType	outValue			= GetXMLAttributeOfType< ValueType >( node, propertyName, wasPropertyPresent );
	if ( !wasPropertyPresent )
		outValue	= defaultValue ;

	return outValue;
}


///-----------------------------------------------------------------------------------
///
///-----------------------------------------------------------------------------------
template< typename ValueType >
void WriteXMLAttribute( XMLNode& node, const std::string& propertyName, ValueType& value, const ValueType& defaultValue )
{
	SetStringProperty( node, propertyName, GetTypedObjectAsString( value ), GetTypedObjectAsString( defaultValue ) );
}




#endif // __included_XMLUtilities__
