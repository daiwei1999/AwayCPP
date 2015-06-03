#include "ShadingMethodEvent.h"

USING_AWAY_NAMESPACE

const std::string ShadingMethodEvent::SHADER_INVALIDATED = "ShaderInvalidated";

ShadingMethodEvent::ShadingMethodEvent(const std::string& type) : Event(type)
{

}