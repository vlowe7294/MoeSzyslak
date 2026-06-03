#pragma once
#include "../Neverwinter/Creature.h"
#include "../Neverwinter/Area.h"

class Character : public Creature
{
	public:
		enum BACKGROUND
		{
			BACKGROUND_NONE,
			BACKGROUND_NOBLE,
			BACKGROUND_PEASANT,
			BACKGROUND_OUTLAW,
			BACKGROUND_MAX
		};

		enum ACTIVITY
		{
			ACTIVITY_IDLE,
			ACTIVITY_ON_QUEST,
			ACTIVITY_RETURNING,
			ACTIVITY_RESTING
		};

		Character(const wchar_t* szName);
		~Character();
		HRESULT CreateCharacter(wstring strCmd, wstring& strOut);
		HRESULT UnitTest();

		inline bool IsComplete() { return m_bComplete; }

private:
	bool m_bComplete;
	BACKGROUND m_background;
	ACTIVITY m_activity;
	int m_nEditStep;

};

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

	HRESULT UnitTest();

private:
	LONG m_cRef;
	int m_state;
	int m_instance;
	Area* m_pArea;
	Character* m_pNewCharacter;
	
};

