#pragma once

#include <unordered_map>
#include <memory>
#include <functional>
#include <cassert>

class CallbackBase
{
public:
	CallbackBase() = default;
	virtual ~CallbackBase() = default;
};

template<typename... Args>
class Callback: public CallbackBase
{
public:
	template<typename TFunc>
	Callback(TFunc&& f): CallbackBase(), m_function(std::forward<TFunc>(f)) {}

	void operator()(Args... args) {
		m_function(std::forward<Args>(args)...);
	}

private:
	std::function<void(Args...)> m_function;
};


template<typename TKey>
class EventCenter
{
public:
	EventCenter() = default;
	~EventCenter() = default;
	EventCenter(EventCenter&) = delete;
	EventCenter(EventCenter&&) = delete;

	template<typename F, typename... Args>
	void RegisterCallback(TKey key, F&& f)
	{
		m_callbacks.emplace(key, std::make_unique<Callback<Args...>>(std::forward<F>(f)));
	}

	template<typename... Args>
	void ExecuteCallback(TKey key, Args&&... args)
	{
		if(m_callbacks.count(key) != 0)
		{
			auto callback = dynamic_cast<Callback<Args...>*>(m_callbacks[key].get());

			//assert if callback is null
			assert(callback != nullptr);

			//execute callback
			(*callback)(std::forward<Args>(args)...);
		}
	}


private:
	std::unordered_map<TKey, std::unique_ptr<CallbackBase>> m_callbacks;
};
