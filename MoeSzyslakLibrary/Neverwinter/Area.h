#pragma once

#include "../VLVariable.h"
#include "Placeable.h"
#include "Creature.h"
#include "..\Utilities\Database.h"
#include <random>	

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






class Quest
{
public:
	Quest(const wchar_t* szName, int nDuration, int nDifficulty);
	~Quest();
	void Copy(Quest& copyFrom);
	void Randomize();
	inline wstring Name() { return m_name; }
private:
	wstring m_name;
	int m_duration;
	int m_difficulty;
};

class Location : public IUnknown
{
public:
	Location(const wchar_t* szName);
	~Location();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	void AddNeighbor(Location& neighbor, int travelTimeSec);

private:
	LONG m_cRef;
	wstring m_name;

	// Graph edges
	vector<Location*> m_neighbors;
	vector<int> m_travelTimes; // seconds to travel to each neighbor
};


class Area : public IAREA
{
public:
	Area(const wchar_t* szName, int nDanger);
	~Area();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Properties(IUnknown** iPrp);
	HRESULT __stdcall Command(const wchar_t* szCmd, IUnknown* iRetStr);
	HRESULT __stdcall Tick(int nSec);

	Placeable& GetPlaceable(int ndx);
	void Save(Table& tbl);
	void Load(Table& tbl);
	Location& AddLocation(const wchar_t* szCmd);
	void LinkLocations(Location& a, Location& b, int travelTimeSec);

	inline Quest& GetQuest() { return *m_pQuest; };
	inline wstring GetName() { return m_name; };
	

private:
	int m_cRef;
	wstring m_name;
	InterfaceCollectionInf m_contents;
	VariableCollection* m_pProperties;
	int m_nDangerLevel;
	Quest* m_pQuest;
	InterfaceCollection m_locations;

};
