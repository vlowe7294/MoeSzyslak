#pragma once
#include "VLString.h"
#include <atomic>
#include "Utilities/Database.h"
#include "Testing.h"

/**
 * @class Trail
 * @brief Represents a hiking trail associated with a campsite or area.
 *
 * A Trail contains basic descriptive information such as name, difficulty,
 * and length. Trails may later be extended to include elevation gain,
 * GPS coordinates, photos, or reviews.
 */
class Trail : public IUnknown
{
public:

	/**
	 * @enum DIFFICULTY
	 * @brief Difficulty rating for a trail.
	 */
	enum DIFFICULTY
	{
		DIFFICULTY_EASY,
		DIFFICULTY_MODERATE,
		DIFFICULTY_HARD
	};

	Trail(wstring nme, DIFFICULTY diff, double len);
	~Trail();

	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	wstring GetName() { return m_name; }

private:
	LONG m_cRef;
	wstring m_name;          /**< Name of the trail. */
	DIFFICULTY m_difficulty; /**< Difficulty rating. */
	double m_lengthMiles;    /**< Length of the trail in miles. */
};





/**
 * @class Site
 * @brief Represents a single campsite within a CampArea.
 *
 * A Site contains a unique ID, name, and geographic coordinates.
 * Sites may later be extended to include trails, photos, and reviews.
 *
 * This class implements IUnknown for COM interoperability.
 */
class Site : public IUnknown
{
public:
	Site(double lat, double lon);
	~Site();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/**
	 * @brief Prints site information to stdout (debugging).
	 */
	void Print();
	wstring MapLink();

	inline wstring GetName() { return m_name; }
	inline void SetName(wstring s) { m_name = s; }

private:
	LONG m_cRef;
	uint32_t m_id;
	wstring m_name;
	double m_latitude;
	double m_longitude;
};



/**
 * @class CampArea
 * @brief Represents a named geographic area containing multiple campsites.
 *
 * A CampArea owns a collection of Site objects and may optionally contain
 * a Trail (currently a single trail, but may be expanded to a list).
 *
 * This class supports saving/loading from a Database object.
 */
class CampArea : public ICAMPAREA
{
public:
	CampArea(wstring nme);
	~CampArea();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT __stdcall GetName(BSTR* szNme);
	HRESULT __stdcall SetName(const wchar_t* szNme);
	HRESULT __stdcall GetSite(int ndx, IUnknown** iSite);



	/**
	 * @brief Saves area data to a database.
	 * @param db Database instance to write to.
	 */
	void Save(Database& db);

	/**
	 * @brief Loads area data from a database.
	 * @param db Database instance to read from.
	 */
	void Load(Database& db);

	void AddTrail(Trail* pTrail);

	/**
	 * @brief Returns the collection of sites belonging to this area.
	 */
	inline ComCollection& GetSites() { return m_sites; }

	inline wstring GetName() { return m_name; }

private:
	LONG m_cRef;               /**< COM reference count. */
	wstring m_name;            /**< Name of the area. */
	ComCollection m_sites;	   /**< Collection of Site objects. */
	ComCollection m_trails;    /**< Collection of trails associated with the area. */
};


/**
 * @class CampSight
 * @brief Main engine class implementing the ICAMPSIGHT COM interface.
 *
 * CampSight manages a collection of CampArea objects and provides
 * high-level operations such as adding sites and running unit tests.
 */
class CampSight : public ICAMPSIGHT
{
public:
	CampSight();
	~CampSight();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();	

	/**
	 * @brief Retrieves the engine's Testing interface.
	 * @param iTester Receives the IUnknown pointer to the tester.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall GetTester(IUnknown** iTester);

	/**
	 * @brief Adds a new campsite to an area, creating the area if needed.
	 * @param szArea Name of the area.
	 * @param szSite Name of the site.
	 * @param lat Latitude.
	 * @param lon Longitude.
	 * @return HRESULT indicating success or failure.
	 */
	HRESULT __stdcall AddSite(const wchar_t* szArea, const wchar_t* szSite, double lat, double lon);

	HRESULT __stdcall GetArea(int ndx, IUnknown** iArea);


	HRESULT __stdcall UnitTest();

private:
	LONG m_cRef;                 /**< COM reference count. */
	Testing* m_pTest;		     /**< Testing interface. */
	ComCollection m_areas;		/**< Collection of CampArea objects. */
};
