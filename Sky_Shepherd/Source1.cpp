#include <WS2tcpip.h>
#include <fstream>
#include <string>
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include "Sheep.h"

int time_to_seconds(std::string time)
{
	auto Hours = 0;
	auto Minutes = 0;
	auto Seconds = 0;
	for (auto i = 0; i < 3; i++)
	{
		std::string TimeToken = "Time Token";
		std::size_t Pos = 0;
		std::string Delimeter = ":";
		if ((Pos = time.find(Delimeter)) != std::string::npos)
		{
			TimeToken = time.substr(0, Pos);
			if (i == 0)
				Hours = std::stoi(TimeToken);
			time.erase(0, Pos + Delimeter.length());
			Minutes = std::stoi(TimeToken.substr(0, Pos));
		}
		else
		{
			Seconds = std::stoi(time);
		}
	}
	return Hours * 3600 + Minutes * 60 + Seconds;
}

bool compare(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
	return a.first < b.first;
}

int __cdecl main(int argc, char** argv)
{
	//Open "BBOB.csv"
	std::ifstream InFile;
	InFile.open(R"(C:\Users\Simon\Desktop\study\EWE\SkySheperd\BBOB.csv)");
	//Determine number of rows and columns in the file
	auto NumRows = -2;
	auto NumColumns = 1;
	std::string Line;
	while (!InFile.eof())
	{
		//read row
		std::getline(InFile, Line, '\n');
		if (NumRows == -1)
		{
			std::string Token = "token";
			std::size_t Pos = 0;
			std::string Delimeter = ",";
			while ((Pos = Line.find(Delimeter)) != std::string::npos)
			{
				Token = Line.substr(0, Pos);
				Line.erase(0, Pos + Delimeter.length());
				NumColumns++;
			}
		}
		NumRows++;
	}
	//Close "BBOB.csv"
	InFile.close();
	auto* BBOB = new Sheep[NumRows];
	InFile.open(R"(C:\Users\Simon\Desktop\study\EWE\SkySheperd\BBOB.csv)");
	std::getline(InFile, Line, '\n'); //removes first row
	for (auto i = 0; i < NumRows; i++)
	{
		std::getline(InFile, Line, '\n');
		for (auto j = 0; j < NumColumns; j++)
		{
			std::string Token = "token";
			std::size_t Pos = 0;
			std::string Delimeter = ",";
			if ((Pos = Line.find(Delimeter)) != std::string::npos)
			{
				Token = Line.substr(0, Pos);
				Line.erase(0, Pos + Delimeter.length());
				switch (j)
				{
				case 2:
					BBOB[i].EventTime = Token;
					break;
				case 3:
					BBOB[i].Longitude = std::stod(Token);
					break;
				case 4:
					BBOB[i].Latitude = std::stod(Token);
					break;
				case 5:
					BBOB[i].Altitude = std::stoi(Token);
					break;
				case 6:
					BBOB[i].HeartRate = std::stod(Token);
					break;
				case 7:
					BBOB[i].BreathingRate = std::stod(Token);
					break;
				case 8:
					BBOB[i].Activity = std::stod(Token);
					break;
				case 9:
					BBOB[i].PeakAcceleration = std::stod(Token);
					break;
				case 10:
					BBOB[i].Original = Token;
					break;
				default:
					break;
				}
			}
			else
			{
				BBOB[i].Original = Line;
			}
		}
	}
	auto* p = new std::pair<int, int>[NumRows];
	for (auto i = 0; i < NumRows; i++)
	{
		p[i] = std::make_pair(time_to_seconds(BBOB[i].EventTime), i + 1);
	}
	std::sort(p, p + NumRows, compare);
	auto* CorrectOrder = new Sheep[NumRows];
	for (auto i = 0; i < NumRows; i++)
	{
		CorrectOrder[i] = BBOB[p[i].second - 1];
	}
	delete[] p;
	for (auto i = 0; i < NumRows; i++)
	{
		BBOB[i] = CorrectOrder[i];
	}
	delete[] CorrectOrder;
	InFile.close();

	auto Delay = 0;
	for (auto i = 0; i < NumRows; i++)
	{
		std::string TransmitString;
		if (i == 0)
			Delay = 0;
		else
		{
			Delay = time_to_seconds(BBOB[i].EventTime) - time_to_seconds(BBOB[i - 1].EventTime);
		}
		//sleep for the delay (convert milliseconds to seconds)
		Sleep(Delay * 1000);
		//print data
		TransmitString = "Time: " + BBOB[i].EventTime + " ";
		TransmitString += "Longitude: " + std::to_string(BBOB[i].Longitude) + " ";
		TransmitString += "Latitude: " + std::to_string(BBOB[i].Latitude) + " ";
		TransmitString += "Altitude: " + std::to_string(BBOB[i].Altitude) + '\n';
		std::cout << TransmitString;
	}
	delete[] BBOB;
	system("pause");
	return 0;
}


