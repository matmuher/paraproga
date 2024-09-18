#include <chrono>
#include <iostream>

namespace SimpleTimer
{
	class Stopwatch
	{
	private:
		const double NANOSECONDS  = 1000000000.0;
		const double MILLISECONDS = 1000000.0;
		const double MICROSECONDS = 1000.0;
		bool m_isRunning = false;
		bool m_hasRun = false; //If the timer has run, get the last result.
		std::chrono::time_point<std::chrono::high_resolution_clock> m_t1 = std::chrono::high_resolution_clock::now();
		std::chrono::time_point<std::chrono::high_resolution_clock> m_t2 = std::chrono::high_resolution_clock::now();
	public:
		void Start() 
		{ 
			m_isRunning = true; 
			m_hasRun = false; 
			m_t1 = std::chrono::high_resolution_clock::now(); 
		} 
		void Stop() 
		{
				if(m_isRunning)
				{
						m_isRunning = false;
						m_hasRun = true;
						m_t2 = std::chrono::high_resolution_clock::now(); 
				}
				else std::cout << "Error stopping timer: Timer not running\n";
		} 
		double GetStopwatchSeconds() 
		{ 
			if(m_hasRun) return std::chrono::duration_cast<std::chrono::nanoseconds>(m_t2 - m_t1).count() / NANOSECONDS; 
			else return -1;
		}
		double GetStopwatchMilliseconds()
		{
			if(m_hasRun) return std::chrono::duration_cast<std::chrono::nanoseconds>(m_t2 - m_t1).count() / MILLISECONDS; 
			else return -1;
		}
		double GetStopwatchMicroseconds()
		{
			if(m_hasRun) return std::chrono::duration_cast<std::chrono::nanoseconds>(m_t2 - m_t1).count() / MICROSECONDS; 
			else return -1;
		}
		double GetStopwatchNanoseconds() 
		{ 
			if(m_hasRun) return std::chrono::duration_cast<std::chrono::nanoseconds>(m_t2 - m_t1).count(); 
			else return -1;
		}
	};

	class Timer
	{
	private:
		Stopwatch t;
        const char* msg_ = "It took";
	public:
		Timer(const char* msg) { t.Start(); msg_ = msg;} 
		//Return time in seconds. Returns -1 if called before Start is called.
		double GetTimerSeconds() { return t.GetStopwatchSeconds(); }
		//Return time in nanoseconds. Returns -1 if called before Start is called.
		double GetTimerMilliseconds() { return t.GetStopwatchMilliseconds(); }
		//Return time in nanoseconds. Returns -1 if called before Start is called.
		double GetTimerMicroseconds() { return t.GetStopwatchMicroseconds(); }
		//Return time in nanoseconds. Returns -1 if called before Start is called.
		double GetTimerNanoseconds() { return t.GetStopwatchNanoseconds(); }
		double Stop() { t.Stop(); return t.GetStopwatchSeconds(); }
        ~Timer() {Stop(); std::cout << msg_ << ": " << GetTimerMilliseconds() << '\n';}
	};
}
