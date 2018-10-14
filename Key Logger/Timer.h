#pragma once

#include<thread>
#include<chrono>

class Timer {


private:
	std::thread thread;
	bool alive = false;

	//Set Number
	long callNumber = -1L;
	//Counter
	long repeatCount = -1L;

	std::chrono::milliseconds interval = std::chrono::milliseconds(0);

	std::function<void(void)> funct = nullptr;

	void SleepAndRun() {
		std::this_thread::sleep_for(interval);

		if (alive)
			Function()();
	}

	void ThreadFunc() {
		if (callNumber == Infinite)
			while (alive)
				SleepAndRun();
		else
			while (repeatCount--)
				SleepAndRun();
	}

public:
	static const long Infinite = -1L;

	Timer() {}

	Timer(const std::function<void(void)> &f)
		:funct(f) {}

	Timer(const std::function<void(void)> &f, const unsigned long &i,
		const long repeat = Timer::Infinite)
		:funct(f), interval(std::chrono::milliseconds(i)), callNumber(repeat) {}

	void Start(bool Async = true) {

		if (alive)
			return;
		alive = true;
		repeatCount = callNumber;

		if (Async)
			thread = std::thread(&Timer::ThreadFunc, this);
		
		else
			this->ThreadFunc();
	}

	void Stop() {
		alive = false;
		thread.join();
	}

	bool IsAlive()const { return alive; }

	void RepeatCounter(const long r) {
		if (alive)
			return;
		callNumber = r;
	}

	long GetLeftCount() const { return repeatCount; }

	long RepeatCount()const { return callNumber; }

	void SetInterval(const unsigned long &i) {
		if (alive)
			return;
		interval = std::chrono::milliseconds(i);
	}

	void SetFunction(std::function<void(void)> function) {
		funct = function;
	}

	unsigned long Interval() const { return interval.count(); }

	const std::function<void(void)>&Function() const {
		return funct;
	}
};


