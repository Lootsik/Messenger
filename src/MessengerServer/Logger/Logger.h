#pragma once



namespace Logger {

enum LogType
{
	Success,
	Action,
	Mistake,
	Error
};

//Opens file for logging
//If file cannot be opened all info will be printed on terminal
bool OpenLogFile(const char* Filename);

//close file for logging
void CloseLogFile();

//log to terminal
void Log(const int TypeEvent, const char* format...);

//log to file
void LogFile(const int TypeEvent, const char* format...);

void LogBoth(const int TypeEvent, const char* format...);

}
