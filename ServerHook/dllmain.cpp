// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "detours.h"

#define DEBUG
// #define SERVER

#include "CLimitSingleInstance.cpp"

// #ifdef SERVER
#include "MinHook.h"
#else
#include "detours.h"
#endif
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include "packets.h"
#include <string>
#include <sstream>
#include <chrono>
#include <future>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

FILE *fp;

typedef void (*_XFatalError_t)(const char *format, ...);
_XFatalError_t fatalError = _XFatalError_t(0x7E6DF0);

using KillGame_t = int (*)();
KillGame_t killGame = KillGame_t(0x7B4770);

typedef BOOL(__thiscall *ParsingPacketClient_t)(void *ecx, uint8_t *pPacket);
typedef BOOL(__thiscall *SendPacketEncapsulateClient_t)(void *ecx, uint8_t *pPacket);
typedef void(__thiscall *SetDelayTimeClient_t)(void *ecx, FLOAT time);
typedef UINT(__thiscall *RegReadIntClient_t)(void *ecx, HKEY hkey, const char *lpKey, const char *lpValue, INT nDefault, BOOL defaultset);

ParsingPacketClient_t ParsingPacketClient = ParsingPacketClient_t(0x6C16D0);
SendPacketEncapsulateClient_t SendPacketEncapsulateClient = SendPacketEncapsulateClient_t(0x6CA1A0);
SetDelayTimeClient_t SetDelayTimeClient = SetDelayTimeClient_t(0x4B8440);
RegReadIntClient_t RegReadIntClient = RegReadIntClient_t(0x7DFFB0);

void PatchBytes(void *address, const uint8_t *bytes, const size_t length)
{
	DWORD tmp;
	VirtualProtectEx(GetCurrentProcess(), address, length, PAGE_EXECUTE_READWRITE, &tmp);
	auto *addr = static_cast<uint8_t *>(address);
	for (size_t i = 0; i < length; i++)
	{
		addr[i] = bytes[i];
	}
	VirtualProtectEx(GetCurrentProcess(), address, length, tmp, nullptr);
}

std::string getPacket(uint8_t *data, size_t size = -1)
{
	if (size == -1)
	{
		size = data[0] | ((data[1] & 0xF) << 8);
	}
	std::stringstream ss;
	ss << std::hex;
	for (size_t i = 0; i < size; i++)
	{
		ss << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
	}
	return ss.str();
}

#ifdef SERVER
struct _point
{
	float m_posX;
	float m_posZ;

	_point() : m_posX(0.0f), m_posZ(0.0f)
	{
	}

	_point(float x, float z) : m_posX(x), m_posZ(z)
	{
	}

	void operator=(const _point &p)
	{
		m_posX = p.m_posX;
		m_posZ = p.m_posZ;
	}
};

typedef void (*PrintConsole_t)(const char *format, ...);
typedef BOOL(__thiscall *sendreq_t)(void *, uint8_t *pPacket, void *pSize);
typedef void(__thiscall *SendData_t)(void *ecx, uint8_t *buf, int length, void *pos, void *pRoom);
typedef int (*ProcessCommand_t)(char *cmd);
typedef void(__thiscall *addpack_t)(void *ecx, uint8_t *buf, int length);
typedef void(__thiscall *AddEffectEntry_t)(void *ecx, uint16_t effID, uint32_t effTime, void *param3);
typedef void *(__thiscall *CreateAndSpawn_t)(void *ecx, uint16_t mobID, _point *point);
typedef void *(*GetDNPCPool_t)();
typedef float(__thiscall *GetUserX_t)(void *ecx);
typedef float(__thiscall *GetUserZ_t)(void *ecx);
typedef int(__cdecl *GS_Main_t)(int argc, char **argv);
typedef int(__cdecl *RandomShitToHook_t)();
typedef int(__thiscall *PrintCombatInfo_t)(void *ecx);

