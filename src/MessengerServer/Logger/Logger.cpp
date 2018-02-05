#define _CRT_SECURE_NO_WARNINGS

#if defined (_WIN32) || (_WIN64)
	#include <Windows.h>
#endif 
#include <stdio.h>
#include <stdarg.h>
#include <ctime>

#include "Logger.h"

static void TimeToString(const tm* Time, char* Buff)
{
	sprintf(Buff, "%02d:%02d:%02d  %02d:%02d\0", Time->tm_mday,
												 Time->tm_mon + 1,
												 Time->tm_year % 100,
												 Time->tm_hour,
												 Time->tm_min);
}

static tm* CurTime()
{
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);
	return now;
}


namespace Logger{

const char* const TypeDesc[] ={
								"Success",
								"Action",
								"Mistake",
								"Error"
							   };

//buffer for string date 
static char TimeBuffer[16];
static FILE* File;

//additional info to windows colored characters
#ifdef _WINDOWS_
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	short ConsoleColor(HANDLE& hConsole)
	{
		CONSOLE_SCREEN_BUFFER_INFO info;
		if (!GetConsoleScreenBufferInfo(hConsole, &info))
		//dont know what do with this
			throw;

		return info.wAttributes;
	}
	//save only initial color of console
	static short ConsoleSaveColor = ConsoleColor(hConsole);
	const char TypeColors[] = { 10,15,13,4 };
#endif


static char* TimeNow()
{
	TimeToString(CurTime(), TimeBuffer);
	return TimeBuffer;
}

//TODO: mb rewrite this
//print colored type
static void PrintType(FILE* file, const int TypeEvent)
{
	if (file == stdout)
	{
#ifdef _WINDOWS_
		if (!hConsole)
		{
			fprintf(file, TypeDesc[TypeEvent]);
			return;
		}

		SetConsoleTextAttribute(hConsole, TypeColors[TypeEvent]);
#endif

		fprintf(file, TypeDesc[TypeEvent]);

#ifdef _WINDOWS_
		SetConsoleTextAttribute(hConsole, ConsoleSaveColor);
#endif
	}
	else
		fprintf(file, TypeDesc[TypeEvent]);
}
//Log to specific file
static void _VLog(FILE* file, const int TypeEvent, const char* format, va_list args)
{
	fprintf(file, "[%s] [", TimeNow());
	PrintType(file, TypeEvent);
	fprintf(file, "]: ");

	vfprintf(file, format, args);

	putc('\n', file);
}


bool OpenLogFile(const char* Filename)
{
	File = fopen(Filename, "w");
	if (!File) {
		Log(Error, "Log file: \"%s\" cannot be opened", Filename);
	}
	return File;
}
static void Flush()
{
	fflush(File);
}

void CloseLogFile()
{
	fclose(File);
}

//log to terminal
void Log(const int TypeEvent, const char* format...)
{
	va_list args;
	va_start(args, format);
	_VLog(stdout, TypeEvent, format, args);
	va_end(args);
}

void LogFile(const int TypeEvent, const char* format...)
{
	va_list args;
	va_start(args, format);
	if( File )
		_VLog(File, TypeEvent, format, args);
	else
		_VLog(stdout, TypeEvent, format, args);
	
	va_end(args);
	//TODO: maybe better flushing strategy needed
	Flush();
}

void LogBoth(const int TypeEvent, const char* format...)
{
	va_list args;
	va_start(args, format);
	_VLog(stdout, TypeEvent, format, args);
	//to prevent double message when File is not open
	if( File )
		_VLog(File,  TypeEvent, format, args);
	va_end(args);
}

}