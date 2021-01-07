#pragma once
#include <cstdint>
#include <functional>
#include <thread>
#include <chrono>

class Timer
{
public:
	Timer() : Timer({}, -1) {

	}

	Timer(std::function<void(void)>&& cb, int64_t timeout) : m_timeout(timeout), m_stopped(true), m_cb(cb) {

	}

	void start()
	{
		if (m_stopped && m_timeout != -1)
		{
			m_stopped = false;
			std::thread([this]() -> void
				{
					auto pre = std::chrono::steady_clock::now();
					m_cb();
					while (!m_stopped)
					{
						auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - pre);
						if (duration.count() >= m_timeout)
						{
							m_cb();
							pre = std::chrono::steady_clock::now();
						}
						std::this_thread::yield();
					}
				}
			).detach();
		}
	}

	void stop()
	{
		m_stopped = true;
	}

	bool started()
	{
		return !m_stopped;
	}
private:
	int64_t m_timeout;
	bool m_stopped;
	std::function<void(void)> m_cb;
};

