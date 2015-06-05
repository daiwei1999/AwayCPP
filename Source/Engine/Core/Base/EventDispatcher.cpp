#include "EventDispatcher.h"
#include "Event.h"

USING_AWAY_NAMESPACE

void EventDispatcher::addEventListener(const std::string& type, std::function<void(Event*)> listener)
{
	ListenersMap::iterator iter = m_listenersMap.find(type);
	if (iter == m_listenersMap.end())
		m_listenersMap[type] = { listener };
	else
		iter->second.push_back(listener);
}

void EventDispatcher::removeEventListener(const std::string& type, function_comparable<void(Event*)> listener)
{
	ListenersMap::iterator iter = m_listenersMap.find(type);
	if (iter != m_listenersMap.end())
	{
		ListenerVector& listeners = iter->second;
		ListenerVector::iterator iter2 = std::find(listeners.begin(), listeners.end(), listener);
		if (iter2 != listeners.end())
			listeners.erase(iter2);
	}
}

bool EventDispatcher::hasEventListener(const std::string& type)
{
	return m_listenersMap.find(type) != m_listenersMap.end();
}

void EventDispatcher::dispatchEvent(Event* event)
{
	ListenersMap::iterator iter = m_listenersMap.find(event->getType());
	if (iter != m_listenersMap.end())
	{
		ListenerVector& listeners = iter->second;
		for (ListenerVector::iterator iter2 = listeners.begin(); iter2 != listeners.end(); iter2++)
			(*iter2)(event);
	}
}