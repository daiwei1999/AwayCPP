#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * MethodVOSet provides a EffectMethodBase and MethodVO combination to be used by a material, allowing methods
 * to be shared across different materials while their internal state changes.
 */
class MethodVOSet
{
public:
	MethodVOSet(EffectMethodBase* method);

public:
	/**
	 * An instance of a concrete EffectMethodBase subclass.
	 */
	EffectMethodBase* m_method;

	/**
	 * The MethodVO data for the given method containing the material-specific data for a given material/method combination.
	 */
	MethodVO* m_data;
};

AWAY_NAMESPACE_END