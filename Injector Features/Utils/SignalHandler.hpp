#pragma once

#ifndef STD_INCLUDED
#error "Missing standard header"
#endif

namespace Utils
{
	class SignalHandler : std::lock_guard<std::mutex>
	{
	public:
		SignalHandler(Utils::Slot<void()> callback);
		~SignalHandler();

	private:
		Utils::Slot<void()> callback;

		static std::mutex Mutex;
		static SignalHandler* MainHandler;

#ifdef _WIN32
		static BOOL WINAPI Handler(DWORD signal);
#elif defined(_POSIX)
		static void Handler(int signal);
#endif
	};
}
