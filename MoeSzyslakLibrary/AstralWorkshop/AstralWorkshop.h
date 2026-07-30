#pragma once
#include "../Neverwinter/Area.h"
#include "Character.h"
#include "../Testing.h"


/**
 * @class AstralWorkshop
 * @brief High‑level COM façade for the Astral Workshop simulation engine.
 *
 * AstralWorkshop provides a unified entry point for creating and managing all
 * major world entities—areas, characters, and placeable objects—and exposes
 * the simulation loop through the Heartbeat() function. External callers
 * (such as Python or a Flask-based world editor) interact with the engine
 * exclusively through this COM interface.
 *
 * Responsibilities:
 *  - World construction: create and register new Areas, Characters, and other
 *    engine-managed objects.
 *
 *  - Simulation control: advance the world state using Heartbeat(), which
 *    updates creatures, areas, world objects, movement, needs, and timed events.
 *
 *  - Engine state management: supports multiple operating modes such as
 *    normal gameplay, character creation, and world editor mode.
 *
 *  - Persistence: Save() and Export() allow the world to be serialized in
 *    engine-native or external-friendly formats.
 *
 *  - Testing and diagnostics: GetTester() exposes the internal Testing
 *    subsystem for automated validation and debugging.
 *
 * AstralWorkshop acts as the authoritative coordinator for all simulation
 * activity, ensuring consistent updates, object lifetime management, and
 * controlled access to internal engine structures.
 */
class AstralWorkshop : public IASTRALWORKSHOP
{
public:

	/**
	 * @enum ENGINE_STATE
	 * @brief Defines the current operating mode of the engine.
	 *
	 * STATE_NORMAL — Standard simulation mode.
	 * STATE_CHARACTER_CREATION — Interactive character creation workflow.
	 * STATE_WORLD_EDITOR — World editing and area manipulation mode.
	 */
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
	 * @brief Creates a new Area and registers it with the engine.
	 *
	 * @param[out] iArea  Receives the IUnknown pointer for the new Area.
	 * @param[out] ndx    Receives the index of the Area in the engine's list.
	 * @param[in]  szName Name of the Area.
	 * @param[in]  nDanger Initial danger rating for the Area.
	 */
	HRESULT __stdcall NewArea(IUnknown** iArea, int* ndx, int nDanger);

	/**
	 * @brief Exports the world to an external XML, human-readable format.
	 *
	 * @param[in] szPath Output file path.
	 */
	HRESULT __stdcall Export(const wchar_t* szPath);

	/**
	 * @brief Saves the world in the engine's native binary format.
	 *
	 * @param[in] szPath Output file path.
	 */
	HRESULT __stdcall Save(const wchar_t* szPath);

	/**
	 * @brief Creates a new Character and registers it with the engine.
	 *
	 * @param[out] iCharacter Receives the IUnknown pointer for the new Character.
	 * @param[in]  szName     Character name.
	 * @param[in]  nClass     Class identifier.
	 * @param[in]  nBackground Background identifier.
	 */
	HRESULT __stdcall NewCharacter(IUnknown** iCharacter, const wchar_t* szName, int nClass, int nBackground);

	/**
	 * @brief Advances the simulation by a number of seconds.
	 *
	 * Updates creatures, areas, world objects, movement, needs, and timed events.
	 *
	 * @param[in] nSec Number of seconds to advance the simulation.
	 */
	HRESULT __stdcall Heartbeat(int nSec);

	/**
	 * @brief Retrieves the internal Testing subsystem interface.
	 *
	 * @param[out] iTst Receives the IUnknown pointer for the Testing object.
	 */
	HRESULT __stdcall GetTester(IUnknown** iTst);

	HRESULT __stdcall GetAreaNames(BSTR* areaList);

	HRESULT UnitTest();

private:
	LONG m_cRef;
	//int m_instance;					// Unique instance identifier for debugging and logging.
	//InterfaceCollection m_areas;	// Collection of all Areas registered with the engine.

	ComCollection m_areas;			// Collection of all Areas registered with the engine. m_areas;	// Collection of all Areas registered with the engine.

	Area* m_pStartArea;				// Pointer to the designated starting Area of the world.
	Character* m_pNewCharacter;		// Temporary pointer used during character creation workflows.
	Testing* m_pTester;				// Internal testing subsystem used for diagnostics and validation
	int m_worldTimeSec;				// Global world time in seconds since engine initialization.

	/**
	 * @brief Handles text-based commands during character creation mode.
	 *
	 * @param[in]  szCmd Command string.
	 * @param[out] szRet Output buffer for command results.
	 * @param[in]  nLen  Length of the output buffer.
	 */
	void HandleCharacterCreationCommand(const wchar_t* szCmd, wchar_t* szRet, UINT nLen);
	
};

