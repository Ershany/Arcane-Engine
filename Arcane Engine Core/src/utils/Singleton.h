#pragma once

namespace arcane {

	class Singleton {
	public:
		Singleton(const Singleton &singleton) = delete;  // Get rid of copy ctor
		Singleton(const Singleton &&singleton) = delete; // Get rid of move ctor
		
		Singleton& operator=(const Singleton &singleton) = delete;
		Singleton& operator=(const Singleton &&singleton) = delete;
	protected:
		Singleton() = default;
		virtual ~Singleton() = default;
	};

}
