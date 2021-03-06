// ----------------------------------------------------------------------------
//  File name: instsoft.cpp
//  Author: Lucas Suggs (lucas.suggs@gmail.com)
//
//  This program is intended to query the registry to determine what software
//  is installed.
// ----------------------------------------------------------------------------


// Preprocessor directives.
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#define SOFTWARE_LIST_KEY2	"Software\\Classes\\Installer\\Products"
#define SOFTWARE_LIST_KEY	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"

#define MAX_KEY_LENGTH			255
#define MAX_VALUE_LENGTH		500 * sizeof(TCHAR)
#define INSTALL_DATE_LENGTH		50
#define DISPLAY_NAME_LENGTH		500
#define VERSION_LENGTH			50
#define COMPUTER_NAME_LENGTH	50

#define VERSION_MAJOR	1
#define VERSION_MINOR	3


// Global declarations.
typedef struct SOFTWARE_DATA
{
	TCHAR	InstallDate[INSTALL_DATE_LENGTH];
	TCHAR	DisplayName[DISPLAY_NAME_LENGTH];
	TCHAR	DisplayVersion[VERSION_LENGTH];
} *PSOFTWARE_DATA;

typedef struct SOFTWARE_DATA_NODE
{
	SOFTWARE_DATA_NODE*	Previous;
	SOFTWARE_DATA		Data;
	SOFTWARE_DATA_NODE*	Next;
} *PSOFTWARE_DATA_NODE;

PSOFTWARE_DATA_NODE	g_pSoftwareListHead		= NULL;
PSOFTWARE_DATA_NODE	g_pSoftwareListTail		= NULL;

PSOFTWARE_DATA_NODE	g_pSoftwareListHead2	= NULL;
PSOFTWARE_DATA_NODE	g_pSoftwareListTail2	= NULL;

HANDLE	g_hProcessHeap	= NULL;
HKEY	g_hBaseKey		= HKEY_LOCAL_MACHINE;


// ----------------------------------------------------------------------------
//  Name: AddNodeToList
//
//  Desc: Adds a node to the software list.
// ----------------------------------------------------------------------------
void AddNodeToList( PSOFTWARE_DATA_NODE pNode )
{
	PSOFTWARE_DATA_NODE pCurrent;

	// If the list is empty, this is simple.
	if( NULL == g_pSoftwareListHead )
	{
		g_pSoftwareListHead = pNode;
		g_pSoftwareListTail = pNode;

		return;
	}

	pCurrent = g_pSoftwareListHead;

	while( pCurrent )
	{
		// If the current node in the list is greater than the node to add,
		// we add it to the point just before the current node.
		if( (CompareString( LOCALE_USER_DEFAULT,
							NORM_IGNORECASE,
							pNode->Data.DisplayName,
							DISPLAY_NAME_LENGTH,
							pCurrent->Data.DisplayName,
							DISPLAY_NAME_LENGTH ) == CSTR_EQUAL) ||
			(CompareString( LOCALE_USER_DEFAULT,
							NORM_IGNORECASE,
							pNode->Data.DisplayName,
							DISPLAY_NAME_LENGTH,
							pCurrent->Data.DisplayName,
							DISPLAY_NAME_LENGTH ) == CSTR_LESS_THAN) )
		{
			if( pCurrent == g_pSoftwareListHead )
			{
				// If the current node is the beginning, do special stuff.
				pNode->Next = pCurrent;
				pCurrent->Previous = pNode;
				g_pSoftwareListHead = pNode;
			}
			else
			{
				pNode->Previous = pCurrent->Previous;
				pNode->Next = pCurrent;
				pCurrent->Previous->Next = pNode;
				pCurrent->Previous = pNode;
			}

			return;
		}
		else
		{
			// If we reach the end and do not find a node that's greater than
			// the one to add, we add the new node at the very end.
			if( pCurrent == g_pSoftwareListTail )
			{
				pNode->Previous = pCurrent;
				pCurrent->Next = pNode;
				g_pSoftwareListTail = pNode;

				return;
			}
		}

		pCurrent = pCurrent->Next;
	}
}


