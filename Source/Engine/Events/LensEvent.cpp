#include "LensEvent.h"

USING_AWAY_NAMESPACE

const std::string LensEvent::MATRIX_CHANGED = "MatrixChanged";

LensEvent::LensEvent(const std::string& type, LensBase* lens) : Event(type)
{
	m_lens = lens;
}