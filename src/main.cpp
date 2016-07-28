#include <windows.h>
#include <cstdio>

HINSTANCE gDllInstance = NULL;

extern "C"
{
	const int _entry = 0x00;
	const int _JMP = 0x51C320;
	const int _MUSICLOAD = 0x51F580;
	const int _MUSICCHANGE = 0x51F640;
	const int ENTRY = 0x0188C810;

	//core functions
	int RecognizeScript(char input[]);

	//script functions
	void JMP(int a1);
	void MUSICLOAD(int music);
	void MUSICCHANGE();

//DEF:
	//signed int(*PlaySystemSound)(unsigned int Sound_ID) = ((signed int(*)(unsigned int))0x0046B270);

    BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
    {
        if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        {
            gDllInstance = hModule;
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
	*/
#pragma endregion

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

	_declspec(dllexport) void SSIGPU()
	{
		signed int(*SSIGPU_Initialize)() = ((signed int(*)())0x0045C320);
		SSIGPU_Initialize();
	}

	_declspec(dllexport) void Start()
	{
		AllocConsole();
		freopen("CON", "w", stdout);
		freopen("CON", "r", stdin);
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
			if (a == 1)
				printf("\nUNRECOGNIZED OPERATION!\n");
		}
	}
#pragma region ScriptRecognizer
	int RecognizeScript(char input[])
	{
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
		return 1;
	}
#pragma endregion

	void MUSICLOAD(int music)
	{
		byte* b = (byte*)(ENTRY + 0x184); //MOV AL, [EDX+184h] (MOVSX ECX, AL)
		*b = 4;
		byte* musicpo = (byte*)(ENTRY+*b*4);
		*musicpo = (byte)(music & 0xFF);
		byte* protectionswitch = (byte*)(ENTRY + 0x174); 
		*protectionswitch = 7;
		signed int(*MusicLoad)(int a1) = ((signed int(*)(int))(_MUSICLOAD + _entry));
		int a = (MusicLoad(ENTRY)&255);
		*protectionswitch = 0;
		printf("\nMUSICLOAD returned: %d\n", a);
	}

	void MUSICCHANGE()
	{
		signed int(*MusicChange)() = ((signed int(*)())(_MUSICCHANGE + _entry));
		int a = (MusicChange()&0xFF);
		printf("\nMUSICCHANGE returned: %d\n", a);
	}

	void JMP(int a1)
	{
		signed int(*JMP)(signed int jump) = ((signed int(*)(signed int))(_JMP + _entry));
		int result = (JMP(a1 & 0xFFFF)&0xFF);
		printf("\nJMP returned: %d\n", result);
	}
}
