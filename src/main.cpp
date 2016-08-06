#include <windows.h>
#include <cstdio>
#include <cstring>
#pragma warning(disable : 4996)

/*
COMP:
0
2,3
B4,B5
*/

HINSTANCE gDllInstance = NULL;

extern "C"
{
	int _entry = 0x00;
	bool _bManualStack = false;
	const int _NOP = 0x51BFB0;
	const int _JMP = 0x51C320;
	const int _JPF = 0x51C340;
	const int _MUSICLOAD = 0x51F580;
	const int _MUSICCHANGE = 0x51F640;
	const int _MOVIE = 0x51F1E0;
	const int _MOVIEREADY = 0x51F110;
	const int _SETBATTLEMUSIC = 0x51F550;
	const int _BATTLE = 0x5230C0;
	const int _MAPJUMPON = 0x00521B00;
	const int _MAPJUMPOFF = 0x00521B10; 
	const int _MAPFADEOFF = 0x00521B20;
	const int _MAPFADEON = 0x00521B30;
	const int _MENUDISABLE = _MAPFADEON + 0x10;
	const int _MENUENABLE = _MENUDISABLE + 0x10;
	const int _MENUNORMAL = _MENUENABLE + 0x10;
	const int _MENUPHS = 0x521B90;
	const int _MENUSHOP = 0x521BB0;
	const int _MENUNAME = 0x521BF0;
	const int _MENUTUTO = 0x521E60;
	const int _UNKNOWN18 = 0x521E80;
	const int _MENUTIPS = 0x521EC0;
	const int _REST = 0x521EF0;
	const int _SETMODEL = 0x51D740;
	int* _ENTRYPOINTER = (int*)0x01D9D040;
	int ENTRY = 0x0188C810;

	//_HACK_BATTLECAMERA_
	int _BattleCamera_WorldX = 0x00B8B7F0;
	int _BattleCamera_WorldZ = 0x00B8B7F2;
	int _BattleCamera_WorldY = 0x00B8B7F4;
	int _BattleCamera_LookAtX = 0x00B8B7F8;
	int _BattleCamera_LookAtZ = 0x00B8B7FA;
	int _BattleCamera_LookAtY = 0x00B8B7FC;

	//Entry Calculator
	const unsigned char musicloadB[] = { 0x8B,0x54,0x24, 0x04, 0xB8, 0x01,0,0,0,0x56 };

	//core functions
	int RecognizeScript(char input[]);

	//script functions
	void Start();
	void Init();
	void SetStackEntry();
	void MANUALSTACK();
	void GetEntities();
	void JMP(int a1);
	void JPF(int a1);
	void MUSICLOAD(int music);
	void MUSICCHANGE();
	void MOVIEREADY(int a1);
	void SETBATTLEMUSIC(int music);
	void BATTLE(int encounter);
	void NoArgumentFunction(int function, char funce[]);
	void OneArgumentFunction(int function, char funce[], unsigned int argument, byte protectionSwitch, char type);
	void SSIGPU();
	void SETMODEL(int a1, int a2);
	int SearchENTRY();

//DEF:
	//signed int(*PlaySystemSound)(unsigned int Sound_ID) = ((signed int(*)(unsigned int))0x0046B270);

    BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
    {
        if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        {
            gDllInstance = hModule;
			Start();
        }
        return TRUE;
    }

#pragma region unused
	/*/
	_declspec(dllexport) void PlayAllSoundTest() //function test
	{
	for (unsigned int i = 1; i != 128u; i++)
	{
	TestSound((unsigned int)i, true);
	Sleep(1000);
	}
	}

	_declspec(dllexport) void PlaySoundTest() //function test
	{
	TestSound((unsigned int)10, false);
	Sleep(1000);
	}

	_declspec(dllexport) void PlayMusicTest() //function test
	{
	TestMusic(38u, 0x7F, 0,0); //41u
	}
	_declspec(dllexport) void PlayMovieTest() //function test
	{
	signed int(*MovieTest)(unsigned __int8 PAKFile, unsigned __int8 MovieID) = ((signed int(*)(unsigned __int8, unsigned __int8))0x00559F70);
	MovieTest(1u, 0u);
	}
	_declspec(dllexport) void _PlayMusic()
	{
	signed int(*MusicPlaying)(char *AKAO) = ((signed int(*)(char *AKAO))0x0046B500);
	char AK[] = "AKAO ";
	AK[4] = 0x3C; //Set number
	for (int i = 0x20; i != 0x64; i++)
	{
	AK[4] = i;
	MusicPlaying(AK);
	Sleep(10000);
	}
	}
	*/
#pragma endregion
#pragma region Core
	//http://stackoverflow.com/questions/5308734/how-can-i-search-for-substring-in-a-buffer-that-contains-null
	char *search_buffer(char *haystack, size_t haystacklen, char *needle, size_t needlelen)
	{
		int searchlen = haystacklen - needlelen + 1;
		for (; searchlen-- > 0; haystack++)
			if (!memcmp(haystack, needle, needlelen))
				return haystack;
		return NULL;
	}

	void SSIGPU()
	{
		signed int(*SSIGPU_Initialize)() = ((signed int(*)())0x0045C320);
		SSIGPU_Initialize();
	}

	void SetStackEntry()
	{
		_ENTRYPOINTER += _entry;
		if (*_ENTRYPOINTER != 00) //DIRECTOR
		{
			ENTRY = *_ENTRYPOINTER;
			return;
		}
		if (*(_ENTRYPOINTER+4) != 00)
		{
			ENTRY = *(_ENTRYPOINTER+4);
			return;
		}
		if (*(_ENTRYPOINTER + 8) != 00)
		{
			ENTRY = *(_ENTRYPOINTER + 8);
			return;
		}
		if (*(_ENTRYPOINTER + 12) != 00)
		{
			ENTRY = *(_ENTRYPOINTER + 0xC);
			return;
		}
		if (*(_ENTRYPOINTER - 4) != 00)
		{
			ENTRY = *(_ENTRYPOINTER - 4);
			return;
		}
		if (*(_ENTRYPOINTER - 8) != 00)
		{
			ENTRY = *(_ENTRYPOINTER - 8);
			return;
		}
		ENTRY = 0x0188C810;
	}

	void MANUALSTACK()
	{
		_bManualStack = true;
		printf("Activated manual stack entity entry resolver!\nUse it only when you know entityID and want to manipulate their stack/script!\nThis function is useful if you want to manipulate specific entity on map, like setmodel or turn...");
		printf("\n\nEntity 0 will again set stack to auto resolve at every script. Entity 1 is first entity on map, usually director");
		printf("\n\n\nType entity ID you want to use (0 for cancel): ");
		int stackID;
		scanf("%d", &stackID);
		if (stackID == 0 || stackID >= 0x0A)
		{
			printf("Typed 0 or bigger than 0A, enabled auto stack resolver!\n");
			_bManualStack = false;
			SetStackEntry();
			return;
		}
		int adde = (stackID - 1) * 4;
		int* entrAdd;
		__asm 
		{
			mov         eax, dword ptr[_ENTRYPOINTER]
			sub         eax, 20h
			push		edx
			mov			edx, [adde]
			add			eax, edx
			pop			edx
			mov         dword ptr[entrAdd], eax
		}
		if (*entrAdd != 00)
			ENTRY = *entrAdd;
		else
		{
			printf("Pointer for choosen entity is equal to 0. You did something wrong...\n");
			printf("Enabling auto stack again and cancelling...\n");
			_bManualStack = false;
			SetStackEntry();
			return;
		}
	}
	void GetEntities()
	{
		int* entryStack = _ENTRYPOINTER - (0x20 / 4);
		printf("\n");
		for (int i = 0; i != 12; i++)
		{
			printf("ENTITY %d: 0x%08x\n", i, *(entryStack + i));
		}
	}


	void Init()
	{
		unsigned char* c = (unsigned char*)_MUSICLOAD;
		int safeHandle = 0;
		for (int i = 0; i != 9; i++)
			if (*c++ == musicloadB[i])
				safeHandle++;
		_entry = safeHandle > 8 ? 0 : SearchENTRY();
		SetStackEntry();
		_BattleCamera_LookAtX += _entry;
		_BattleCamera_LookAtY += _entry;
		_BattleCamera_LookAtZ += _entry;
		_BattleCamera_WorldX += _entry;
		_BattleCamera_WorldZ += _entry;
		_BattleCamera_WorldY += _entry;
	}

	signed int SearchENTRY()
	{
		printf("AdelWhisper automatically located and calculated function locations for your FF8! :)\n");
		size_t musicLoadB_t = sizeof(musicloadB) - 1;
		char* buffer[2048 * 12];
		int* startAddress = (int*)_MUSICLOAD - (2048 * 6);
		memcpy(buffer, startAddress, sizeof(buffer));
		char* c = search_buffer((char*)buffer, sizeof(buffer) - 1, (char*)musicloadB, musicLoadB_t);
		if (c == NULL)
			return 0;
		c -= 2048 * 24;
		return (int)c - (int)buffer;
	}

	void Start()
	{
		AllocConsole();
		freopen("CON", "w", stdout);
		freopen("CON", "r", stdin);
		Init();
		bool bFirstMessage = false;
		while (true)
		{
			if (!bFirstMessage) {
				printf("Engine initialized!");
				bFirstMessage = true;
			}
			char buffer[80];
			printf("> ");
			scanf("%79s", buffer);
			int a = RecognizeScript(buffer);
			if (a == 1) {
				printf("\nUNRECOGNIZED OPERATION!\n");
				Init();
			}
		}
	}
#pragma endregion
#pragma region ScriptRecognizer
	int RecognizeScript(char input[])
	{
		if(!_bManualStack)
			SetStackEntry();
		if (!strcmp(input, "MUSICLOAD"))
		{
			printf("\nMUSICLOAD::MusicID= ");
			int music = 0;
			scanf("%d", &music);
			MUSICLOAD(music);
			return 0;
		}
		if (!strcmp(input, "_MANUALSTACK"))
		{
			MANUALSTACK();
			return 0;
		}
		if (!strcmp(input, "_GETENTITIES"))
		{
			GetEntities();
			return 0;
		}
		if (!strcmp(input, "MUSICCHANGE"))
		{
			printf("MUSICCHANGE();");
			MUSICCHANGE();
			return 0;
		}
		if (!strcmp(input, "JMP"))
		{
			printf("JMP::jumpInt= ");
			int jmpint = 0;
			scanf("%d", &jmpint);
			JMP(jmpint);
			return 0;
		}
		if (!strcmp(input, "NOP"))
		{
			printf("NOP();");
			NoArgumentFunction(_NOP, input);
			return 0;
		}
		if (!strcmp(input, "MOVIE"))
		{
			printf("MOVIE();");
			NoArgumentFunction(_MOVIE, input);
			return 0;
		}
		if (!strcmp(input, "MOVIEREADY"))
		{
			printf("MOVIEREADY::MovieID= ");
			int movie = 0;
			scanf("%d", &movie);
			MOVIEREADY(movie);
			return 0;
		}
		if (!strcmp(input, "SETBATTLEMUSIC"))
		{
			printf("SETBATTLEMUSIC::MusicID= ");
			int music = 0;
			scanf("%d", &music);
			SETBATTLEMUSIC(music);
			return 0;
		}
		if (!strcmp(input, "BATTLE"))
		{
			printf("BATTLE::EncounterID= ");
			int music = 0;
			scanf("%d", &music);
			BATTLE(music);
			return 0;
		}
		if (!strcmp(input, "_SSIGPU"))
		{
			printf("Custom function: SSIGPU();");
			SSIGPU();
			return 0;
		}
		if (!strcmp(input, "MAPJUMPON"))
		{
			printf("MAPJUMPON();");
			NoArgumentFunction(_MAPJUMPON, input);
			return 0;
		}
		if (!strcmp(input, "MAPJUMPOFF"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MAPJUMPOFF, input);
			return 0;
		}
		if (!strcmp(input, "MAPFADEOFF"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MAPFADEOFF, input);
			return 0;
		}
		if (!strcmp(input, "MAPFADEON"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MAPFADEON, input);
			return 0;
		}
		if (!strcmp(input, "MENUDISABLE"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MENUDISABLE, input);
			return 0;
		}
		if (!strcmp(input, "MENUENABLE"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MENUENABLE, input);
			return 0;
		}
		if (!strcmp(input, "MENUNORMAL"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MENUNORMAL, input);
			return 0;
		}
		if (!strcmp(input, "MENUPHS"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MENUPHS, input);
			return 0;
		}
		if (!strcmp(input, "MENUSHOP"))
		{
			printf("%s::ShopID= ", input);
			int argument = 0;
			scanf("%d", &argument);
			OneArgumentFunction(_MENUSHOP, input, argument, 9, 'b');
			return 0;
		}
		if (!strcmp(input, "MENUNAME"))
		{
			printf("%s::nameID= ", input);
			int argument = 0;
			scanf("%d", &argument);
			OneArgumentFunction(_MENUNAME, input, argument, 9, 'u');
			return 0;
		}
		if (!strcmp(input, "MENUTUTO"))
		{
			printf("%s();", input);
			NoArgumentFunction(_MENUTUTO, input);
			return 0;
		}
		if (!strcmp(input, "UNKNOWN18"))
		{
			printf("%s::var= ", input);
			int argument = 0;
			scanf("%d", &argument);
			OneArgumentFunction(_UNKNOWN18, input, argument, 9, 'b');
			return 0;
		}
		if (!strcmp(input, "MENUTIPS"))
		{
			printf("%s::var= ", input);
			int argument = 0;
			scanf("%d", &argument);
			OneArgumentFunction(_MENUTIPS, input, argument, 9, 'i');
			return 0;
		}
		if (!strcmp(input, "REST"))
		{
			printf("%s();", input);
			NoArgumentFunction(_REST, input);
			return 0;
		}
		if (!strcmp(input, "_CALL"))
		{
			printf("Sets EBP to your address, type in hex= ");
			int argument = 0;
			scanf("%x", &argument);
			signed int(*Func)() = ((signed int(*)())(argument));
			Func();
			return 0;
		}
		if (!strcmp(input, "SETMODEL"))
		{
			printf("SETMODEL::");
			int argument = 0;
			scanf("%x", &argument);
			SETMODEL(ENTRY, argument);
			return 0;
		}
		if (!strcmp(input, "_HACK_BATTLECAMERA_WORLDX"))
		{
			short* pointer = (short*)_BattleCamera_WorldX;
			float f = (float)*pointer;
			printf("\nCurrent BattleCamera::World::X=%f", f);
			printf("\nNew BattleCamera::World::X::(float)=");
			float argument = 0;
			scanf("%f", &argument);
			*pointer = (short)argument;
			return 0;
		}
		if (!strcmp(input, "_HACK_BATTLECAMERA_WORLDY"))
		{
			short* pointer = (short*)_BattleCamera_WorldY;
			float f = (float)*pointer;
			printf("\nCurrent BattleCamera::World::Y=%f", f);
			printf("\nNew BattleCamera::World::Y::(float)=");
			float argument = 0;
			scanf("%f", &argument);
			*pointer = (short)argument;
			return 0;
		}
		if (!strcmp(input, "_HACK_BATTLECAMERA_WORLDZ"))
		{
			short* pointer = (short*)_BattleCamera_WorldZ;
			float f = (float)*pointer;
			printf("\nCurrent BattleCamera::World::Z=%f", f);
			printf("\nNew BattleCamera::World::Z::(float)=");
			float argument = 0;
			scanf("%f", &argument);
			*pointer = (short)argument;
			return 0;
		}
		if (!strcmp(input, "_HACK_BATTLECAMERA_LOOKATX"))
		{
			short* pointer = (short*)_BattleCamera_LookAtX;
			float f = (float)*pointer;
			printf("\nCurrent BattleCamera::LookAt::X=%f", f);
			printf("\nNew BattleCamera::LookAt::X::(float)=");
			float argument = 0;
			scanf("%f", &argument);
			*pointer = (short)argument;
			return 0;
		}
		if (!strcmp(input, "_HACK_BATTLECAMERA_LOOKATY"))
		{
			short* pointer = (short*)_BattleCamera_LookAtY;
			float f = (float)*pointer;
			printf("\nCurrent BattleCamera::LookAt::Y=%f", f);
			printf("\nNew BattleCamera::LookAt::Y::(float)=");
			float argument = 0;
			scanf("%f", &argument);
			*pointer = (short)argument;
			return 0;
		}
		if (!strcmp(input, "_HACK_BATTLECAMERA_LOOKATZ"))
		{
			short* pointer = (short*)_BattleCamera_LookAtZ;
			float f = (float)*pointer;
			printf("\nCurrent BattleCamera::LookAt::Z=%f", f);
			printf("\nNew BattleCamera::LookAt::Z::(float)=");
			float argument = 0;
			scanf("%f", &argument);
			*pointer = (short)argument;
			return 0;
		}

		return 1;
	}
#pragma endregion

#pragma region ScriptParser


	void MUSICLOAD(int music)
	{
		byte* b = (byte*)(ENTRY + 0x184); //MOV AL, [EDX+184h] (MOVSX ECX, AL)
		*b = 4;
		byte* musicpo = (byte*)(ENTRY+*b*4);
		*musicpo = (byte)(music & 0xFF);
		byte* protectionswitch = (byte*)(ENTRY + 0x174); 
		*protectionswitch = 1;
		signed int(*MusicLoad)(int a1) = ((signed int(*)(int))(_MUSICLOAD + _entry));
		int a = (MusicLoad(ENTRY)&255);
		*protectionswitch = 0;
		printf("\nMUSICLOAD returned: %d\n", a);
	}

	void MOVIEREADY(int movie)
	{
		byte* b = (byte*)(ENTRY + 0x184); //MOV AL, [EDX+184h] (MOVSX ECX, AL)
		*b = 9;
		byte* moviepo = (byte*)(ENTRY + *b * 4-4); //stack pointer
		*moviepo = (byte)(movie & 0xFF);
		signed int(*MovieReady)(int a1) = ((signed int(*)(int))(_MOVIEREADY + _entry));
		int a = MovieReady(ENTRY);
		printf("\nMOVIEREADY returned: %d\n", a);
	}

	void SETBATTLEMUSIC(int music)
	{
		byte* b = (byte*)(ENTRY + 0x184); //MOV AL, [EDX+184h] (MOVSX ECX, AL)
		*b = 4;
		byte* moviepo = (byte*)(ENTRY + *b * 4); //stack pointer
		*moviepo = (byte)(music & 0xFF);
		signed int(*Func)(int a1) = ((signed int(*)(int))(_SETBATTLEMUSIC + _entry));
		int a = Func(ENTRY);
		printf("\nSETBATTLEMUSIC returned: %d\n", a);
	}

	void MUSICCHANGE()
	{
		signed int(*MusicChange)() = ((signed int(*)())(_MUSICCHANGE + _entry));
		int a = (MusicChange()&0xFF);
		printf("\nMUSICCHANGE returned: %d\n", a);
	}

	void JMP(int a1)
	{
		signed int(*JMP)(unsigned int _a1,signed int jump) = ((signed int(*)(unsigned int,signed int))(_JMP + _entry));
		int result = JMP(ENTRY, a1);
		printf("\nJMP returned: %d\n", result);
	}
	void JPF(int a1)
	{
		signed int(*JPF)(unsigned int _a1,signed int jump) = ((signed int(*)(unsigned int,signed int))(_JPF + _entry));
		int result = JPF(ENTRY, a1);
		printf("\nJPF returned: %d\n", result);
	}
	void BATTLE(int encounter)
	{
		byte* b = (byte*)(ENTRY + 0x184); //MOV AL, [EDX+184h] (MOVSX ECX, AL)
		*b = 9;
		UINT16* moviepo = (UINT16*)(ENTRY + *b * 4 - 4); //stack pointer
		*moviepo = (UINT16)(encounter & 0xFFFF);
		moviepo = (UINT16*)(ENTRY + *b * 4);
		*moviepo = 0x0000;
		signed int(*Func)(int a1) = ((signed int(*)(int))(_BATTLE + _entry));
		int result = Func(ENTRY);
		printf("\nBATTLE returned: %d\n", result);
	}

	void NoArgumentFunction(int function, char funce[])
	{
		signed int(*func)() = ((signed int(*)())(function + _entry));
		int result = func();
		printf("\n%s returned: %d\n", funce,result);
	}
	
	void OneArgumentFunction(int function, char funce[], unsigned int argument, byte protectionSwitch, char type)
	{
		byte* b = (byte*)(ENTRY + 0x184);
		*b = protectionSwitch;
		if (type == 'b') //Byte unsigned
		{
			byte* stack = (byte*)(ENTRY + *b * 4);
			*stack = argument && 0xFF;
			signed int(*Func)(int a1) = ((signed int(*)(int))(function + _entry));
			int a2 = Func(ENTRY);
			printf("\n%s returned: %d\n", funce, a2);
			return;
		}
		if (type == 'i') //int signed
		{
			signed int* stack = (int*)(ENTRY + *b * 4);
			*stack = (signed int)argument;
			signed int(*Func)(int a1) = ((signed int(*)(int))(function + _entry));
			int a2 = Func(ENTRY);
			printf("\n%s returned: %d\n", funce, a2);
			return;
		}
		if (type == 's') //int16 signed
		{
			short* stack = (short*)(ENTRY + *b * 4);
			*stack = (short)(argument && 0xFFFF);
			signed int(*Func)(int a1) = ((signed int(*)(int))(function + _entry));
			int a2 = Func(ENTRY);
			printf("\n%s returned: %d\n", funce, a2);
			return;
		}
		if (type == 'u') //int unsigned
		{
			unsigned int* stack = (unsigned int*)(ENTRY + *b * 4);
			*stack = argument;
			signed int(*Func)(int a1) = ((signed int(*)(int))(function + _entry));
			int a2 = Func(ENTRY);
			printf("\n%s returned: %d\n", funce, a2);
			return;
		}
		if (type == '6') //uint16
		{
			UINT16* stack = (UINT16*)(ENTRY + *b * 4);
			*stack = (UINT16)(argument && 0xFFFF);
			signed int(*Func)(int a1) = ((signed int(*)(int))(function + _entry));
			int a2 = Func(ENTRY);
			printf("\n%s returned: %d\n", funce, a2);
			return;
		}
		if (type == 'c') //char
		{
			char* stack = (char*)(ENTRY + *b * 4);
			*stack = (char)(argument && 0xFF);
			signed int(*Func)(int a1) = ((signed int(*)(int))(function + _entry));
			int a2 = Func(ENTRY);
			printf("\n%s returned: %d\n", funce, a2);
			return;
		}
	}
	void SETMODEL(int a1, int a2)
	{
		UINT16 argument = (UINT16)(a2 && 0xFFFF);
		signed int(*Func)(int stack, UINT16 argument) = ((signed int(*)(int, UINT16))(_SETMODEL + _entry));
		int result = Func(a1, argument);
		printf("\nSETMODEL returned: %d\n", result);
		return;
	}
#pragma endregion
}
