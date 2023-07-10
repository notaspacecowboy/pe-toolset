#pragma once

template <typename T>
class Singleton
{
public:
	static T& instance()
	{
		static T instance;
		return instance;
	}

public:
	virtual ~Singleton() = default;

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

protected:
	Singleton() = default;
};