// ----------------------------------------------------------------------------
//  Name: AddNodeToList2
//
//  Desc: Adds a node to the second software list.
// ----------------------------------------------------------------------------
void AddNodeToList2( PSOFTWARE_DATA_NODE pNode )
{
	PSOFTWARE_DATA_NODE pCurrent;

	// If the list is empty, this is simple.
	if( NULL == g_pSoftwareListHead2 )
	{
		g_pSoftwareListHead2 = pNode;
		g_pSoftwareListTail2 = pNode;

		return;
	}

	pCurrent = g_pSoftwareListHead2;

	while( pCurrent )
	{
		// If the current node in the list is greater than the node to add,
		// we add it to the point just before the current node.
		if( (CompareString( LOCALE_USER_DEFAULT,
							NORM_IGNORECASE,
							pNode->Data.DisplayName,
							DISPLAY_NAME_LENGTH,
							pCurrent->Data.DisplayName,
							DISPLAY_NAME_LENGTH ) == CSTR_EQUAL) ||
			(CompareString( LOCALE_USER_DEFAULT,
							NORM_IGNORECASE,
							pNode->Data.DisplayName,
							DISPLAY_NAME_LENGTH,
							pCurrent->Data.DisplayName,
							DISPLAY_NAME_LENGTH ) == CSTR_LESS_THAN) )
		{
			if( pCurrent == g_pSoftwareListHead2 )
			{
				// If the current node is the beginning, do special stuff.
				pNode->Next = pCurrent;
				pCurrent->Previous = pNode;
				g_pSoftwareListHead2 = pNode;
			}
			else
			{
				pNode->Previous = pCurrent->Previous;
				pNode->Next = pCurrent;
				pCurrent->Previous->Next = pNode;
				pCurrent->Previous = pNode;
			}

			return;
		}
		else
		{
			// If we reach the end and do not find a node that's greater than
			// the one to add, we add the new node at the very end.
			if( pCurrent == g_pSoftwareListTail2 )
			{
				pNode->Previous = pCurrent;
				pCurrent->Next = pNode;
				g_pSoftwareListTail2 = pNode;

				return;
			}
		}

		pCurrent = pCurrent->Next;
	}
}


// ----------------------------------------------------------------------------
//  Name: DisplaySoftwareList
//
//  Desc: Displays the content of the software list.
// ----------------------------------------------------------------------------
void DisplaySoftwareList( FILE* hFile )
{
	PSOFTWARE_DATA_NODE pCurrent;
	
	pCurrent = g_pSoftwareListHead;

	while( pCurrent )
	{
		_ftprintf( hFile,
				   TEXT("%-20s%s -- %s\n"),
				   pCurrent->Data.InstallDate,
				   pCurrent->Data.DisplayName,
				   pCurrent->Data.DisplayVersion );

		pCurrent = pCurrent->Next;
	}
}


// ----------------------------------------------------------------------------
//  Name: DestroySoftwareList
//
//  Desc: Frees the memory used by the software list.
// ----------------------------------------------------------------------------
void DestroySoftwareLists()
{
	PSOFTWARE_DATA_NODE pCurrent, pNext;

	pCurrent = g_pSoftwareListHead;

	while( pCurrent )
	{
		pNext = pCurrent->Next;

		HeapFree( g_hProcessHeap, NULL, pCurrent );

		pCurrent = pNext;
	}

	g_pSoftwareListHead = NULL;
	g_pSoftwareListTail = NULL;

	pCurrent = g_pSoftwareListHead2;

	while( pCurrent )
	{
		pNext = pCurrent->Next;

		HeapFree( g_hProcessHeap, NULL, pCurrent );

		pCurrent = pNext;
	}

	g_pSoftwareListHead2 = NULL;
	g_pSoftwareListTail2 = NULL;
}


// ----------------------------------------------------------------------------
//  Name: MergeLists
//
//  Desc: Merges the second software list into the first for a more
//        comprehensive list of installed software.
// ----------------------------------------------------------------------------
void MergeLists()
{
	PSOFTWARE_DATA_NODE pCurrent, pSecond;
	PSOFTWARE_DATA_NODE pNew;

	pCurrent = g_pSoftwareListHead2;
	
	while( pCurrent )
	{
		pSecond = g_pSoftwareListHead;

		while( pSecond )
		{
			if( CompareString( LOCALE_USER_DEFAULT,
							   NORM_IGNORECASE,
							   pSecond->Data.DisplayName,
							   DISPLAY_NAME_LENGTH,
							   pCurrent->Data.DisplayName,
							   DISPLAY_NAME_LENGTH ) == CSTR_EQUAL )
							   break;

			pSecond = pSecond->Next;
		}

		if( !pSecond )
		{
			// Create a new list node.
			pNew = (PSOFTWARE_DATA_NODE)HeapAlloc( g_hProcessHeap,
												   HEAP_ZERO_MEMORY,
												   sizeof(SOFTWARE_DATA_NODE) );
			if( NULL == pNew )
			{
				_ftprintf( stderr, TEXT("Out of memory.\n") );
			}
			else
			{
				StringCchCopy( pNew->Data.InstallDate, INSTALL_DATE_LENGTH, TEXT("N/A") );
				StringCchCopy( pNew->Data.DisplayName, INSTALL_DATE_LENGTH, pCurrent->Data.DisplayName );
				StringCchCopy( pNew->Data.DisplayVersion, INSTALL_DATE_LENGTH, TEXT("N/A") );

				AddNodeToList( pNew );
			}
		}

		pCurrent = pCurrent->Next;
	}
}


