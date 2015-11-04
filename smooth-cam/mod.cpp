#include <SADXModLoader.h>
#include <limits>

// TODO: Smooth camera rotation
// TODO: Make camera rotatable left and right with analog stick

DataPointer(int, FirstPerson_X, 0x03C4ADA0);
DataPointer(int, FirstPerson_Y, 0x03C4AD9C);

#define clamp(value, low, high) min(max(low, value), high)

int __cdecl SmoothCamera_c()
{
	const short x = Controllers[0].RightStickX;
	const short y = Controllers[0].RightStickY;
	const float m = (float)sqrt(x * x + y * y);
	const float nx = (x == 0) ? 0.0f : x / m;
	const float ny = (y == 0) ? 0.0f : y / m;
	const float n = m / 127.0f;

	FirstPerson_X += (int)(-364 * (nx * n));
	// Because manually moving into EAX wasn't working.
	return FirstPerson_Y = clamp(FirstPerson_Y + (int)(-364 * (ny * n)), -16201, 16201);
}

void* retaddr = (void*)0x004661DB;
void __declspec(naked) SmoothCamera_asm()
{
	__asm
	{
		call SmoothCamera_c
		jmp retaddr
	}
}

bool __cdecl CheckStick()
{
	return abs(Controllers[0].RightStickX) > 63 || abs(Controllers[0].RightStickY) > 63;
}

DataPointer(int, dword_3B2CA18, 0x3B2CA18);
void* trigger = (void*)0x00437648;
void* no_trigger = (void*)0x00437545;
void __declspec(naked) FirstPersonTrigger()
{
	__asm
	{
		// because the actual code is dumb
		mov dword_3B2CA18, ecx

		call CheckStick
		cmp eax, 0
		jnz not_zero

		jmp no_trigger

	not_zero:
		jmp trigger
	}
}

extern "C"
{
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };

	PointerInfo jumps[] = {
		{ (void*)0x00466173, SmoothCamera_asm },
		{ (void*)0x00437523, FirstPersonTrigger }
	};

	__declspec(dllexport) PointerList Jumps[] = { { arrayptrandlength(jumps) } };
}