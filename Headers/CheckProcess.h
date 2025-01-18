#pragma once
#ifndef CHECKPROCESS_H
#define CHECKPROCESS_H
#include "MySQL_DB.h"

#include <algorithm>
#include <codecvt>
#include <locale>
#include <map>
#include <vector>


#ifdef UNICODE
#define tcout std::wcout
#elif
#define tcout std::cout
#endif

using convert_type = std::codecvt_utf8<wchar_t>;

// handle  -  ��� ��� ������ � Windows API, ������� ������������ ��� ����������� ����������� �������,
// ������ ��� �������, �����, ���� � �. �. 
// ���������� ������������ ����� ���������� ������������� ������� � �������, 
// ������� ������������ ��� ��������� � ����� ������� � ���������� �������� � ���.

struct ProcessInfo
{
	PROCESSENTRY32 pe32;
};


//Function declaration
const TCHAR* computerID();
Chrono::time_point timer();
int MonitoringNewProcesses();
void _endApp();

//Variables declaration
std::vector<ProcessInfo> workingProcess{}, SourceProcess{};
std::map<std::wstring, Chrono::time_point> tableTimer;
std::vector<std::string> necessaryProc{ 
	"mspaint.exe",
	"AnyLogic.exe",
	"nCad.exe",
	"nCadM.exe",
	"nCadS.exe",
	"CorelDRW.exe",
	"Surfer.exe",
	"Arduino IDE.exe", 
	"tNavigator_v_20.2.exe", 
	"MATLAB.exe", 
	"Code.exe",
	"MBTY.exe",
	"KOMPASLT.exe",
	"vwware.exe",
	"Zelio2.exe",
	"blender.exe",
	"Ampps.exe",
	"pythonw.exe",
	"SandMan.exe",
	"vncviewer.exe",
	"Office2PDF.exe",
	"statist.exe",
	"PDFXTools.exe",
	"PDFXEdit.exe",
	"VirtualBox.exe",
	"polyspace.exe",
};


//Class declaration
MySQL_API class_DB(3520, timer());


void _endApp()
{
	for (auto _itWorkingProcess = workingProcess.cbegin(); _itWorkingProcess != workingProcess.cend(); ++_itWorkingProcess)
	{
		std::chrono::seconds ResultTime = std::chrono::duration_cast<std::chrono::seconds>(timer() - tableTimer[(std::wstring((*_itWorkingProcess).pe32.szExeFile))]);

		// ������������ ������
		//setup converter
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::string converted_str = converter.to_bytes((*_itWorkingProcess).pe32.szExeFile);

		long _longResultTime = static_cast<long>(ResultTime.count());

		class_DB.set_ProcessInfo_finish(converted_str, _longResultTime);
	}
}


Chrono::time_point timer()
{
	Chrono::time_point startTime = Chrono::now();
	return startTime;
}