// ----------------------------------------------------------------------------
//  Name: QuerySubkey
//
//  Desc: Queries the information for a key and displays the installed
//        software information.
// ----------------------------------------------------------------------------
void QuerySubkey( TCHAR* sKey, DWORD nKeyLength )
{
	TCHAR sBaseKey[MAX_KEY_LENGTH];
	PTCHAR sValue;
	DWORD nValueSize = MAX_VALUE_LENGTH;
	HKEY hSubkey = NULL;
	HANDLE hProcessHeap = NULL;
	LONG result = ERROR_SUCCESS;
	PSOFTWARE_DATA_NODE pNew = NULL;

	// Allocate memory for the buffer to contain the value data.
	sValue = (PTCHAR)HeapAlloc( g_hProcessHeap,
								HEAP_ZERO_MEMORY,
								MAX_VALUE_LENGTH );
	if( NULL == sValue )
	{
		_ftprintf( stderr, TEXT("Out of memory.\n") );
		goto done;
	}

	// Create a new list node.
	pNew = (PSOFTWARE_DATA_NODE)HeapAlloc( g_hProcessHeap,
										   HEAP_ZERO_MEMORY,
										   sizeof(SOFTWARE_DATA_NODE) );
	if( NULL == pNew )
	{
		_ftprintf( stderr, TEXT("Out of memory.\n") );
		goto done;
	}

	// Set up the base key.
	StringCchCopy( sBaseKey, MAX_KEY_LENGTH, TEXT(SOFTWARE_LIST_KEY) );

	// Tack on the subkey to the base key.
	StringCchCat( sBaseKey, MAX_KEY_LENGTH, TEXT("\\") );
	StringCchCat( sBaseKey, MAX_KEY_LENGTH, sKey );

	// Open the specific software key.
	result = RegOpenKeyEx( g_hBaseKey,
						   sBaseKey,
						   0,
						   KEY_READ,
						   &hSubkey );
	if( ERROR_SUCCESS != result )
	{
		_ftprintf( stderr, TEXT("Unable to open the required registry key!\n") );
		goto done;
	}

	// Retrieve the InstallDate and DisplayName values and display them.
	result = RegQueryValueEx( hSubkey,
							  TEXT("InstallDate"),
							  NULL,
							  NULL,
							  (LPBYTE)sValue,
							  &nValueSize );
	if( ERROR_SUCCESS != result )
	{
		// If there is no InstallDate value, we don't want to fail, instead
		// we'll put that it's not available.
		StringCchCopy( pNew->Data.InstallDate, INSTALL_DATE_LENGTH, TEXT("N/A") );
		result = ERROR_SUCCESS;
	}
	else
	{
		StringCchCopy( pNew->Data.InstallDate, INSTALL_DATE_LENGTH, sValue );
	}

	nValueSize = MAX_VALUE_LENGTH;

	result = RegQueryValueEx( hSubkey,
							  TEXT("DisplayName"),
							  NULL,
							  NULL,
							  (LPBYTE)sValue,
							  &nValueSize );
	if( ERROR_SUCCESS != result ) goto done;

	StringCchCopy( pNew->Data.DisplayName, DISPLAY_NAME_LENGTH, sValue );

	nValueSize = MAX_VALUE_LENGTH;

	result = RegQueryValueEx( hSubkey,
							  TEXT("DisplayVersion"),
							  NULL,
							  NULL,
							  (LPBYTE)sValue,
							  &nValueSize );
	if( ERROR_SUCCESS != result )
	{
		// If there is no DisplayVersion value, we don't want to fail, instead
		// we'll put that it's not available.
		StringCchCopy( pNew->Data.DisplayVersion, VERSION_LENGTH, TEXT("N/A") );
		result = ERROR_SUCCESS;
	}
	else
	{
		StringCchCopy( pNew->Data.DisplayVersion, VERSION_LENGTH, sValue );
	}

	AddNodeToList( pNew );

done:
	if( ERROR_SUCCESS != result )
	{
		if( pNew ) HeapFree( g_hProcessHeap, NULL, pNew );
	}

	if( hSubkey ) RegCloseKey( hSubkey );
	if( sValue ) HeapFree( g_hProcessHeap, NULL, sValue );
}


