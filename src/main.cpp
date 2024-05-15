#define NOMINMAX
#include <Windows.h>
#include <random>
#include <glaze/glaze.hpp>
#include <cstdint>
#include "PatternScanner.hpp"

class CDamageInstance
{
public:
	int32_t m_id; //0x0000
	int32_t m_base_damage; //0x0004
	int32_t m_penetration_no_angle; //0x0008
	int32_t m_penetration_angle; //0x000C
	int32_t m_penetration_3; //0x0010
	int32_t m_penetration_4; //0x0014
	int32_t m_demolition; //0x0018
	int32_t m_pushback; //0x001C
	int32_t m_unk1; //0x0020
	int32_t m_unk2; //0x0024
	int32_t m_unk3; //0x0028
	int32_t m_unk4; //0x002C
	float m_unk5; //0x0030
	int32_t m_unk6; //0x0034
	float m_unk7; //0x0038
	char pad_003C[16]; //0x003C
}; //Size: 0x004C
static_assert(sizeof(CDamageInstance) == 0x4C);

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
			&T::m_base_damage,
			&T::m_penetration_no_angle,
			&T::m_penetration_angle,
			&T::m_penetration_3,
			&T::m_penetration_4,
			&T::m_demolition,
			&T::m_pushback,
			&T::m_unk1,
			&T::m_unk2,
			&T::m_unk3,
			&T::m_unk4,
			&T::m_unk5,
			&T::m_unk6,
			&T::m_unk7);
};

class generated_damage_settings
{
public:
	generated_damage_settings()
	{
		for (auto& item : m_damage_instances)
		{
			item = new CDamageInstance;
		}
	}
	~generated_damage_settings()
	{
		for (auto& item : m_damage_instances)
		{
			delete item;
		}
	}

	CDamageInstance* m_damage_instances[380]; // 0x0000
}; // Size: 0x0BE0
static_assert(sizeof(generated_damage_settings) == 0xBE0);

void dump_damage_data(generated_damage_settings* inst)
{
	auto span_hack = std::span(inst->m_damage_instances, 380); //memset(generated_damage_settings, 0, 0xBE0ui64);

	// ignoring the return because compiler asks us not to
	(void)glz::write_file_json < glz::opts{ .prettify = true } > (span_hack, "./data/game/generated_damage_settings.json", std::string{});

	// no clue why this doesn't work, can be figured out later
	// https://github.com/stephenberry/glaze/blob/main/docs/csv.md
	// (void)glz::write_file_csv(span_hack, "test.csv", std::string{});
}

#ifdef _DEBUG
int main()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, std::numeric_limits<std::uint32_t>::max());

	auto inst = new generated_damage_settings();

	int i = 0;
	for (auto tmp : inst->m_damage_instances)
	{
		tmp->m_id = i++;
		tmp->m_base_damage = dist(rng);
		tmp->m_penetration_no_angle = dist(rng);
		tmp->m_penetration_angle = dist(rng);
		tmp->m_penetration_3 = dist(rng);
		tmp->m_penetration_4 = dist(rng);
		tmp->m_demolition = dist(rng);
		tmp->m_pushback = dist(rng);
	}

	dump_damage_data(inst);

	return 0;
}
#else

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL, // handle to DLL module
	DWORD fdwReason,    // reason for calling function
	LPVOID lpvReserved) // reserved
{
	// Perform actions based on the reason for calling.
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		auto generated_damage_settings_instance = PatternScanner::Scan("48 8D 15 ? ? ? ? 89 46").GetRef(0x3).To<generated_damage_settings*>();
		if (generated_damage_settings_instance == nullptr)
		{
			MessageBox(NULL, "generated_damage_settings instance signature failed.", "ERROR", MB_ICONERROR | MB_OK);
		}
		else
		{
			dump_damage_data(generated_damage_settings_instance);
		}
		return TRUE; // Successful DLL_PROCESS_ATTACH
	}
}

#endif