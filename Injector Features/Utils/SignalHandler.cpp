#include "STDInclude.hpp"

namespace Utils
{
	std::mutex SignalHandler::Mutex;
	SignalHandler* SignalHandler::MainHandler;

#ifdef _WIN32
	BOOL WINAPI SignalHandler::Handler(DWORD signal)
	{
		if ((signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) && SignalHandler::MainHandler && SignalHandler::MainHandler->callback)
		{
			SignalHandler::MainHandler->callback();
		}

		return TRUE;
	}

#elif defined(_POSIX)
	void SignalHandler::Handler(int signal)
	{
		if (signal == SIGINT && SignalHandler::MainHandler && SignalHandler::MainHandler->callback)
		{
			SignalHandler::MainHandler->callback();
		}
	}
#endif

	SignalHandler::SignalHandler(Utils::Slot<void()> _callback) : std::lock_guard<std::mutex>(SignalHandler::Mutex), callback(_callback)
	{
#ifdef _WIN32
		SetConsoleCtrlHandler(SignalHandler::Handler, TRUE);
#elif defined(_POSIX)
		signal(SIGINT, SignalHandler::Handler);
#endif

		SignalHandler::MainHandler = this;
	}

	SignalHandler::~SignalHandler()
	{
#ifdef _WIN32
		SetConsoleCtrlHandler(SignalHandler::Handler, FALSE);
#elif defined(_POSIX)
		signal(SIGINT, SIG_DFL);
#endif

		SignalHandler::MainHandler = nullptr;
	}
}