PrintConsole_t PrintConsole = (PrintConsole_t)0x006A01DF;
PrintConsole_t fpPrintConsole = static_cast<PrintConsole_t>(nullptr);
sendreq_t sendReq = (sendreq_t)0x69479A;
sendreq_t fpSendReq = static_cast<sendreq_t>(nullptr);
SendData_t send_data = (SendData_t)0x005CCB5F;
SendData_t fpSend_data = static_cast<SendData_t>(nullptr);
ProcessCommand_t ProcessCommand = (ProcessCommand_t)0x006B3A49;
ProcessCommand_t fpProcessCommand = static_cast<ProcessCommand_t>(nullptr);
addpack_t addpack = (addpack_t)0x0042FAB0;
addpack_t fpAddpack = nullptr;
AddEffectEntry_t AddEffectEntry = (AddEffectEntry_t)0x004ED611;
AddEffectEntry_t fpAddEffectEntry = nullptr;
CreateAndSpawn_t createAndSpawn = (CreateAndSpawn_t)0x005BB88F;
GetDNPCPool_t GetDNPCPool = (GetDNPCPool_t)0x00406B10;
GetUserX_t GetUserX = (GetUserX_t)0x00431F70;
GetUserZ_t GetUserZ = (GetUserZ_t)0x00431F90;
GS_Main_t __main__ = GS_Main_t(0x6AEF80);
GS_Main_t fpMain = GS_Main_t(nullptr);
RandomShitToHook_t r = RandomShitToHook_t(0x41C820);
RandomShitToHook_t fpR = RandomShitToHook_t(nullptr);
PrintCombatInfo_t print_combat_info = PrintCombatInfo_t(0x4A2F6E);
PrintCombatInfo_t fp_print_combat_info = PrintCombatInfo_t(nullptr);

int HookProcessCommand(char *cmd)
{
	// PrintConsole("LMAO, addresses: orig 0x%p, fp 0x%p\n", ProcessCommand, fpProcessCommand);
	char command[128] = {0};
	uint16_t mobID = 0;
	uint16_t mobCount = 1;
	float x = 0.0f;
	float z = 0.0f;
	int count = sscanf_s(cmd, "%s %hu (%f,%f) %hu", command, 128, &mobID, &x, &z, &mobCount);
	if (strcmp(command, "spawn") == 0)
	{
		if (count < 4)
		{
			PrintConsole("Syntax: spawn mobID (x,z) [mobCount]\n");
			return 0;
		}
		if (mobCount <= 0)
		{
			PrintConsole("Mob count must be higher than 0\n");
			return 0;
		}
		// PrintConsole("Attempting this crap");
		PrintConsole("Command %s, pos (x: %.2f, z: %.2f), mob ID: %hu, mob count: %hhu\n", command, x, z, mobID,
					 mobCount);
		if (mobID < 0)
		{
			PrintConsole("Invalid mob ID\n");
		}
		if (mobID < 4000)
		{
			PrintConsole("Mob ID too low - received %hu, assumed: %hu\n", mobID, mobID + 4000);
			mobID += 4000;
		}
		while (mobCount--)
		{
			createAndSpawn(GetDNPCPool(), mobID, new _point(x, z));
		}
		return 0;
	}
	PrintConsole("Different command: %s", cmd);
	return fpProcessCommand(cmd);
}

bool shouldLogPacket()
{
#ifdef DEBUG
	return true;
#else
	return false;
#endif
}

void __fastcall HookAddEffectEntry(void *ecx, void *edx, uint16_t effID, uint32_t effTime, void *param3)
{
	if (effID == 504 && effTime >= 30)
	{
		// map impervious
		PrintConsole("AddEffectEntry: ID %hu, time %d, param3: 0x%08X", effID, effTime, (uint32_t)param3);
		effTime = 30;
		return;
	}
	fpAddEffectEntry(ecx, effID, effTime, param3);
}

std::string GM_NAMES[] = {"Shuji", "Bogdy", "Kinga", "ZeroN"};

