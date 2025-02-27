#include "EPlayer.h"
#include "Using.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <functional>

const Miliseconds m_timePlayer = (Miliseconds)(60 * 1000);
const Miliseconds m_timeExtra = (Miliseconds)(5 * 1000);
const Miliseconds m_timeCheck = (Miliseconds)10;

class Timer
{
public:
	Timer();

	Timer(const Timer& other) = delete;
	Timer& operator=(const Timer& other) = delete;

	~Timer();

	void Start();
	void Stop();
	void Reset();

	Miliseconds GetElapsedTime() const;
	Miliseconds GetPlayerTime(EPlayer player) const;
	Miliseconds GetTimeAction(EPlayer player) const;

	bool IsRunning() const;

	void AddTime(EPlayer player);

	void SwitchPlayers();

	void OppositePlayer();

	void SetAction(Miliseconds time, std::function<void(void)> function)
	{
		std::lock_guard<std::mutex> lock(m_timerMutex);
		m_extraFunction = function;
		m_extraFunctionTime = std::chrono::system_clock::now() + time;
	}

	void ExecuteFunction()
	{
		if (m_extraFunction)
		{
			m_extraFunction();
			
			m_extraFunction = {};
		}
	}

private:
	std::thread m_timerThread;
	std::condition_variable m_timerCondVariable;
	mutable std::mutex m_timerMutex;

	bool m_isRunning;

	EPlayer m_player;
	Miliseconds  m_player1Remaining, m_player2Remaining, m_elapsedTime,m_timeStamp;
	
	std::function<void(void)> m_extraFunction;
	std::chrono::system_clock::time_point m_extraFunctionTime;
};