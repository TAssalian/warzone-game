#pragma once

#include <iostream>
#include <string>
#include <vector>

//ILoggable interface for any class that can be logged
//stringToLog() method returns a string representation of the object for logging purposes

class ILoggable {
public:
	ILoggable();
	ILoggable(const ILoggable& other);
	ILoggable& operator=(const ILoggable& other);
	virtual ~ILoggable();

	virtual std::string stringToLog() const = 0;

	friend std::ostream& operator<<(std::ostream& os, const ILoggable& loggable);
};

//Observer
//update is call by a subject to notify of changes

class Observer {


public:
	Observer();
	Observer(const Observer& other);
	Observer& operator=(const Observer& other);
	virtual ~Observer();

	virtual void update(ILoggable* loggable) = 0;

	friend std::ostream& operator<<(std::ostream& os, const Observer& observer);
};

//subject
//maintains a list of observers
//attach(), detach() and notify() methods to manage observers and send notifications
class Subject {
private:
	std::vector<Observer*>* observers;
public:
	Subject();
	Subject(const Subject& other);
	Subject& operator=(const Subject& other);
	virtual ~Subject();
	virtual void attach(Observer* observer);
	virtual void detach(Observer* observer);
	virtual void notify(ILoggable* loggable);
	friend std::ostream& operator<<(std::ostream& os, const Subject& subject);

	
};
//LogObserver
//concrete observer that implements the update method to log changes to a file specified by logFileName
class LogObserver : public Observer {
private:
	std::string* logFileName;
public:
	explicit LogObserver(const std::string& logFileName);
	LogObserver(const LogObserver& other);
	LogObserver& operator=(const LogObserver& other);
	~LogObserver();

	void update(ILoggable* loggable);

	friend std::ostream& operator<<(std::ostream& os, const LogObserver& logObserver);

};