#include "pch.h"
#include "Engine/Core/Thread.h"

#include <pthread.h>

namespace Engine {

	Thread::Thread(const std::string& name)
		: m_Name(name)
	{
	}

	void Thread::SetName(const std::string& name)
	{
		pthread_setname_np(m_Thread.native_handle(), name.c_str());
	}

	void Thread::Join()
	{
		m_Thread.join();
	}

	// TODO(Emily): `ThreadSignal`
}