// ----------------------------------------------------------------------------
//  Name: QuerySubkey2
//
//  Desc: Queries the information for a key and displays the installed
//        software information.
// ----------------------------------------------------------------------------
void QuerySubkey2( TCHAR* sKey, DWORD nKeyLength )
{
	TCHAR sBaseKey[MAX_KEY_LENGTH];
	PTCHAR sValue;
	DWORD nValueSize = MAX_VALUE_LENGTH;
	HKEY hSubkey = NULL;
	HANDLE hProcessHeap = NULL;
	LONG result = ERROR_SUCCESS;
	PSOFTWARE_DATA_NODE pNew = NULL;

	// Allocate memory for the buffer to contain the value data.
	sValue = (PTCHAR)HeapAlloc( g_hProcessHeap,
								HEAP_ZERO_MEMORY,
								MAX_VALUE_LENGTH );
	if( NULL == sValue )
	{
		_ftprintf( stderr, TEXT("Out of memory.\n") );
		goto done;
	}

	// Create a new list node.
	pNew = (PSOFTWARE_DATA_NODE)HeapAlloc( g_hProcessHeap,
										   HEAP_ZERO_MEMORY,
										   sizeof(SOFTWARE_DATA_NODE) );
	if( NULL == pNew )
	{
		_ftprintf( stderr, TEXT("Out of memory.\n") );
		goto done;
	}

	// Set up the base key.
	StringCchCopy( sBaseKey, MAX_KEY_LENGTH, TEXT(SOFTWARE_LIST_KEY2) );

	// Tack on the subkey to the base key.
	StringCchCat( sBaseKey, MAX_KEY_LENGTH, TEXT("\\") );
	StringCchCat( sBaseKey, MAX_KEY_LENGTH, sKey );

	// Open the specific software key.
	result = RegOpenKeyEx( g_hBaseKey,
						   sBaseKey,
						   0,
						   KEY_READ,
						   &hSubkey );
	if( ERROR_SUCCESS != result )
	{
		_ftprintf( stderr, TEXT("Unable to open the required registry key!\n") );
		goto done;
	}

	StringCchCopy( pNew->Data.InstallDate, INSTALL_DATE_LENGTH, TEXT("N/A") );

	nValueSize = MAX_VALUE_LENGTH;

	result = RegQueryValueEx( hSubkey,
							  TEXT("ProductName"),
							  NULL,
							  NULL,
							  (LPBYTE)sValue,
							  &nValueSize );
	if( ERROR_SUCCESS != result ) goto done;

	StringCchCopy( pNew->Data.DisplayName, DISPLAY_NAME_LENGTH, sValue );

	nValueSize = MAX_VALUE_LENGTH;

	StringCchCopy( pNew->Data.DisplayVersion, VERSION_LENGTH, TEXT("N/A") );

	AddNodeToList2( pNew );

done:
	if( ERROR_SUCCESS != result )
	{
		if( pNew ) HeapFree( g_hProcessHeap, NULL, pNew );
	}

	if( hSubkey ) RegCloseKey( hSubkey );
	if( sValue ) HeapFree( g_hProcessHeap, NULL, sValue );
}