void __fastcall HookAddPack(void *ecx, void *edx, uint8_t *pPacket, int size)
{
	if (pPacket[2] == 0x1B)
	{
		// pPacket[0] += 12;
		//         PrintConsole("[Add Packet] : %s\n", getPacket(pPacket, (size_t)size).c_str());
		CharacInfo *characInfo = (CharacInfo *)pPacket;

		for (const auto &gm : GM_NAMES)
		{
			if (strcmp((const char *)characInfo->name, gm.c_str()) == 0)
			{
				pPacket[0x1A] |= 8;
				pPacket[0x57] = 0xe8;
				pPacket[0x59] = 0xf0;
			}
			fpAddpack(ecx, (uint8_t *)characInfo, size);
		}
	}
	else
		fpAddpack(ecx, pPacket, size);
}

BOOL __fastcall SendRequest(void *ecx, void *edx, uint8_t *pPacket, void *pSize)
{
	if (shouldLogPacket())
	{
		PrintConsole("Sent data (sendreq): %s\n", getPacket(pPacket, (size_t)pSize).c_str());
	}

	// if (pPacket[2] == 0x7A) {
	// 	PrintConsole("Illusive Branch test\n");
	// 	//todo: check potion type
	// 	float x = GetUserX(ecx);
	// 	float z = GetUserZ(ecx);
	// 	uint16_t mobID = 4001 + rand() % 10;
	// 	createAndSpawn(GetDNPCPool(), mobID, new _point(x, z));
	// }

	if (pPacket[2] == 16)
	{
	}

	if (pPacket[2] == 0x09)
	{
		uint8_t *pinConfirmation = new uint8_t[4]{0x04, 0x00, 0x07, 0x15};

		//    MSG_CHARAC_LIST* list = (MSG_CHARAC_LIST*)pPacket;

		//    MSG_CHARAC_LIST_REF * refList = new MSG_CHARAC_LIST_REF;

		//    refList->header = list->header;
		//    refList->header.sLength = 287;
		//    refList->nCharac = list->nCharac;
		//    refList->ucMessage = 0x09;
		//    refList->uiStateFlag = list->uiStateFlag;
		//    DWORD addr1 = reinterpret_cast<DWORD>(&list->List[0]);
		//    DWORD addr2 = reinterpret_cast<DWORD>(&list->List[1]);
		//    DWORD addr3 = reinterpret_cast<DWORD>(&list->List[2]);

		//    DWORD refAddr1 = reinterpret_cast<DWORD>(&refList->List[0]);
		//    DWORD refAddr2 = reinterpret_cast<DWORD>(&refList->List[1]);
		//    DWORD refAddr3 = reinterpret_cast<DWORD>(&refList->List[2]);
		//    memcpy((void*)refAddr1, (void*)addr1, 60);
		//    memcpy((void*)refAddr2, (void*)addr2, 60);
		//    memcpy((void*)refAddr3, (void*)addr3, 60);

		//    memcpy((void*)(refAddr1 + 65), (void*)(addr1 + 60), 28);
		//    memcpy((void*)(refAddr2 + 65), (void*)(addr2 + 60), 28);
		//    memcpy((void*)(refAddr3 + 65), (void*)(addr3 + 60), 28);

		//    refList->List[0].hatRefine = 7;
		//    refList->List[0].glovesRefine = 7;
		//    refList->List[0].pantsRefine = 7;
		//    refList->List[0].jacketRefine = 7;
		//    refList->List[0].bootsRefine = 7;

		//    refList->List[1].hatRefine = 7;
		//    refList->List[1].glovesRefine = 7;
		//    refList->List[1].pantsRefine = 7;
		//    refList->List[1].jacketRefine = 7;
		//    refList->List[1].bootsRefine = 7;

		//    refList->List[2].hatRefine = 7;
		//    refList->List[2].glovesRefine = 7;
		//    refList->List[2].pantsRefine = 7;
		//    refList->List[2].jacketRefine = 7;
		//    refList->List[2].bootsRefine = 7;
		//    printf((char*)getPacket((uint8_t*)refList, 287).c_str());

		//       fpSendReq(ecx, (uint8_t*)refList, (void*)refList->header.sLength);

		fpSendReq(ecx, pPacket, pSize);
		return fpSendReq(ecx, pinConfirmation, (void *)4);
	}

	if (pPacket[2] == 27)
	{
		unsigned char *ggauth = new unsigned char[0x51]{
			0x51, 0x00, 0x9e, 0x4c, 0x00, 0x0c, 0x47, 0x29, 0x91, 0x27, 0x8e, 0x52, 0x22, 0x36, 0xd5, 0x78, 0xb3, 0x48,
			0x1c, 0xa1, 0x44, 0xd1, 0x0e, 0x45, 0x22, 0x24, 0x06, 0x81, 0xa3, 0x0f, 0x75, 0x6c, 0x0f, 0xe4, 0x55, 0x6d,
			0x59, 0x28, 0x4d, 0x4d, 0x3b, 0xfa, 0xcb, 0xe7, 0x9d, 0xb6, 0x12, 0x66, 0x3b, 0xba, 0x3d, 0xe7, 0x99, 0xa9,
			0xea, 0x45, 0xa0, 0x94, 0x09, 0x27, 0x33, 0x55, 0x91, 0x81, 0xb1, 0xfd, 0xd1, 0x5d, 0xd3, 0x43, 0x4e, 0x3f,
			0xec, 0xf4, 0x23, 0xf3, 0x46, 0xc7, 0xf8, 0x47, 0x88};

		return fpSendReq(ecx, static_cast<uint8_t *>(ggauth), (void *)0x51);
	}

	return fpSendReq(ecx, pPacket, pSize);
}