int MonitoringNewProcesses()
{
	std::vector<ProcessInfo> currentProcess; // ������� ������
	SmartHandle hProccesSnap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

	// ���������� � ������ ���������
	// TH32CS_SNAPPROCESS - ����, ���������� �� ��, ��� ������ �� ����� ��������.
	// 0 - process ID
	// CreateToolhelp32Snapshot - ��� ������, ������� ������ �����-�� ���������, ����������� ������ �������
	// ���� ���������� � ��������: CreateToolhelp32Snapshot<PROCESSENTRY32>
	// PROCESSENTRY32 - ��� ���������, ����������� ������ �������

	PROCESSENTRY32 pe32{ sizeof(PROCESSENTRY32) };

	// ��������� ������

	if (hProccesSnap == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	if (!Process32First(hProccesSnap, &pe32))
	{
		return 2;
	}

	// 

	if (SourceProcess.empty())
	{
		do
		{
			SourceProcess.push_back(ProcessInfo{ pe32 });
			//tcout << (*(SourceProcess.cend() - 1)).pe32.szExeFile << std::endl;
			//tcout << "Process ID: " << pe32.th32ProcessID << " Parent ID: " << pe32.th32ParentProcessID << " Process Name: " << pe32.szExeFile << std::endl;
		} while (Process32Next(hProccesSnap, &pe32));
	}
	else
	{
		do
		{
			currentProcess.push_back(ProcessInfo{ pe32 });
		} while (Process32Next(hProccesSnap, &pe32));

		// �������� ���������
		// ���� ����� �������
		for (auto _itCurrentProcess = currentProcess.cbegin(); _itCurrentProcess != currentProcess.cend(); ++_itCurrentProcess)
		{
			//��������� ���� �� ���� ������� � ������ ������
			int MatchChecking = 0;
			for (auto _itSourceProcess = SourceProcess.cbegin(); _itSourceProcess != SourceProcess.cend(); ++_itSourceProcess)
			{
				if ((*_itCurrentProcess).pe32.th32ProcessID == (*_itSourceProcess).pe32.th32ProcessID)
				{
					MatchChecking++;
					break;
				}
			}

			// ���� ���������� ���, �� (�� ���� ��� ����� �������)
			if (MatchChecking == 0)
			{
				// ��������, �� �� ����� ���� ������� ��� �������� � ��� ��������
				int CheckingID = 0;
				for (auto _itWorkingProcess = workingProcess.cbegin(); _itWorkingProcess != workingProcess.cend(); ++_itWorkingProcess)
				{
					if (std::wstring((*_itCurrentProcess).pe32.szExeFile) == std::wstring((*_itWorkingProcess).pe32.szExeFile))
					{
						CheckingID++;
						break;
					}
				}
				if (CheckingID == 0)
				{
					// �������, ��������, �� ��, ��� ���� ������ ��� ������� 
					for (auto _itNecessaryProcess = necessaryProc.cbegin(); _itNecessaryProcess != necessaryProc.cend(); ++_itNecessaryProcess)
					{
						std::wstring Unicode_itNecessaryProcess((*_itNecessaryProcess).cbegin(), (*_itNecessaryProcess).cend());
						if ((*_itCurrentProcess).pe32.szExeFile == Unicode_itNecessaryProcess)
						{
							workingProcess.push_back(*_itCurrentProcess);

							// ��������� ����� ������ ������ � ��������
							tableTimer[(std::wstring((*_itCurrentProcess).pe32.szExeFile))] = timer();
							std::string nameProcess = (*_itNecessaryProcess);

							// ������� � ���� ������
							class_DB.set_ProcessInfo_start(nameProcess);

							//tcout << "Name: " << (*_itCurrentProcess).pe32.szExeFile << "\tID: " << (*_itCurrentProcess).pe32.th32ProcessID << "\tParent ID: " << (*_itCurrentProcess).pe32.th32ParentProcessID << std::endl;

							break;
						}
					}
				}
			}
		}
		// ���� ������� �������� 
		for (auto _itSourceProcess = SourceProcess.cbegin(); _itSourceProcess != SourceProcess.cend(); ++_itSourceProcess)
		{
			// ��������� ������� ������ �� ��������
			int MatchChecking = 0;
			for (auto _itCurrentProcess = currentProcess.cbegin(); _itCurrentProcess != currentProcess.cend(); ++_itCurrentProcess)
			{
				if ((*_itCurrentProcess).pe32.th32ProcessID == (*_itSourceProcess).pe32.th32ProcessID)
				{
					MatchChecking++;
					break;
				}
			}
			// ���� ���������� ���, ������ ������� ��������
			if (MatchChecking == 0)
			{
				// ��������� ���� �� � ������� ��������� (�������� �� �������� �� ������)
				for (auto _itWorkingProcess = workingProcess.cbegin(); _itWorkingProcess != workingProcess.cend(); ++_itWorkingProcess)
				{
					// ���� ���� ���������� ��������� ��� � ������� �����
					if ((*_itSourceProcess).pe32.th32ProcessID == (*_itWorkingProcess).pe32.th32ProcessID)
					{
						// ���������� ������ ������� 
						std::chrono::seconds ResultTime = std::chrono::duration_cast<std::chrono::seconds>(timer() - tableTimer[(std::wstring((*_itWorkingProcess).pe32.szExeFile))]);

						// ������������ ������
						//setup converter
						using convert_type = std::codecvt_utf8<wchar_t>;
						std::wstring_convert<convert_type, wchar_t> converter;

						//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
						std::string nameProcess = converter.to_bytes((*_itWorkingProcess).pe32.szExeFile);

						long _longResultTime = static_cast<long>(ResultTime.count());

						class_DB.set_ProcessInfo_finish(nameProcess, _longResultTime);

						// ������� ������� �� �������
						tableTimer.erase((std::wstring((*_itWorkingProcess).pe32.szExeFile)));
						workingProcess.erase(_itWorkingProcess);

						//tcout << "Process was closed.  Name: " << (*_itSourceProcess).pe32.szExeFile
						//	<< "\tID: " << (*_itSourceProcess).pe32.th32ProcessID
						//	<< "\tParent ID: " << (*_itSourceProcess).pe32.th32ParentProcessID
						//	<< "\tWork time: " << ResultTime.count() << std::endl;
						break;
					}
				}
			}
		}
		SourceProcess = currentProcess;
	}

	return 0;
}
#endif // !CHECKPROCESS_H