// ----------------------------------------------------------------------------
//  Name: _tmain
//
//  Desc: Application entry point.
// ----------------------------------------------------------------------------
int _tmain( int argc, TCHAR** argv )
{
	TCHAR sComputerName[COMPUTER_NAME_LENGTH];
	TCHAR* sSubkeyName = NULL;
	TCHAR sFilename[MAX_PATH];
	TCHAR sPath[MAX_PATH];
	TCHAR sTime[50];
	TCHAR sDate[50];
	DWORD nComputerNameSize = COMPUTER_NAME_LENGTH;
	DWORD nSubkeyNameSize;
	DWORD nNumberOfSubkeys;
	HKEY hSoftwareListKey = NULL;
	LONG result = ERROR_SUCCESS;
	BOOL bRemoteComputer = FALSE;
	BOOL bPrintToFile = FALSE;
	FILE* hFile = stdout;
	SYSTEMTIME tDateTime;

	if( argc > 1 )
	{
		if( CompareString( LOCALE_USER_DEFAULT,
						   NORM_IGNORECASE,
						   argv[1],
						   2,
						   TEXT("/?"),
						   2 ) == CSTR_EQUAL )
		{
			_tprintf( TEXT("instsoft version %d.%d, Copyright (c) 2011, Lucas M. Suggs\n"), VERSION_MAJOR, VERSION_MINOR );
			_tprintf( TEXT("Usage: %s [/f path] [computername]\n"), argv[0] );

			return 0;
		}
	}

	if( argc > 1 )
	{
		if( argc > 2 )
		{
			if( CompareString( LOCALE_USER_DEFAULT,
							   NORM_IGNORECASE,
							   argv[1],
							   2,
							   TEXT("/f"),
							   2 ) == CSTR_EQUAL)
			{
				bPrintToFile = TRUE;

				StringCchCopy( sPath, MAX_PATH, argv[2] );

				if( argc > 3 )
				{
					StringCchCopy( sComputerName, nComputerNameSize, argv[3] );
					bRemoteComputer = TRUE;
				}
				else
				{
					// Get the computer name.
					GetComputerName( sComputerName, &nComputerNameSize );
				}
			}
			else
			{
				StringCchCopy( sComputerName, nComputerNameSize, argv[1] );
				bRemoteComputer = TRUE;
			}
		}
		else
		{
			StringCchCopy( sComputerName, nComputerNameSize, argv[1] );
			bRemoteComputer = TRUE;
		}
	}
	else
	{
		// Get the computer name.
		GetComputerName( sComputerName, &nComputerNameSize );
	}

	if( bRemoteComputer )
	{
		// Connect to the registry on the remote computer.
		result = RegConnectRegistry( sComputerName,
									 HKEY_LOCAL_MACHINE,
									 &g_hBaseKey );
		if( ERROR_SUCCESS != result )
		{
			_ftprintf( stderr,
					   TEXT("Failed to connect to the remote registry on %s\n"),
					   sComputerName );

			return -1;
		}
	}

	// Open the appropriate registry key to enumerate the list of installed
	// software.
	result = RegOpenKeyEx( g_hBaseKey,
						   TEXT(SOFTWARE_LIST_KEY),
						   0,
						   KEY_READ,
						   &hSoftwareListKey );
	if( ERROR_SUCCESS != result )
	{
		_ftprintf( stderr, TEXT("Unable to open the required registry key!\n") );
		goto done;
	}

	// Query the information about this key to get the number of subkeys
	// and the length of the longest-named subkey.
	result = RegQueryInfoKey( hSoftwareListKey,
							  NULL,
							  NULL,
							  NULL,
							  &nNumberOfSubkeys,
							  &nSubkeyNameSize,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL );
	if( ERROR_SUCCESS != result )
	{
		_ftprintf( stderr, TEXT("Unable to query information about the key, %S\n"), TEXT(SOFTWARE_LIST_KEY) );
		goto done;
	}

	// Get a handle to the process heap and allocate memory for the subkey
	// name buffer.
	g_hProcessHeap = GetProcessHeap();
	if( NULL == g_hProcessHeap )
	{
		_ftprintf( stderr, TEXT("Failed to get the process heap handle.\n") );
		goto done;
	}

	sSubkeyName = (TCHAR*)HeapAlloc( g_hProcessHeap,
									 HEAP_ZERO_MEMORY,
									 sizeof(TCHAR) * (nSubkeyNameSize + 1) );
	if( NULL == sSubkeyName )
	{
		_ftprintf( stderr, TEXT("Out of memory error when creating sSubkeyName.\n") );
		goto done;
	}

	// Enumerate the list of subkeys and retrieve the software information
	// from each one.
	for( DWORD i = 0; i < nNumberOfSubkeys; i++ )
	{
		nSubkeyNameSize = MAX_KEY_LENGTH;

		result = RegEnumKeyEx( hSoftwareListKey,
							   i,
							   sSubkeyName,
							   &nSubkeyNameSize,
							   NULL,
							   NULL,
							   NULL,
							   NULL );
		if( ERROR_SUCCESS == result )
		{
			QuerySubkey( sSubkeyName, nSubkeyNameSize );
		}
	}

	// Open the appropriate registry key to enumerate the list of installed
	// software.
	result = RegOpenKeyEx( g_hBaseKey,
						   TEXT(SOFTWARE_LIST_KEY2),
						   0,
						   KEY_READ,
						   &hSoftwareListKey );
	if( ERROR_SUCCESS != result )
	{
		_ftprintf( stderr, TEXT("Unable to open the required registry key!\n") );
		goto done;
	}

	// Query the information about this key to get the number of subkeys
	// and the length of the longest-named subkey.
	result = RegQueryInfoKey( hSoftwareListKey,
							  NULL,
							  NULL,
							  NULL,
							  &nNumberOfSubkeys,
							  &nSubkeyNameSize,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL,
							  NULL );
	if( ERROR_SUCCESS != result )
	{
		_ftprintf( stderr, TEXT("Unable to query information about the key, %S\n"), TEXT(SOFTWARE_LIST_KEY2) );
		goto done;
	}

	// Get a handle to the process heap and allocate memory for the subkey
	// name buffer.
	g_hProcessHeap = GetProcessHeap();
	if( NULL == g_hProcessHeap )
	{
		_ftprintf( stderr, TEXT("Failed to get the process heap handle.\n") );
		goto done;
	}

	sSubkeyName = (TCHAR*)HeapAlloc( g_hProcessHeap,
									 HEAP_ZERO_MEMORY,
									 sizeof(TCHAR) * (nSubkeyNameSize + 1) );
	if( NULL == sSubkeyName )
	{
		_ftprintf( stderr, TEXT("Out of memory error when creating sSubkeyName.\n") );
		goto done;
	}

	// Enumerate the list of subkeys and retrieve the software information
	// from each one.
	for( DWORD i = 0; i < nNumberOfSubkeys; i++ )
	{
		nSubkeyNameSize = MAX_KEY_LENGTH;

		result = RegEnumKeyEx( hSoftwareListKey,
							   i,
							   sSubkeyName,
							   &nSubkeyNameSize,
							   NULL,
							   NULL,
							   NULL,
							   NULL );
		if( ERROR_SUCCESS == result )
		{
			QuerySubkey2( sSubkeyName, nSubkeyNameSize );
		}
	}

	MergeLists();

	// If we are outputting to a file, open it now.
	if( bPrintToFile )
	{
		GetLocalTime( &tDateTime );

		GetTimeFormat( LOCALE_USER_DEFAULT,
					   TIME_FORCE24HOURFORMAT,
					   &tDateTime,
					   TEXT("HHmmss"),
					   sTime,
					   50 );

		GetDateFormat( LOCALE_USER_DEFAULT,
					   0,
					   &tDateTime,
					   TEXT("MMddyyyy"),
					   sDate,
					   50 );

		StringCchCopy( sFilename, MAX_PATH, sPath );
		StringCchCat( sFilename, MAX_PATH, TEXT("\\") );
		StringCchCat( sFilename, MAX_PATH, sComputerName );
		StringCchCat( sFilename, MAX_PATH, TEXT("_") );
		StringCchCat( sFilename, MAX_PATH, sDate );
		StringCchCat( sFilename, MAX_PATH, TEXT("-") );
		StringCchCat( sFilename, MAX_PATH, sTime );
		StringCchCat( sFilename, MAX_PATH, TEXT(".txt") );

		_tprintf( TEXT("%s\n"), sFilename );

		_tfopen_s( &hFile, sFilename, TEXT("w") );
		if( !hFile )
		{
			_ftprintf( stderr, TEXT("Unable to open output file for writing: %s\n"), sFilename );
			goto done;
		}
	}

	// Display the table header.
	_ftprintf( hFile, TEXT("Computer name: %s\n"), sComputerName );
	_ftprintf( hFile, TEXT("------------------------------------\n\n") );
	_ftprintf( hFile, TEXT("%-20sProgram Name\n\n"), TEXT("Install Date") );

	DisplaySoftwareList( hFile );

	if( bPrintToFile )
	{
		fclose( hFile );
	}

done:
	DestroySoftwareLists();

	if( sSubkeyName ) HeapFree( g_hProcessHeap, NULL, sSubkeyName );
	if( hSoftwareListKey ) RegCloseKey( hSoftwareListKey );

	return result;
}