void EnableDebugs()
{
	uint8_t *firstAddr = (uint8_t *)0x00776344;
	uint8_t *lastAddr = (uint8_t *)0x00776350;
	while (firstAddr <= lastAddr)
	{
		*firstAddr = 1;
		firstAddr = (uint8_t *)((DWORD)firstAddr + 1);
	}
	PrintConsole("Enabled debugs!\n");
}

void __fastcall HookSendData(void *ecx, void *edx, uint8_t *buf, int length, void *pos, void *pRoom)
{
	if (shouldLogPacket())
	{
		PrintConsole("Sent data (send_data): %s\n", getPacket(buf, length).c_str());
	}
	fpSend_data(ecx, buf, length, pos, pRoom);
}

void __fastcall HookPrintCombatInfo(void *ecx, void *)
{
	if (fp_print_combat_info != nullptr)
	{
		fp_print_combat_info(ecx);
	}
	// print custom stuff
	printf("Shuji's hook\n");
	void *skill_atk = nullptr;
	void *spell_atk = nullptr;
}

void HookPrintConsole(const char *format, ...)
{
	fpPrintConsole("Debugging PrintConsole hooking\n");
	fpPrintConsole(format);
}

bool InitMHHooks()
{
	MH_STATUS st;
	if ((st = MH_Initialize()) != MH_OK)
	{
		PrintConsole("Error: %s", MH_StatusToString(st));
		return false;
	}

	/*if ((st = MH_CreateHook(PrintConsole, &HookPrintConsole, reinterpret_cast<LPVOID*>(&fpPrintConsole))) != MH_OK) {
		PrintConsole("Failed to hook PrintConsole! Reason: %s", MH_StatusToString(st));
		return false;
	}*/

	if ((st = MH_CreateHook(sendReq, &SendRequest, reinterpret_cast<LPVOID *>(&fpSendReq))) != MH_OK)
	{
		PrintConsole("Failed to hook sendReq! Reason: %s", MH_StatusToString(st));
		return false;
	}

	if ((st = MH_CreateHook(send_data, &HookSendData, reinterpret_cast<LPVOID *>(&fpSend_data))) != MH_OK)
	{
		PrintConsole("Failed to hook send_data! Reason: %s", MH_StatusToString(st));
		return false;
	}

	/*if ((st = MH_EnableHook(PrintConsole)) != MH_OK) {
		PrintConsole("Failed to enable a PrintConsole hook, reason: %s", MH_StatusToString(st));
		return false;
	}*/

	if ((st = MH_EnableHook(sendReq)) != MH_OK)
	{
		PrintConsole("Failed to enable a sendReq hook, reason: %s", MH_StatusToString(st));
		return false;
	}

	if ((st = MH_EnableHook(send_data)) != MH_OK)
	{
		PrintConsole("Failed to enable a send_data hook, reason: %s", MH_StatusToString(st));
		return false;
	}

	if ((st = MH_CreateHook(ProcessCommand, &HookProcessCommand, reinterpret_cast<LPVOID *>(&fpProcessCommand))) != MH_OK)
	{
		PrintConsole("Failed to hook ProcessCommand! Reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully created ProcessCommand hook\n");

	if ((st = MH_EnableHook(ProcessCommand)) != MH_OK)
	{
		PrintConsole("Failed to enable a ProcessCommand hook, reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully enabled ProcessCommand hook\n");

	// HOOK ADDPACK

	if ((st = MH_CreateHook(addpack, &HookAddPack, reinterpret_cast<LPVOID *>(&fpAddpack))) != MH_OK)
	{
		PrintConsole("Failed to hook AddPack! Reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully created AddPack hook\n");

	if ((st = MH_EnableHook(addpack)) != MH_OK)
	{
		PrintConsole("Failed to enable a AddPack hook, reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully enabled AddPack hook\n");

	// HOOK ADD EFFECT (imperv fix)

	if ((st = MH_CreateHook(AddEffectEntry, &HookAddEffectEntry, reinterpret_cast<LPVOID *>(&fpAddEffectEntry))) != MH_OK)
	{
		PrintConsole("Failed to hook AddEffectEntry! Reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully created AddEffectEntry hook\n");

	if ((st = MH_EnableHook(AddEffectEntry)) != MH_OK)
	{
		PrintConsole("Failed to enable a AddEffectEntry hook, reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully enabled AddEffectEntry hook\n");

#ifndef DEBUG
	if ((st = MH_CreateHook(print_combat_info, &HookPrintCombatInfo, reinterpret_cast<LPVOID *>(&fp_print_combat_info))) != MH_OK)
	{
		PrintConsole("Failed to hook PrintCombatInfo! Reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully created PrintCombatInfo hook\n");

	if ((st = MH_EnableHook(print_combat_info)) != MH_OK)
	{
		PrintConsole("Failed to enable a PrintCombatInfo hook, reason: %s\n", MH_StatusToString(st));
		return false;
	}
	PrintConsole("Successfully enabled PrintCombatInfo hook\n");
#endif

	// Don't break a weapon
	PatchBytes(reinterpret_cast<void *>(0x00522810),
			   new uint8_t[9]{0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 9);
	// Don't use a scale if XRand < break_chance
	PatchBytes(reinterpret_cast<void *>(0x0052267A), new uint8_t[6]{0xE9, 0x10, 0x01, 0x00, 0x00, 0x90}, 6);
	PrintConsole("Hooking weapon breaking\n");

	return true;
}

void InitHooks()
{
	MessageBoxW(nullptr, (LPCWSTR) "", (LPCWSTR) "", MB_OK);
	if (!InitMHHooks())
	{
		PrintConsole("Hooking failed!");
	}
	else
	{
		PrintConsole("Initialized hooks!");
	}
}

bool isInjected = false;
int __cdecl HookForInitialization()
{
	if (!isInjected)
	{
		PrintConsole("Hooks not injected yet, hooking...\n");
		isInjected = true;
		InitHooks();
	}
	if (fpR != nullptr)
	{
		return fpR();
	}
	else if (r != nullptr)
	{
		return r();
	}
	return 0;
}

void InitServerHooks()
{
	MH_STATUS st;
	if ((st = MH_Initialize()) != MH_OK)
	{
		PrintConsole("Error: %s", MH_StatusToString(st));
		return;
	}

	if ((st = MH_CreateHook(r, &HookForInitialization, reinterpret_cast<LPVOID *>(&fpR))) != MH_OK)
	{
		PrintConsole("Failed to hook server hooks! Reason: %s", MH_StatusToString(st));
		return;
	}
	else
	{
		PrintConsole("Hooked server stuff");
	}

	if ((st = MH_EnableHook(r)) != MH_OK)
	{
		PrintConsole("Failed to enable server hooks, reason: %s", MH_StatusToString(st));
		return;
	}
	else
	{
		PrintConsole("Enabled server hook");
	}
}

#else

bool isKilledAlready = false;
bool isTheOneToStartFirst = false;
void checkIfMutex()
{
	// Declare some variables
	WSADATA wsaData;

	int iResult = 0; // used to return function results

	// the listening socket to be created
	SOCKET ListenSocket = INVALID_SOCKET;

	// The socket address to be passed to bind
	sockaddr_in service;

	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27015);

	//----------------------
	// Bind the socket.
	iResult = bind(ListenSocket, (SOCKADDR *)&service, sizeof(service));
	if (iResult == SOCKET_ERROR)
	{
		if (!isKilledAlready)
		{
			fatalError("Game is already running");
			isKilledAlready = true;
		}
	}
	else
	{
		isTheOneToStartFirst = true;
	}
}

UINT __fastcall HookRegReadInt(void *ecx, void *, HKEY hkey, const char *lpKey, const char *lpValue, INT nDefault, BOOL defaultset)
{
	if (strcmp(lpValue, "loadseparatedscript") == 0)
	{
		return 0;
	}
	return RegReadIntClient(ecx, hkey, lpKey, lpValue, nDefault, defaultset);
}

void __fastcall HookSetDelay(void *ptr, void *, FLOAT time)
{
	if (time == 12000.0f)
	{
		time = 3000.0f;
	}
	SetDelayTimeClient(ptr, time);
}

BOOL __fastcall HookSendPacketEncapsulate(void *ptr, void *, uint8_t *packet)
{
#ifdef DEBUG
	std::cout << "[C->S]" << getPacket(packet).c_str() << std::endl;
#endif
	if (packet[2] == 0x7C) // refinement
	{
		const auto size = packet[0];
		packet[0] -= 4;
		std::vector<uint8_t> v(packet, packet + size);
		// v.insert(v.begin() + 4, { 0,0,0,0 });
		v.erase(v.begin() + 4, v.begin() + 8); // 09 00 7C 01 xx xx xx xx 01 -> 05 00 7C 01 01
		return SendPacketEncapsulateClient(ptr, static_cast<uint8_t *>(&v[0]));
	}

	return SendPacketEncapsulateClient(ptr, packet);
}

void FixLOD()
{
	const auto defaultValue = 300.0f;
	auto *lodFarPlane = reinterpret_cast<float *>(0x1574F68);
	auto *defaultLod1 = reinterpret_cast<float *>(0x1574F5C);
	auto *defaultLod2 = reinterpret_cast<float *>(0x1574F60);
	auto *defaultLod3 = reinterpret_cast<float *>(0x1574F64);

	auto *actualLod1 = reinterpret_cast<float *>(0x1574F50);
	auto *actualLod2 = reinterpret_cast<float *>(0x1574F54);
	auto *actualLod3 = reinterpret_cast<float *>(0x1574F58);

	*defaultLod1 = *defaultLod2 = *defaultLod3 = defaultValue;
	*lodFarPlane = *actualLod1 = *actualLod2 = *actualLod3 = defaultValue + 50.0f;
}

BOOL __fastcall HookParsingPacket(void *ptr, void *, uint8_t *packet)
{

	short *pDeco = reinterpret_cast<short *>(packet + 85);
	if (*pDeco == 14213)
	{
		packet[0x57] = 0xFE;
		packet[0x58] = 0x0B;
	}

	// if (packet[2] == 0x46 && packet[3] == 0x0A)
// {
// 	auto* hdr = reinterpret_cast<_sHeader*>(packet);
// 	auto* newPacket = new uint8_t[22];
//
// 	// fprintf(fp, (char*)getPacket(packet, (size_t)hdr->sLength).c_str());
// 	// fprintf(fp, "\n");
//
// 	memcpy(newPacket, packet, 20);
// 	newPacket[0] = 22;
// 	newPacket[21] = packet[19];
// 	newPacket[19] = 0;
// 	newPacket[20] = 0;
//
// 	return ParsingPacketClient(ptr, newPacket);
// }
#ifdef DEBUG
	std::cout << "[S->C]" << getPacket(packet).c_str() << std::endl;
#endif
	if (packet[2] == 0x7C) // refinement
	{
		const auto size = packet[0];
		packet[0] += 4;
		std::vector<uint8_t> v(packet, packet + size);
		v.insert(v.begin() + 4, {0, 0, 0, 0});
		return ParsingPacketClient(ptr, static_cast<uint8_t *>(&v[0]));
	}

	// if (packet[2] == 24)
	if (packet[2] == 27)
	{
		FixLOD();
#ifdef DEBUG
		// std::cout << getPacket(packet).c_str() << std::endl;
#else
		if (!isTheOneToStartFirst)
		{
			checkIfMutex();
		}
#endif
		auto *ggauth = new uint8_t[0x51]{0x51, 0x00, 0x9e, 0x4c, 0x00, 0x0c, 0x47, 0x29, 0x91, 0x27, 0x8e, 0x52, 0x22, 0x36, 0xd5, 0x78, 0xb3, 0x48, 0x1c, 0xa1, 0x44, 0xd1, 0x0e, 0x45, 0x22, 0x24, 0x06, 0x81, 0xa3, 0x0f, 0x75, 0x6c, 0x0f, 0xe4, 0x55, 0x6d, 0x59, 0x28, 0x4d, 0x4d, 0x3b, 0xfa, 0xcb, 0xe7, 0x9d, 0xb6, 0x12, 0x66, 0x3b, 0xba, 0x3d, 0xe7, 0x99, 0xa9, 0xea, 0x45, 0xa0, 0x94, 0x09, 0x27, 0x33, 0x55, 0x91, 0x81, 0xb1, 0xfd, 0xd1, 0x5d, 0xd3, 0x43, 0x4e, 0x3f, 0xec, 0xf4, 0x23, 0xf3, 0x46, 0xc7, 0xf8, 0x47, 0x88};

		ParsingPacketClient(ptr, ggauth);
	}

	return ParsingPacketClient(ptr, packet);
}

void InitClientHooks()
{
#ifdef DEBUG
	AllocConsole();
	SetConsoleTitle(L"[debug] HOOK_9D");
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
#endif
	// fp = fopen("E:\\9DV215\\9Dragons2\\log.txt", "w");
	// fprintf(fp, "Injected");
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// this will hook the function
	DetourAttach(reinterpret_cast<PVOID *>(&ParsingPacketClient), &HookParsingPacket);
	DetourAttach(reinterpret_cast<PVOID *>(&SendPacketEncapsulateClient), &HookSendPacketEncapsulate);
	DetourAttach(reinterpret_cast<PVOID *>(&SetDelayTimeClient), &HookSetDelay);
	DetourAttach(reinterpret_cast<PVOID *>(&RegReadIntClient), &HookRegReadInt);
	// Allow slotting refined weapons
	PatchBytes(reinterpret_cast<void *>(0x005239A4), new uint8_t[1]{0xEB}, 1); // DONE
	// Allow slotting clothes
	PatchBytes(reinterpret_cast<void *>(0x00523D2C), new uint8_t[6]{0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 6); // DONE
	// Don't delete a weapon if refinement finished with result = 2
	PatchBytes(reinterpret_cast<void *>(0x00702263), new uint8_t[3]{0x90, 0x90, 0x90}, 3);		 // DONE
	PatchBytes(reinterpret_cast<void *>(0x007021FC), new uint8_t[4]{0x58, 0x1B, 0x00, 0x00}, 4); // patch XSD index 7000 DONE

	// 3 characters on keyboard
	PatchBytes(reinterpret_cast<void *>(0x00410CA1), new uint8_t[3]{0xE9, 0xBE, 0x00}, 3);
	PatchBytes(reinterpret_cast<void *>(0x00410D1C), new uint8_t[1]{0xEB}, 1);
	PatchBytes(reinterpret_cast<void *>(0x0042765F), new uint8_t[1]{0xEB}, 1);

	// Fix bone count issue
	PatchBytes(reinterpret_cast<void *>(0x00728865), new uint8_t[6]{0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 6);
	PatchBytes(reinterpret_cast<void *>(0x00727DF6), new uint8_t[6]{0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 6);
	PatchBytes(reinterpret_cast<void *>(0x007273A7), new uint8_t[6]{0x90, 0x90, 0x90, 0x90, 0x90, 0x90}, 6);

	// Model fix
	//  PatchBytes(reinterpret_cast<void*>(0x00737C80), new uint8_t[2]{ 0xE8, 0x03 }, 2);
	//  PatchBytes(reinterpret_cast<void*>(0x00737CA3), new uint8_t[4]{ 0xF0, 0x2A, 0x62, 0x01 }, 4);
	//  PatchBytes(reinterpret_cast<void*>(0x005BF323), new uint8_t[4]{ 0xF0, 0x2A, 0x62, 0x01 }, 4);
	//  PatchBytes(reinterpret_cast<void*>(0x00409385), new uint8_t[4]{ 0xF0, 0x2A, 0x62, 0x01 }, 4);
	//  PatchBytes(reinterpret_cast<void*>(0x00406DDC), new uint8_t[4]{ 0xF0, 0x2A, 0x62, 0x01 }, 4);
#ifdef DEBUG
#endif

	// LOD fix
	FixLOD();
	DetourTransactionCommit();
}
#endif

std::string GetLineFromCin()
{
	std::string line;
	std::getline(std::cin, line);
	return line;
}

void FreezeFlags()
{
	auto *camera_shake_flag = reinterpret_cast<BOOL *>(0x15750D4);
	*camera_shake_flag = FALSE;
}

int MainThread(HMODULE hModule)
{
#ifdef SERVER
	InitHooks();
	// InitServerHooks();
#ifdef DEBUG
	EnableDebugs();
#endif

#else
	InitClientHooks();
#endif
#ifdef DEBUG
	auto future = std::async(std::launch::async, GetLineFromCin);
#endif
	while (true)
	{
#ifdef DEBUG
		if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			auto line = future.get();

			// Set a new line. Subtle race condition between the previous line
			// and this. Some lines could be missed. To aleviate, you need an
			// io-only thread. I'll give an example of that as well.
			future = std::async(std::launch::async, GetLineFromCin);

			std::cout << "you wrote " << line << std::endl;

			// std::string test = "01A1"; // assuming this is an even length string
			uint8_t *bytes = new uint8_t[line.length() / 2];
			std::stringstream converter;
			for (auto i = 0; i < line.length(); i += 2)
			{
				converter << std::hex << line.substr(i, 2);
				int byte;
				converter >> byte;
				bytes[i / 2] = static_cast<uint8_t>(byte & 0xFF);
				converter.str(std::string());
				converter.clear();
			}
			SendPacketEncapsulateClient(reinterpret_cast<void *>(0x1575198), bytes);
		}
		// Sleep(1000 / 60);
#endif
		FreezeFlags();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

extern "C" void __declspec(dllexport) svhook()
{
	// fp = fopen("C:\\9DV215\\9Dragons2\\log.txt", "w");
	// fprintf(fp, "Injected");
}

BOOL APIENTRY DllMain(HMODULE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&MainThread), hModule, NULL, nullptr);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}