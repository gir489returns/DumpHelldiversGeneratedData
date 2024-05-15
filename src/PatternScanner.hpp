#pragma once

#include <Psapi.h>

struct module_info_helper
{
	static inline void GetModuleBaseAndSize(DWORD64* lpBase, DWORD64* lpSize)
	{
		MODULEINFO moduleInfo = {};

		HMODULE module = GetModuleHandle("game.dll");
		if (module)
		{
			GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof MODULEINFO);
		}
		else
		{
			MessageBox(NULL, "game.dll not found", "ERROR", MB_ICONERROR | MB_OK);
		}

		if (lpBase)
		{
			*lpBase = DWORD64(moduleInfo.lpBaseOfDll);
		}
		if (lpSize)
		{
			*lpSize = DWORD64(moduleInfo.SizeOfImage);
		}
	}
};

/**
 * \brief Provides compact utility to scan patterns and manipulate addresses.
 */
struct PatternScanner

{
	uint64_t Value = 0;

	PatternScanner(uint64_t value) :
		Value(value)
	{
	}

	PatternScanner() :
		PatternScanner(0)
	{
	}

	static inline PatternScanner Scan(const char* patternStr, const char* debugName = nullptr)
	{
		static uint64_t s_ModuleSize;
		static uint64_t s_ModuleBase;
		static bool s_Init = false;
		if (!s_Init)
		{
			module_info_helper::GetModuleBaseAndSize(&s_ModuleBase, &s_ModuleSize);
			s_Init = true;
		}

		// Convert string pattern into byte array form
		int16_t pattern[256];
		uint8_t patternSize = 0;
		for (size_t i = 0; i < strlen(patternStr); i += 3)
		{
			const char* cursor = patternStr + i;

			if (cursor[0] == '?')
			{
				pattern[patternSize] = -1;
			}
			else
			{
				pattern[patternSize] = static_cast<int16_t>(strtol(cursor, nullptr, 16));
			}

			// Support single '?' (we're incrementing by 3 expecting ?? and space, but with ? we must increment by 2)
			if (cursor[1] == ' ')
			{
				i--;
			}

			patternSize++;
		}

		// In two-end comparison we approach from both sides (left & right) so size is twice smaller
		uint8_t scanSize = patternSize;
		if (scanSize % 2 == 0)
		{
			scanSize /= 2;
		}
		else
		{
			scanSize = patternSize / 2 + 1;
		}

		// Search for string through whole module
		// We use two-end comparison, nothing fancy but better than just brute force
		for (uint64_t i = 0; i < s_ModuleSize; i += 1)
		{
			const uint8_t* modulePos = (uint8_t*)(s_ModuleBase + i);
			for (uint8_t j = 0; j < scanSize; j++)
			{
				int16_t lExpected = pattern[j];
				int16_t lActual = modulePos[j];

				if (lExpected != -1 && lActual != lExpected)
				{
					goto miss;
				}

				int16_t rExpected = pattern[patternSize - j - 1];
				int16_t rActual = modulePos[patternSize - j - 1];

				if (rExpected != -1 && rActual != rExpected)
				{
					goto miss;
				}
			}
			return { s_ModuleBase + i };
		miss:;
		}

		/*if (debugName)
			printf("Not found %s", debugName);*/

		return { 0 };
	}

	PatternScanner GetAt(int32_t offset) const
	{
		return Value + offset;
	}

	PatternScanner GetRef(int32_t offset = 0) const
	{
		return Value + offset + sizeof(DWORD) + *(int32_t*)(Value + offset);
	}

	PatternScanner GetCall() const
	{
		return GetRef(1);
	}

	template<typename T>
	T To() const
	{
		return (T)Value;
	}

	template<typename T>
	T* ToFunc() const
	{
		return To<T*>();
	}

	PatternScanner& operator=(uint64_t value)
	{
		Value = value;
		return *this;
	}

	operator uint64_t() const
	{
		return Value;
	}

	operator void* () const
	{
		return (void*)Value;
	}
};
