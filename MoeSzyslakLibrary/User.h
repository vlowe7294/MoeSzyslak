#pragma once
#include "Utilities\Database.h"

/**
 * @class User
 * @brief Represents a system user with authentication, session state,
 *        property management, and COM-style lifetime semantics.
 *
 * The User class implements the IUSER COM interface and provides:
 *  - Login and authentication
 *  - Idle timeout tracking
 *  - Property bag access (name, password, admin flag, email, etc.)
 *  - Serialization to/from SQL tables
 *  - Participation in a global master user list
 *
 * A special internal constructor is used to create the first administrator
 * account during system initialization.
 */
class User : public IUSER
{
public:
	User(const wchar_t* szName, const wchar_t* szPwd, BOOL bIsAdmin);
	~User();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);

	/**
	 * @brief Attempts to authenticate the user
	 *
	 * If the login name and password match the internal name and password
	 * state is updated
	 *
	 * @return S_OK on success (even if login fails), or error codes.
	 */
	HRESULT __stdcall Login(const wchar_t* szName, const wchar_t* szPwd);

	/**
	 * @brief Executes a command string against the user object.
	 *
	 * Supported commands include:
	 *  - "login" : attempts to authenticate the user
	 *  - Other commands are forwarded to the property collection
	 *
	 * Also enforces idle timeout and updates last-active timestamp.
	 *
	 * @param szCmd The command string.
	 * @return HRESULT indicating success or failure.
	 */
	HRESULT __stdcall Command(const wchar_t* szCmd);

	/**
	 * @brief Retrieves the return string from the internal property collection.
	 *
	 * @param iStr Receives an IUnknown pointer to the return string object.
	 * @return HRESULT status.
	 */
	HRESULT __stdcall GetReturnString(IUnknown** iStr);

	/**
	 * @brief Returns the user's property collection as an IUnknown.
	 *
	 * @param iPrp Receives the property collection interface.
	 * @return HRESULT status.
	 */
	HRESULT __stdcall Properties(IUnknown** iPrp);

	/**
	 * @brief Saves all users in the master list to the provided database.
	 *
	 * Writes each user's properties into the "users" table.
	 *
	 * @param iDB Pointer to an IDATABASE interface.
	 * @return HRESULT status.
	 */
	HRESULT __stdcall SaveUserList(IUnknown* iDB);

	/**
	 * @brief Executes internal unit tests for the User class.
	 *
	 * Exercises database serialization, login behavior, and idle timeout.
	 *
	 * @return S_OK on completion.
	 */
	HRESULT __stdcall UnitTest();	

	/**
	 * @brief Adds a new user to the master list.
	 *
	 * Only permitted when:
	 *  - The current user is logged in
	 *  - The current user is an administrator
	 *
	 * @param szTag Login name of the new user.
	 * @param szPassword Password for the new user.
	 * @return S_OK on success, E_FAIL if unauthorized or user exists.
	 */
	HRESULT __stdcall AddToMasterList(const wchar_t* szTag, const wchar_t* szPassword);

	/**
	 * @brief Loads all users from the database into the master list.
	 *
	 * Clears the existing list and reconstructs User objects from the
	 * "users" table.
	 *
	 * @param db Reference to a Database object.
	 */
	HRESULT __stdcall LoadUserList(IUnknown* iDB);

	/**
	 * @brief Copies selected properties from another User.
	 *
	 * Copies password, admin flag, and email. Does not copy login name.
	 *
	 * @param iCopyFrom The source user.
	 */
	HRESULT __stdcall Copy(IUnknown* iCopyFrom);

	void EditMasterList(User& usr);

	/**
	 * @brief Gets the user logged in state
	 *
	 * Will set login state to FALSE, if more than 15 minutes have passed since last check	 
	 */
	BOOL IsLoggedIn();

	void Save(Table& tbl);
	

private:
	/**
	 * @brief Special constructor used only for creating the initial
	 *        administrator account during system initialization.
	 *
	 * @param bAdmin Whether the created user should be an administrator.
	 */
	User(bool bAdmin, const wchar_t* szName, const wchar_t* szPwd);

	int m_instance;                      ///< Instance ID for debugging.
	
	int m_cRef;                         ///< COM reference count.
	VariableCollection* m_pProperties;  ///< Property bag for all user fields.
	wstring m_name;		                ///< Login name.
	wstring m_password;			        ///< Password (plaintext).
	BOOL m_bIsLoggedIn;			        ///< Login state flag.
	BOOL m_bIsAdmin;					///< Administrator privilege flag.
	VLVariable* m_pEmail;               ///< Email address.
	MoeInf<IDATETIME, CLASSID::DATETIME> m_iLastActive; ///< Last activity timestamp.
	StringInf m_iReturnStr;             ///< Return string for property commands.

	static InterfaceCollection m_masterUserList; ///< Global list of all users.
	static BOOL m_bInit;                          ///< Tracks master list initialization.

	/**
	 * @brief Compares this user with another for authentication.
	 *
	 * Checks login name and password.
	 *
	 * @param usr The user to compare against.
	 * @return true if credentials match, false otherwise.
	 */
	bool Compare(User& usr);

	/**
	 * @brief Initializes the master user list and creates the first admin.
	 *
	 * Called automatically on first User construction.
	 */
	static void InitMasterList();
};

class UserService : public IUSERSERVICE
{
public:
	UserService();
	~UserService();
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID* ppvObj);
	HRESULT __stdcall GetUser(const wchar_t* szName, IUnknown** iUsr);
	HRESULT __stdcall UnitTest();
	

private:
	int m_cRef;
	InterfaceCollection m_users;

	void LoadUsers();
};
