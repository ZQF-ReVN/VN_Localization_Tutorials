#include "Tools.h"


DWORD g_dwExeBase = (DWORD)GetModuleHandleW(NULL);


DWORD g_dwReadChar = 0x0050F227;
DWORD g_dwRetAddre = g_dwReadChar + 0x6;
VOID __declspec(naked) ProcDBCS_0()
{
	_asm
	{
		movzx esi, byte ptr[eax];
		cmp esi, 0x81;
		jb not_DBCS;

		//is_DBCS
		movzx esi, word ptr[eax];
		inc eax;

	not_DBCS:
		mov dword ptr[ebp - 0x10], esi;
		jmp g_dwRetAddre;
	}
}

VOID StartHook()
{
//Step1:
	//Hook CreateFontA 替换字体和字符集
	HookCreateFontA(0x86, "黑体");

//Step2:
	//0050F227 | 0FB630        | movzx esi, byte ptr ds : [eax]      | eax为指向当前脚本字符的指针，随着读取的字符数移动
	//0050F22A | 8975 F0       | mov dword ptr ss : [ebp - 10] , esi | 什么的代码将读取一个字节，即单字节到esi，此处把读取的数据放到ebp-10处
	//0050F22D | 85F6          | test esi, esi                       | 判断esi是否为0，即空字符
	//0050F22F | 0F84 4C050000 | je eden_pm_en.50F781                | 为0则跳出处理
	//0050F235 | 40            | inc eax                             | 不为0则表示正常读取，并给eax加一，即指向脚本中下一个字节
	// 
	// 所以我们Hook这个位置，判断eax读到esi的一个字节是否大于等于0x81，如果是则认为这个是属于DBCS(GBK,SJIS)即是双字节组成一个字符的，那么读取两个字节放到esi中
	// 如果是小于0x81的那么说明是单字节组成一个字符的，比如英文字母阿拉伯数字这种，按照正常逻辑读取就行了。
	// 这得注意的是eax如果判断为当前读取的字符是双字节得再加1才行。
	// 
	//HooK读取脚本字符串 始终单字节读取 转为 判断单双字节读取
	WriteHookCode(g_dwReadChar, (DWORD)ProcDBCS_0, 0x6);

//Step3:
	//00568DEC 人物名 括号
	// 
	//00598720 人物名 英日对照	   表        [存储游戏所有 英文人物名 对应的 日文人物名]
	//004B7530 人物名 英日对照   初始化函数 [用于初始化遍历人物名表]
	// 
	//005D0C08 人物名 英日对照   缓冲区     [存储当前读取到的 英文人物名 对应的 日文人物名]
	//004B7340 人物名 对比对照   处理函数   [用于显示文字过程中对比当前 英文人物名 找出 日文人物名]
	// 
	//人物名括号修正 英文版本用的一种单字节字符集拓展，会大于7F,游戏的括号两个分别是0xAB,0xBB均超出了0x7F
	//一般我们只判断单字节是否大于0x81，如果是则认为是DBCS，如果游戏保持这种括号，那么单双字节读取判断会混乱。
	//所以这里我们把 0xAB ,0xBB这种单字节的括号改成中文的 【】 让其符合DBCS逻辑，防止单双字节判断混乱。
	static BYTE aCharacterName0[] = { 0x20,0xA1,0xBF };
	static BYTE aCharacterName1[] = { 0xA1,0xBE,0x20 };
	WriteMemory((LPVOID)(g_dwExeBase + 0x168DF4), aCharacterName0, sizeof(aCharacterName0));
	WriteMemory((LPVOID)(g_dwExeBase + 0x168DF8), aCharacterName1, sizeof(aCharacterName1));

	//此处Path人物名 ，在人物名  英日对照 表 里可以找到地址
	UCHAR** pTable = (UCHAR**)(g_dwExeBase + 0x198720);
	//00  "Elica"   -> 艾丽卡
	//01  "エリカ"
	//02  "General" -> 总部部长
	//03  "本部長"
	//04  "Inaba"   -> 稻叶
	//05  "稲葉"
	//06  "Lavie"   -> 拉维
	//07  "ラヴィ"
	//08  "Maya"    -> 真夜
	//09  "真夜"
	//10  "Natsume" -> 夏目
	//11  "ナツメ"
	//12  "Ryou"    -> 亮
	//13  "亮"
	//14  "Sion"    -> 诗音
	//15  "シオン"
	//16  "Soldier" -> 士兵
	//17  "兵士"
	static UCHAR aElica[] = { 0xB0, 0xAC, 0xC0, 0xF6, 0xBF, 0xA8, 0x00 };
	pTable[0] = aElica;

	static UCHAR aGeneral[] = { 0xD7, 0xDC, 0xB2, 0xBF, 0xB2, 0xBF, 0xB3, 0xA4, 0x00 };
	pTable[2] = aGeneral;

	static UCHAR aInaba[] = { 0xB5, 0xBE, 0xD2, 0xB6, 0x0 };
	pTable[4] = aInaba;

	static UCHAR aLavie[] = { 0xC0, 0xAD, 0xCE, 0xAC,0x00 };
	pTable[6] = aLavie;

	static UCHAR aMaya[] = { 0xD5, 0xE6, 0xD2, 0xB9, 0x00 };
	pTable[8] = aMaya;

	static UCHAR aNatsume[] = { 0xCF, 0xC4, 0xC4, 0xBF, 0x00 };
	pTable[10] = aNatsume;

	static UCHAR aRyou[] = { 0xC1, 0xC1, 0x00 };
	pTable[12] = aRyou;

	static UCHAR aSion[] = { 0xCA, 0xAB, 0xD2, 0xF4, 0x00 };
	pTable[14] = aSion;

	static UCHAR aSoldier[] = { 0xCA, 0xBF, 0xB1, 0xF8, 0x00 };
	pTable[16] = aSoldier;

//Step4:
	//0050F084
	//004C293D
	//处理常见的0x81 0x9F 0xE0边界
	static BYTE aSJISRange[] = { 0xFE };
	WriteMemory((LPVOID)(g_dwExeBase + 0xC293F), aSJISRange, sizeof(aSJISRange));
	WriteMemory((LPVOID)(g_dwExeBase + 0x10F08E), aSJISRange, sizeof(aSJISRange));
	WriteMemory((LPVOID)(g_dwExeBase + 0x10F09E), aSJISRange, sizeof(aSJISRange));

//Step5:
	//004C2971
	//此处为游戏脚本的初始化过滤，这个步骤发生在游戏读取脚本打印之前，可以理解为脚本的预处理
	//这个步骤进行了一些字符串的替换，比如这个游戏的脚本里虽然对话都是英文，但是有一部分日文的标点符号
	//比如双引号，游戏脚本里其实还是日文的，但是显示出来就成了正常的双引号，这个原因是游戏在脚本预处理阶段
	//把这些日文的双引号替换掉了。
	//游戏替换的逻辑是，如果定义了 A 替换为 B 输入的是 A 则替换成 B
	//如果输入的是双字节字符，但是并没有预定义该字符应该替换成什么，那么游戏就会统一把该双字节字符替换成单字节0xB6
	//其实游戏里把日文的符号替换也是从双字节改成单字节，比如0x8168改成0x94
	//此处为防止游戏把没有预定义的双字节字符替换成0xB6
	static BYTE aProcDBCS_1_ASM[] = { 0x66, 0x89, 0x5D, 0x0C, 0x90, 0x90, 0x90 };
	WriteMemory((LPVOID)(g_dwExeBase + 0xC2971), aProcDBCS_1_ASM, sizeof(aProcDBCS_1_ASM));

	//004C299B
	//替换MAP一些符号，比如把日文的引号MAP成0x91,0x92最后在MultiByteToWideChar之前替换回来
	//游戏会把一些日文的双字节标点符号给替换成单字节的，并在MultiByteToWideChar之前替换为Unicode编码的字符
	//这个游戏最终输出是通过MultiByteToWideChar转换成宽字节调用GetGlyphOutlineW输出的
	//Patch把日文的标点符号转换成GBK的
	static BYTE aMapCharASM[] =
	{
		0x3D, 0x65, 0x81, 0x00, 0x00,            // cmp eax,8165          -->   ‘   SJIS
		0x75, 0x07,					             // jne eden_pm_en.4C29A9 	  	    
		0xBB, 0xA1, 0xAE, 0x00, 0x00,            // mov ebx,AEA1          -->   ‘   GBK
		0xEB, 0xC8,					             // jmp eden_pm_en.4C2971 	  	    
		0x3D, 0x66, 0x81, 0x00, 0x00,            // cmp eax,8166          -->   ’   SJIS
		0x75, 0x07,					             // jne eden_pm_en.4C29B7 	  	    
		0xBB, 0xA1, 0xAF, 0x00, 0x00,            // mov ebx,AFA1          -->   ’   GBK
		0xEB, 0xBA,					             // jmp eden_pm_en.4C2971 	  	 
																			 	 
		0x3D, 0x67, 0x81, 0x00, 0x00,            // cmp eax,8167           -->   “   SJIS
		0x75, 0x07,					             // jne eden_pm_en.4C29C5  	  	 
		0xBB, 0xA1, 0xBA, 0x00, 0x00,            // mov ebx,BAA1           -->   『   GBK
		0xEB, 0xAC,					             // jmp eden_pm_en.4C2971  	  	 
		0x3D, 0x68, 0x81, 0x00, 0x00,            // cmp eax,8168           -->   ”   SJIS
		0x75, 0x07,					             // jne eden_pm_en.4C29D3  	  	 
		0xBB, 0xA1, 0xBB, 0x00, 0x00,            // mov ebx,BBA1           -->   』   GBK
		0xEB, 0x9E,					             // jmp eden_pm_en.4C2971  	 	 
													 						 	 
		0x3D, 0x5C, 0x81, 0x00, 0x00,            // cmp eax,815C           -->   ―   SJIS
		0x75, 0x07,					             // jne eden_pm_en.4C29E1 	 	 
		0xBB, 0xA8, 0x44, 0x00, 0x00,            // mov ebx,44A8           -->   ―   GBK
		0xEB, 0x90,					             // jmp eden_pm_en.4C2971 
												    
		0x3D, 0x96, 0x81, 0x00, 0x00,            // cmp eax,8196           -->   ＊   SJIS
		0x75, 0x07,					             // jne eden_pm_en.4C29EF 
		0xBB, 0xA3, 0xAA, 0x00, 0x00,            // mov ebx,AAA3           -->   ＊   GBK
		0xEB, 0x82,					             // jmp eden_pm_en.4C2971
												    
		0x3D, 0x60, 0x81, 0x00, 0x00,            // cmp eax,8160           -->   〜   SJIS
		0x75, 0x0A,					             // jne eden_pm_en.4C2A00  
		0xBB, 0x7E, 0x00, 0x00, 0x00,            // mov ebx,7E             -->   ~   ASCII
		0xE9, 0x71, 0xFF, 0xFF, 0xFF,            // jmp eden_pm_en.4C2971  
												    
		0x3D, 0xE1, 0x81, 0x00, 0x00,            // cmp eax,81E1          -->   ≪   SJIS
		0x75, 0x0A,					             // jne eden_pm_en.4C2A11
		0xBB, 0x3C, 0x00, 0x00, 0x00,            // mov ebx,3F            -->   <   ASCII
		0xE9, 0x60, 0xFF, 0xFF, 0xFF,            // jmp eden_pm_en.4C2971 
												    
		0x3D, 0xE2, 0x81, 0x00, 0x00,            // cmp eax,81E2           -->   ≫   SJIS
		0x75, 0x0A,					             // jne eden_pm_en.4C2A22  
		0xBB, 0x3E, 0x00, 0x00, 0x00,            // mov ebx,3F             -->   >   ASCII
		0xE9, 0x4F, 0xFF, 0xFF, 0xFF,            // jmp eden_pm_en.4C2971  
												    
		//处理不在预定义替换字符内的双字节字符		   
		0x89, 0xC3,                              // mov ebx,eax          
		0xC1, 0xEB, 0x08,                        // shr ebx,8            
		0xC1, 0xE0, 0x08,                        // shl eax,8            
		0x09, 0xC3,                              // or ebx,eax    
		0x90, 0x90, 0x90, 0x90,                  // NOP
		0x90, 0x90, 0x90, 0x90
	};
	WriteMemory((LPVOID)(g_dwExeBase + 0xC299B), aMapCharASM, sizeof(aMapCharASM));

//Step6:
	//0051A22F
	//游戏在调用 GetGlyphOutlineW 输出字形之前通过MultiByteToWideChar进行的窄字节转宽字节，
	//MultiByteToWideChar的参数需要修改，让其从单字节读取转换成双字节读取。该函数返回值也会影响单双字节读取需要修改判断返回值的代码
	//处理MultiByteToWideChar转换输入的字节从单字节转换到双字节 转换宽度改为2，CodePage改为0x3A8。
	static BYTE aProcDBCS_2_ASM[] =
	{
		0x6A, 0x02,                               // push 0x2
		0x66, 0x89, 0x45, 0xF8,                   // mov word ptr ss:[ebp-0x8], ax
		0x8D, 0x45, 0xF4,                         // lea eax, ss:[ebp-0xC]
		0x50,                                     // push eax
		0x6A, 0x02,                               // push 0x2
		0x8D, 0x4D, 0xF8,                         // lea ecx, ss:[ebp-0x8]
		0x51,                                     // push ecx
		0x6A, 0x01,                               // push 0x1
		0x68, 0xA8, 0x03, 0x00, 0x00,             // push 0x3A8
		0x89, 0x55, 0xF4,                         // mov dword ptr ss:[ebp-0xC], edx
		0xFF, 0x15, 0x10, 0x92, 0x54, 0x00,       // call dword ptr ds:[%MulitiByteToWideChar]
		0x83, 0xF8, 0x01,                         // cmp eax, 1
		0x75, 0x06,                               // jne 
		0x8B, 0x55, 0xF4,                         // mov edx, dword ptr ss:[ebp-0xC]
		0x89, 0x55, 0xFC,                         // mov dword ptr ss:[ebp-0x4], edx
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90        // NOP
	};
	WriteMemory((LPVOID)(g_dwExeBase + 0x11A22F), aProcDBCS_2_ASM, sizeof(aProcDBCS_2_ASM));
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		StartHook();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

VOID __declspec(dllexport) DirA() {}
