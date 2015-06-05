#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

template<typename Callback, typename Function> inline
bool func_compare(const Function &lhs, const Function &rhs)
{
	typedef typename std::conditional<std::is_function<Callback>::value, typename std::add_pointer<Callback>::type, Callback> ::type request_type;

	if (const request_type *lhs_internal = lhs.template target<request_type>())
		if (const request_type *rhs_internal = rhs.template target<request_type>())
			return *rhs_internal == *lhs_internal;

	return false;
}

template<typename function_signature>
struct function_comparable : std::function<function_signature>
{
	typedef std::function<function_signature> Function;
	bool (*type_holder)(const Function &, const Function &);
public:
	function_comparable() {}
	template<typename Func> function_comparable(Func f) : Function(f), type_holder(func_compare<Func, Function>)
	{
	}
	template<typename Func> function_comparable &operator=(Func f)
	{
		Function::operator=(f);
		type_holder = func_compare<Func, Function>;
		return *this;
	}
	friend bool operator==(const Function &lhs, const function_comparable &rhs)
	{
		return rhs.type_holder(lhs, rhs);
	}
	friend bool operator==(const function_comparable &lhs, const Function &rhs)
	{
		return rhs == lhs;
	}
	friend void swap(function_comparable &lhs, function_comparable &rhs)
	{
		lhs.swap(rhs);
		lhs.type_holder.swap(rhs.type_holder);
	}
};

class EventDispatcher
{
	typedef std::vector<std::function<void(Event*)>> ListenerVector;
	typedef std::map<const std::string, ListenerVector> ListenersMap;

public:
	void addEventListener(const std::string& type, std::function<void(Event*)> listener);
	void removeEventListener(const std::string& type, function_comparable<void(Event*)> listener);
	bool hasEventListener(const std::string& type);
	void dispatchEvent(Event* event);

private:
	ListenersMap m_listenersMap;
};

AWAY_NAMESPACE_END