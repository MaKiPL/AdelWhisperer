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
	const int _NOP = 0x51BFB0;
	const int _JMP = 0x51C320;
	const int _JPF = 0x51C340;
	const int _MUSICLOAD = 0x51F580;
	const int _MUSICCHANGE = 0x51F640;
	const int _MOVIE = 0x51F1E0;
	const int _MOVIEREADY = 0x51F110;
	const int* _ENTRYPOINTER = (int*)0x01D9D028;
	int ENTRY = 0x0188C810;

	//Entry Calculator
	const unsigned char musicloadB[] = { 0x8B,0x54,0x24, 0x04, 0xB8, 0x01,0,0,0,0x56 };

	//core functions
	int RecognizeScript(char input[]);

	//script functions
	void Start();
	void Init();
	void SetStackEntry();
	void JMP(int a1);
	void JPF(int a1);
	void MUSICLOAD(int music);
	void MUSICCHANGE();
	void MOVIEREADY(int a1);
	void NoArgumentFunction(int function, char funce[]);
	void SSIGPU();
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
		if (*_ENTRYPOINTER != 00)
		{
			ENTRY = *_ENTRYPOINTER;
			return;
		}
		if (*_ENTRYPOINTER+4 != 00)
		{
			ENTRY = *_ENTRYPOINTER;
			return;
		}
		if (*_ENTRYPOINTER+8 != 00)
		{
			ENTRY = *_ENTRYPOINTER;
			return;
		}
		ENTRY = 0x0188C810;
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
		ENTRY += _entry;
	}

	signed int SearchENTRY()
	{
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
		SetStackEntry();
		if (!strcmp(input, "MUSICLOAD"))
		{
			printf("\nMUSICLOAD::MusicID= ");
			int music = 0;
			scanf("%d", &music);
			MUSICLOAD(music);
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
		if (!strcmp(input, "SSIGPU"))
		{
			printf("Custom function: SSIGPU();");
			SSIGPU();
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

	void NoArgumentFunction(int function, char funce[])
	{
		signed int(*func)() = ((signed int(*)())(function + _entry));
		int result = func();
		printf("\n%s returned: %d\n", funce,result);
	}
#pragma endregion
}
