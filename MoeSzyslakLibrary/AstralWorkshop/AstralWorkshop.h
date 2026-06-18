#pragma once
#include "../Neverwinter/Area.h"
#include "Character.h"


/**
 * @class AstralWorkshop
 * @brief COM-based game engine façade for the Astral Workshop simulation.
 *
 * This class implements the IASTRALWORKSHOP interface and acts as the primary
 * command dispatcher for the game engine. It manages engine state, character
 * creation, world initialization, and provides a single entry point for
 * external callers (such as Python/Flask) to interact with the simulation.
 */
class AstralWorkshop : public IASTRALWORKSHOP
{
public:

	enum ENGINE_STATE
	{
		STATE_NORMAL,
		STATE_CHARACTER_CREATION,
		STATE_WORLD_EDITOR
	};

	AstralWorkshop();
	~AstralWorkshop();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/**
	 * @brief Processes a command string and returns a response.
	 *
	 * This is the primary entry point for external callers. Commands may
	 * initiate character creation, exit the engine, or be routed to the
	 * character creation state machine if a character is currently being built.
	 *
	 * @param szCmd The input command string (wide-character).
	 * @param szRet Output buffer receiving the engine's response text.
	 * @param nLen  Length of the output buffer in wide characters.
	 *
	 * @return S_OK on success, or an HRESULT error code on failure.
	 */
	HRESULT __stdcall Command(const wchar_t* szCmd, wchar_t* szRet, UINT nLen);

	HRESULT __stdcall NewArea(IUnknown** iArea, int* ndx, const wchar_t* szName, int nDanger);

	HRESULT __stdcall Export(const wchar_t* szPath);
	HRESULT __stdcall Save(const wchar_t* szPath);
	HRESULT __stdcall NewCharacter(IUnknown** iCharacter, const wchar_t* szName, int nClass, int nBackground);

	HRESULT UnitTest();

private:
	LONG m_cRef;
	ENGINE_STATE m_state;
	int m_instance;
	InterfaceCollection m_areas;
	Area* m_pStartArea;
	Character* m_pNewCharacter;

	void HandleCharacterCreationCommand(const wchar_t* szCmd, wchar_t* szRet, UINT nLen);
	
};

