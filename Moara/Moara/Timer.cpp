#include "Timer.h"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

using namespace std::chrono;

Timer::Timer()
	: m_isRunning(false)
	, m_player1Remaining(m_timePlayer)
	, m_player2Remaining(m_timePlayer)
	, m_player(EPlayer::Player1)
	, m_elapsedTime(0)
	, m_timeStamp(0)
{
}

void Timer::Start()
{
	if (m_isRunning)
		return;

	m_isRunning = true;

	m_timerThread = std::thread([this]()
		{
			while (m_isRunning)
			{
				auto moveStartTime = system_clock::now();

				{
					std::unique_lock<std::mutex> lock(m_timerMutex);
					m_timerCondVariable.wait_for(lock, m_timeCheck, [&]
						{
							return !m_isRunning;
						});
				}

				auto moveEndTime = system_clock::now();
				auto timePassed = duration_cast<Miliseconds>(moveEndTime - moveStartTime);

				{
					std::lock_guard<std::mutex> lock(m_timerMutex);

					if (m_extraFunction && moveEndTime >= m_extraFunctionTime)
					{
						ExecuteFunction();
						m_timeStamp = m_elapsedTime;
					}

					auto remaining = m_player == EPlayer::Player1 ? &m_player1Remaining : &m_player2Remaining;
					*remaining -= timePassed;

					m_elapsedTime += timePassed;
				
					if ( *remaining <= Miliseconds(0) )
					{
						m_isRunning = false;
					}
				}

			}
		});
}

void Timer::SwitchPlayers()
{
	std::lock_guard<std::mutex> lock(m_timerMutex);

	{
		AddTime(m_player);
		m_player = (EPlayer)(1 - (int)m_player);
	}

	m_timeStamp = m_elapsedTime;
	m_timerCondVariable.notify_one();
}

void Timer::OppositePlayer()
{
	m_player = (EPlayer)(1 - (int)m_player);
}

void Timer::Stop()
{
	{
		std::lock_guard<std::mutex> lock(m_timerMutex);
			
		m_isRunning = false;
	}

	m_timerCondVariable.notify_one();

	if (m_timerThread.joinable())
		m_timerThread.join();
}

void Timer::Reset()
{
	Stop();

	{
		std::lock_guard<std::mutex> lock(m_timerMutex);
		m_player1Remaining = m_timePlayer;
		m_player2Remaining = m_timePlayer;
		m_elapsedTime = Miliseconds(0);
		m_player = EPlayer::Player1;
		m_timeStamp = Miliseconds(0);
	}

	Start();
}

Miliseconds Timer::GetElapsedTime() const
{
	std::lock_guard<std::mutex> lock(m_timerMutex);
	return m_elapsedTime;
}

Miliseconds Timer::GetPlayerTime(EPlayer player) const
{
	std::lock_guard<std::mutex> lock(m_timerMutex);
	return player == EPlayer::Player1 ? m_player1Remaining : m_player2Remaining;
}

Miliseconds Timer::GetTimeAction(EPlayer player) const
{
	Miliseconds time = GetElapsedTime() - m_timeStamp +1s;

	if (time < (Miliseconds)0)
	{
		time = (Miliseconds)0;
	}

	return time;
}



bool Timer::IsRunning() const
{
	std::lock_guard<std::mutex> lock(m_timerMutex);
	return m_isRunning;
}

Timer::~Timer()
{
	Stop();
}

void Timer::AddTime(EPlayer player)
{
	
	if (player == EPlayer::Player1)
	{
		m_player1Remaining += m_timeExtra;
	}
	else
	{
		m_player2Remaining += m_timeExtra;
	}
}
