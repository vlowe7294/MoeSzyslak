#pragma once
#include "Interfaces.h"
#include "VLVariable.h"

/**
 * @class Transaction
 * @brief Represents a financial transaction with date, category, description, amount, and recurrence.
 *
 * This class implements a COM-style reference-counted object. Each Transaction
 * has a unique ID, a scheduled date, and an optional recurrence frequency.
 * The Update() method advances the transaction date forward based on its
 * recurrence rule until it is no longer in the past.
 */
class Transaction : public IUnknown
{
public:
	/**
	 * @enum FREQUENCY
	 * @brief Defines how often a transaction recurs.
	 */
	enum FREQUENCY 
	{
		NONE,
		WEEK,
		BIWEEK,
		MONTH,
		YEAR	
	};

	Transaction(VLDateTime& dte, wstring ctgry, wstring desc, FREQUENCY nFreq );
	~Transaction();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT Command(const wchar_t* szCmd);

	/**
	 * @brief Prints transaction details to stdout.
	 *
	 * Intended for debugging and console output.
	 */
	void Print();

	/**
	*@brief Advances the transaction date based on its recurrence rule.
	*
	* If the transaction date is in the past, this method repeatedly increments
	* the date until it is no longer earlier than the current system date.
	*/
	void Update();

	inline VLDateTime& GetDate() { return m_date; };
	inline FREQUENCY GetFrequency() { return m_nFrequency; };

private:
	int m_cRef;                 /**< COM reference count. */
	VariableCollection* m_pProperties;

	int m_id;                   /**< Unique transaction ID. */
	static std::atomic<int> m_nNextID;       /**< Static counter for generating IDs. */
	VLVariable* m_pAmount;      /**< Transaction amount. */
	VLDateTime m_date;          /**< Transaction date. */
	wstring m_category;         /**< Category label. */
	wstring m_desc;             /**< Description text. */
	FREQUENCY m_nFrequency;     /**< Recurrence rule. */
};



/**
 * @class Account
 * @brief Represents a financial account containing transactions and metadata.
 *
 * This class stores account information such as ID, name, balance, and
 * a collection of transactions. It exposes COM-style interfaces for
 * integration with other components.
 */
class Account : IUnknown
{
public:
	Account(double bal);
	~Account();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	HRESULT Command(const wchar_t* szCmd);
	HRESULT GetReturnString(IUnknown** iStr);
	HRESULT GetTransaction(UINT ndx, IUnknown** iTran);
	HRESULT AddTransaction();

	void ID(IUnknown** iVar);
	void HTMLPage(VLStringCollection& varList, wstring& inHTML);
	void Update();

private:
	int m_cRef;                         /**< COM reference count. */
	VariableCollection* m_pProperties;
	StringInf m_iRetStr;
	VLVariable* m_iAccountID;           /**< Account ID variable. */
	VLVariable* m_iName;                /**< Account name variable. */
	VLVariable* m_iOpeningBalance;     /**< Opening balance. */
	InterfaceCollectionInf m_iTransactions; /**< Collection of transactions. */
};


/**
 * @class Finance
 * @brief High-level finance manager providing HTML generation and testing utilities.
 *
 * This class exposes the IFINANCE COM interface and manages a single Account
 * instance. It can generate HTML pages and run internal unit tests.
 */	
class Finance : public FinanceInf::IFINANCE
{
public:
	Finance();
	~Finance();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();

	/**
	 * @brief Generates an HTML page based on input variables.
	 * @param szVarList Semicolon-separated key=value pairs.
	 * @param iHtml IUnknown pointer to a StringInf object receiving the HTML.
	 * @return S_OK on success.
	 */
	HRESULT __stdcall HTMLPage(const wchar_t* szVarList, IUnknown* iHtml);

	HRESULT __stdcall Command(const wchar_t* szCmd);
	HRESULT __stdcall GetReturnString(IUnknown** iStr);

	HRESULT __stdcall UnitTest();	

private:
	int m_cRef;                     /**< COM reference count. */
	Account* m_iAccount;            /**< Managed account instance. */
	static const wchar_t* HTML_TEMPLATE; /**< Base HTML template. */
	StringInf m_iRetStr;
};
