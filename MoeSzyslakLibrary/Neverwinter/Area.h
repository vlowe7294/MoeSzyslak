#pragma once

#include "../VLVariable.h"
#include "Placeable.h"
#include "Creature.h"
#include "..\Utilities\Database.h"
#include "../Testing.h"
#include <random>
#include <functional>

class Dice
{
public:
	Dice(UINT m_nSides, UINT nSeed = std::random_device{}());
	~Dice();
	UINT Roll(UINT nRolls = 1);

private:
	UINT m_nSides;
	std::mt19937 m_rng;
	

};



class Quest : public IUnknown
{
public:
	Quest(const wchar_t* szName, int nDuration, int nDifficulty);
	~Quest();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	void Copy(Quest& copyFrom);
	void Randomize();
	inline wstring Name() { return m_name; }
private:
	long m_cRef;
	wstring m_name;
	int m_duration;
	int m_difficulty;
};

class Encounter
{
public:
	enum DIFFICULTY
	{
		VERY_EASY,
		EASY,
		NORMAL,
		HARD,
		IMPOSSIBLE,
		DIFFICULTY_MAX
	};

	Encounter(wstring tag, DIFFICULTY dif, int nMax);
	~Encounter();

private:
	wstring m_tag;
	DIFFICULTY m_difficulty;
	int m_maximumCreatures;

};

class Location : public ILOCATION
{
public:
	Location();
	~Location();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall AddNeighbor(IUnknown* iNeighbor, int travelTimeSec);
	HRESULT __stdcall AddEncounter(const wchar_t* szTag, int dif, int nMax);
	HRESULT __stdcall AddPlaceable(const wchar_t* szName, const wchar_t* szTag);
	HRESULT __stdcall GetProperties(IUnknown** iPrp);

	void Tick(int nSec);
	void GetObjectByTag(IUnknown** iunk, const wchar_t* szTag);

private:
	LONG m_cRef;
	VLVariable* m_pName;

	// Graph edges
	vector<Location*> m_neighbors;
	vector<int> m_travelTimes; // seconds to travel to each neighbor
	Encounter* m_pEncounter;
	Placeable* m_pPlaceable;
	VariableCollection* m_pProperties;
};

using EventFn = std::function<void(IUnknown*, IUnknown*)>;

/**
 * @class Area
 * @brief Represents a distinct region within the game world.
 *
 * An Area is a container for Locations, environmental properties, and
 * quest-related state. It acts as the primary unit of world structure
 * and simulation. Each Area maintains its own danger level, properties,
 * and an associated Quest object. Areas are updated through Tick(),
 * which advances local simulation such as location behavior, world
 * objects (future), and quest progression.
 *
 * Areas are COM objects and follow standard COM lifetime rules.
 */
class Area : public IAREA
{
public:
	/**
	 * @brief Constructs a new Area with a name and danger rating.
	 *
	 * @param[in] szName   Display name of the Area.
	 * @param[in] nDanger  Initial danger level for encounters or events.
	 */
	Area();

	~Area();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/**
	 * @brief Advances the Area simulation by a number of seconds.
	 *
	 * This updates all Locations within the Area, as well as the associated
	 * Quest and any future world objects or environmental systems.
	 *
	 * @param[in] nSec Number of seconds to advance.
	 */
	HRESULT __stdcall Tick(int nSec);

	/**
	 * @brief Creates and adds a new Location to this Area.
	 *
	 * @param[out] iLoc      Receives the IUnknown pointer for the new Location.
	 * @param[in]  locName   Name of the Location.
	 */
	HRESULT __stdcall AddLocation(IUnknown** iLoc);

	HRESULT __stdcall GetProperties(IUnknown** iVarList);
	HRESULT __stdcall AddQuest(IUnknown** iQuest, const wchar_t* szName, int nDuration, int nDifficulty);
	
	/**
	* @brief Retrieves a Location by index.
	*
	* @param[in] ndx  Zero-based index of the Location.
	* @return Reference to the Location object.
	*/
	HRESULT __stdcall GetLocation(int ndx, IUnknown** iLoc);

	HRESULT __stdcall SetOnEnterHandler(const wchar_t* szFnName);

	HRESULT __stdcall UnitTest();

	/**
	 * @brief Serializes this Area into a database table.
	 *
	 * @param[out] tbl  Table to write Area data into.
	 */
	void Save(Table& tbl);

	/**
	 * @brief Loads Area data from a database table.
	 *
	 * @param[in] tbl  Table containing serialized Area data.
	 */
	void Load(Table& tbl);	

	void OnEnter(IUnknown* iEnteringObj);
	void GetObjectByTag(IUnknown** iunk, const wchar_t* szTag);

private:
	int m_cRef;
	VLVariable* m_pName;
	VLVariable* m_pDangerLevel;				// Danger rating used for encounters, events, or quest weighting.
	VariableCollection* m_pProperties;		// Property collection for editor-defined or dynamic attributes.

	ComCollection m_quests;						// The primary Quest associated with this Area.
	InterfaceCollection m_locations;		// Collection of Locations contained within this Area.
	EventFn m_onEnterHandler;

};
