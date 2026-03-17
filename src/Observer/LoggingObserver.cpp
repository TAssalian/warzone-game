#include "LoggingObserver.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::string, std::ostream, std::vector,std::ofstream;
class ILoggable;
class Observer;
class Subject;
class LogObserver;
//ILoggable implementation

ILoggable::ILoggable() {};
ILoggable::ILoggable(const ILoggable& other) {};
ILoggable& ILoggable::operator=(const ILoggable& other) {
	if (this != &other) {
	}
	return *this;
}
ILoggable::~ILoggable() {};

ostream& operator<<(std::ostream& os, const ILoggable& loggable) {
	os << loggable.stringToLog();
	return os;
}

//Observer implementation
Observer::Observer() {};
Observer::Observer(const Observer& other) {};
Observer& Observer::operator=(const Observer& other) {
	if (this != &other) {
	}
	return *this;
}
Observer::~Observer() {};
ostream& operator<<(std::ostream& os, const Observer& observer) {
	os << "Observer";
	return os;
}

//Subject implementation
Subject::Subject() {
	observers = new vector<Observer*>();
};
Subject::Subject(const Subject& other) {
	this->observers = new vector<Observer*>(*other.observers);
}
Subject& Subject::operator=(const Subject& other) {
	if (this != &other) {
		delete this->observers;
		this->observers = new vector<Observer*>(*other.observers);
	}
	return *this;
}
Subject::~Subject() {
	delete observers;
};

//attach adds an observer at the end of the list of observers
void Subject::attach(Observer* observer) {
	observers->push_back(observer);
}
//dettach removes an observer from the list of observers
//by using std::remove to move the observer to the end of the vector and then erasing it
void Subject::detach(Observer* observer) {
	observers->erase(
		std::remove(observers->begin(), observers->end(), observer), observers->end());
}
//notify calls the update method
//for every observer
void Subject::notify(ILoggable* loggable) {
	for (Observer* observer : *observers) {
		observer->update(loggable);
	}
}
ostream& operator<<(ostream& os, const Subject& subject) {
	os << "Subject[observer count: " << subject.observers->size() << "] ";
	return os;
}

//log observer implementation
//constructor takes a log file name
//initializes the log file by opening it in trunc mode to clear any existing content
LogObserver::LogObserver(const string& logFileName) {
	this->logFileName = new string(logFileName);
	ofstream ofs(*this->logFileName, ofstream::trunc);
	if(!ofs.is_open()) {
		std::cerr << "Error opening log file: " << *this->logFileName << std::endl;
	}
	ofs.close();

}

LogObserver::~LogObserver() {
	delete logFileName;
}
//this method is called by the subject when it notifies its observers of a change
//it opens the log file in append mode and writes the string representation of the loggable object to the file
void LogObserver::update(ILoggable* loggable) {
	ofstream ofs(*logFileName, ofstream::app);
	if(!ofs.is_open()) {
		std::cerr << "Error opening log file: " << *logFileName << std::endl;
		return;
	}
	ofs << loggable->stringToLog() << std::endl;
	ofs.close();
}
ostream& operator<<(ostream& os, const LogObserver& logObserver) {
	os << "LogObserver[logFileName: " << *logObserver.logFileName << "]";
	return os;
}