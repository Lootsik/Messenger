// API.cpp : Defines the entry point for the console application.
//
#include "MessengerAPI.h"

int main()
{
	MessengerAPI api;
	api.Connect("127.0.0.1", 8021);
	api.TryLogin("w", "w");
	while (1);
}

