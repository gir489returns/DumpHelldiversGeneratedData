#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <random>
#include <glaze/glaze.hpp>
#include <cstdint>

#define DAMAGE_STRUCT_SIZE 3168 / 8
#define DAMAGE_POINTER_LOCATION 0x7FFB89F58390
#define DAMAGE_STRING_LOCATION 0x11ED375A3B5 //DamageInfoType_None

class CDamageInstance
{
public:
	int32_t m_id; //0x0000
	int32_t m_max_damage; //0x0004
	int32_t m_min_damage; //0x0008
	int32_t m_penetration_no_angle; //0x000C
	int32_t m_penetration_angle; //0x0010
	int32_t m_penetration_3; //0x0014
	int32_t m_penetration_4; //0x0018
	int32_t m_demolition; //0x001C
	int32_t m_pushback; //0x0020
	int32_t m_unk1; //0x0024
	int32_t m_unk2; //0x0028
	int32_t m_status_effect_type; //0x002C
	float m_stun_factor; //0x0030
	int32_t m_unk5; //0x0034
	float m_unk6; //0x0038
	char m_name[100];
}; //Size: 0x004C
//static_assert(sizeof(CDamageInstance) == 0x4C);

/**
 * @brief Specify the values we want to export
 *
 * @tparam
 */
template <>
struct glz::meta<CDamageInstance>
{
	using T = CDamageInstance;
	static constexpr auto value =
		object(
			&T::m_id,
			&T::m_name,
			&T::m_max_damage,
			&T::m_min_damage,
			&T::m_penetration_no_angle,
			&T::m_penetration_angle,
			&T::m_penetration_3,
			&T::m_penetration_4,
			&T::m_demolition,
			&T::m_pushback,
			&T::m_unk1,
			&T::m_unk2,
			&T::m_status_effect_type,
			&T::m_stun_factor,
			&T::m_unk5,
			&T::m_unk6);
};

class generated_damage_settings
{
public:

	CDamageInstance m_damage_instances[DAMAGE_STRUCT_SIZE]; // 0x0000
}; // Size: 0x0BE0

void dump_damage_data(generated_damage_settings* inst)
{
	auto damage_instance_span = std::span(inst->m_damage_instances, DAMAGE_STRUCT_SIZE); //memset(generated_damage_settings, 0, 0xBE0ui64);

	// ignoring the return because compiler asks us not to
	(void)glz::write_file_json < glz::opts{ .prettify = true } > (damage_instance_span, "generated_damage_settings.json", std::string{});

	// no clue why this doesn't work, can be figured out later
	// https://github.com/stephenberry/glaze/blob/main/docs/csv.md
	// (void)glz::write_file_csv(span_hack, "test.csv", std::string{});
}

std::vector<std::string> splitText(const std::string& text, char delimiter) {
	std::vector<std::string> parts;
	std::string part;
	for (char ch : text) {
		if (ch == delimiter) {
			if (!part.empty()) {
				parts.push_back(part);
				part.clear();
			}
		}
		else {
			part += ch;
		}
	}
	if (!part.empty()) {
		parts.push_back(part);
	}
	return parts;
}


int main()
{
	//auto generated_damage_settings_constructor = PatternScanner::Scan("E8 ? ? ? ? 48 8B 3D ? ? ? ? 48 8B 87").GetCall().To<std::uint8_t*>();
	//if (generated_damage_settings_constructor == nullptr)
	//{
	//	MessageBox(NULL, "Failed to find generated_damage_settings_constructor. Defaulting to size 380.", "ERROR", MB_ICONERROR | MB_OK);
	//	damage_struct_size = 380;
	//}
	//else
	//{
	//	for (int i = 0; i < 0x100; i++)
	//	{
	//		if (*reinterpret_cast<std::uint16_t*>(generated_damage_settings_constructor + i) == 0xB841)
	//		{
	//			damage_struct_size = *reinterpret_cast<std::uint32_t*>(generated_damage_settings_constructor + i + 2);
	//			damage_struct_size /= 8;
	//			break;
	//		}
	//	}
	//	if (damage_struct_size == NULL)
	//	{
	//		MessageBox(NULL, "Failed to find memset instruction size. Defaulting to size 380.", "ERROR", MB_ICONERROR | MB_OK);
	//		damage_struct_size = 380;
	//	}
	//}
	//
	//auto generated_damage_settings_instance = PatternScanner::Scan("48 8D 15 ? ? ? ? 89 46").GetRef(0x3).To<generated_damage_settings*>();
	//if (generated_damage_settings_instance == nullptr)
	//{
	//	MessageBox(NULL, "generated_damage_settings instance signature failed.", "ERROR", MB_ICONERROR | MB_OK);
	//}
	//else
	//{
	//	dump_damage_data(generated_damage_settings_instance);
	//}

	auto inst = new generated_damage_settings();

	DWORD processID = 0;
	SIZE_T bytesRead;
	DWORD buffer = 0;

	// Get the process ID
	HWND hwnd = FindWindow(NULL, "HELLDIVERS™ 2");
	if (hwnd == NULL)
	{
		std::cerr << "Cannot find Helldivers 2." << std::endl;
		return 1;
	}

	GetWindowThreadProcessId(hwnd, &processID);

	// Open the process
	HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, processID);
	if (process == NULL)
	{
		std::cerr << "Cannot open Helldivers 2." << std::endl;
		return 1;
	}

	LPCVOID pointers[DAMAGE_STRUCT_SIZE];

	// Read the memory
	if (ReadProcessMemory(process, (LPCVOID)DAMAGE_POINTER_LOCATION, &pointers, sizeof(pointers), &bytesRead))
	{
		std::cout << "Pointer read successfully." << std::endl;
	}
	else
	{
		std::cerr << "Failed to read memory from Helldivers 2." << std::endl;
	}

	for (int i = 1; i < DAMAGE_STRUCT_SIZE; i++)
	{
		if (ReadProcessMemory(process, pointers[i], &inst->m_damage_instances[i], sizeof(CDamageInstance), &bytesRead))
		{
			std::cout << "Weapon " << i << " read successfully." << std::endl;
		}
		else
		{
			std::cerr << "Pointer " << i << " failed to read." << std::endl;
		}
	}

	std::cout << "Reading damage names." << std::endl;

	// Read the specified chunk of memory
	SIZE_T length = 0x20000;
	std::vector<char> char_buffer(length);
	if (!ReadProcessMemory(process, (LPCVOID)DAMAGE_STRING_LOCATION, char_buffer.data(), length, &bytesRead)) {
		std::cerr << "Failed to read damage names." << std::endl;
		CloseHandle(process);
		return 1;
	}

	std::cout << "Read damage names." << std::endl;

	// Convert buffer to string
	std::string textChunk(char_buffer.begin(), char_buffer.end());

	// Split the text by periods
	std::vector<std::string> splitTextParts = splitText(textChunk, '\0');

	std::cout << "Spliting names." << std::endl;

	// Output the split text parts
	int i = 0;
	for (const std::string& part : splitTextParts) {
		if (part == "DamageInfoType_Count")
			break;
		std::cout << "Found damage name: " << part.substr(15, 100) << std::endl;
		strcpy(inst->m_damage_instances[i++].m_name, part.substr(15, 100).c_str());
	}

	std::cout << "Flushing to disk." << std::endl;

	dump_damage_data(inst);

	std::cout << "Finished." << std::endl;

	return 0;
}