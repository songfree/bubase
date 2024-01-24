// HwPlayer.cpp: implementation of the CHwPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "HwPlayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma comment(lib, "winmm.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHwPlayer::CHwPlayer()
	: m_bIsVideo ( FALSE )
	, m_bPause ( FALSE )
	, m_wDeviceID ( 0 )
	, m_bInited ( FALSE )
	, m_bCDIsValid ( FALSE )
	, m_nMaxTrackNum ( 0 )
	, m_hWnd ( NULL )
	, m_nTrackLength ( 0 )
	, m_bIsPlayCD ( FALSE )
	, m_bPlaying ( FALSE )
	, m_nCurTrack ( -1 )
{
	static int nAliasCount = 0;
	m_csMedaiAlias.Format ( "hwMedia%u", nAliasCount++ );
}

CHwPlayer::~CHwPlayer()
{
	Stop ();
	Close ();
}

void CHwPlayer::DispatchMciError ( MCIERROR mciError )
{
	if ( mciError )		//get error
	{
		char chError[100];
		mciGetErrorString ( mciError,chError,lstrlen(chError) );
		#ifdef _DEBUG
		AfxMessageBox ( chError );
		#endif
		m_csLastError = chError;
	}
}

void CHwPlayer::OutputError ( LPCTSTR lpszErr, ... )
{
	char szErr[256] = {0};
	va_list args;
	va_start( args, lpszErr );
	_vsnprintf( szErr, sizeof(szErr)-1, lpszErr, args);
	va_end( args );

	#ifdef _DEBUG
	AfxMessageBox ( szErr );
	#endif
	m_csLastError = szErr;
}

BOOL CHwPlayer::Open ( LPCTSTR lpszMedia )
{
	if ( !lpszMedia || strlen(lpszMedia) < DRIVER_ROOT_LENGTH )
		return FALSE;

	if ( m_bInited )
		Stop ();
	m_bInited = FALSE;

	if ( strlen(lpszMedia) == DRIVER_ROOT_LENGTH && lpszMedia[1] == ':' )
		return OpenCD ( lpszMedia );
	else
		return OpenMediaFile ( lpszMedia );

	return TRUE;
}

BOOL CHwPlayer::OpenCD ( LPCTSTR lpszCDROM )
{
	MCI_OPEN_PARMS	mciOpenParms;
    MCI_SET_PARMS	mciSetParms;

	ASSERT ( lpszCDROM && strlen(lpszCDROM) == DRIVER_ROOT_LENGTH );
	ASSERT ( lpszCDROM[1] == ':' );

	mciOpenParms.lpstrDeviceType = "cdaudio";
	mciOpenParms.lpstrElementName = lpszCDROM;
	mciOpenParms.lpstrAlias = m_csMedaiAlias;
	if ( !mciSendCommand ( NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_ALIAS | MCI_OPEN_TYPE, (DWORD) (LPVOID) &mciOpenParms ) )
	{
		return FALSE;
	}
	m_wDeviceID = mciOpenParms.wDeviceID;
	m_bInited = TRUE;

    // Set the time format to track/minute/second/frame (TMSF).
    mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
    if ( !mciSendCommand ( m_wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms ) )
    {
		Close ();
		return FALSE;
	}

	if ( !GetAudioDiskInfo() ) return FALSE;
	m_bIsPlayCD = TRUE;

	return TRUE;
}

BOOL CHwPlayer::OpenMediaFile ( LPCTSTR lpszMediaFile )
{
	m_strMedia = lpszMediaFile;
	int iIndex = m_strMedia.ReverseFind('.');
	if ( iIndex >= 0 )
	{
		m_mediaSuffix = m_strMedia.Mid ( iIndex+1, 3 );	// get the file's suffix
		m_mediaSuffix.MakeLower();
	}

	MCI_OPEN_PARMS mciOpenParms;
	if ( m_mediaSuffix == "avi" )
		mciOpenParms.lpstrDeviceType = "avivideo";
	else if ( m_mediaSuffix == "wav" )
		mciOpenParms.lpstrDeviceType = "waveaudio";
	else if ( m_mediaSuffix == "cda" )
		mciOpenParms.lpstrDeviceType = "cdaudio";
	else
		mciOpenParms.lpstrDeviceType = "MPEGvideo2";
		
	mciOpenParms.lpstrElementName = m_strMedia;
	mciOpenParms.lpstrAlias = m_csMedaiAlias;
	if ( !mciSendCommand ( NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_ALIAS | MCI_OPEN_TYPE , (DWORD_PTR)&mciOpenParms ) )
		return FALSE;
	m_wDeviceID = mciOpenParms.wDeviceID;

	if ( m_mediaSuffix == "mp3" || m_mediaSuffix=="wav" || m_mediaSuffix=="cda" )
		m_bIsVideo = FALSE;
	else
		m_bIsVideo=TRUE;

	m_bIsPlayCD = FALSE;
	m_bInited = TRUE;

	return TRUE;
}

BOOL CHwPlayer::Play(int iStartPos/*=CURRENTPOS*/)
{
	ASSERT ( iStartPos >= CURRENTPOS );
	BOOL bRet;
	if ( iStartPos == CURRENTPOS )
	{
		bRet = mciSendString ( NULL, 0, NULL, "play %s", m_csMedaiAlias );
	}
	else
	{
		bRet = mciSendString ( NULL, 0, NULL, "play %s from %d", m_csMedaiAlias, iStartPos );
	}

	if ( bRet )
	{
		m_bPause = FALSE;
		m_bPlaying = TRUE;
	}
	return bRet;
}

BOOL CHwPlayer::Close()
{
	m_bPause = FALSE;
	m_bPlaying = FALSE;
	if ( !m_bInited ) return TRUE;
	if ( !mciSendString ( NULL, 0, NULL, "close %s", m_csMedaiAlias ) )
		return FALSE;
	m_bInited = FALSE;
	m_wDeviceID = 0;

	return TRUE;
}

BOOL CHwPlayer::Stop()
{
	m_bPause = FALSE;
	m_bPlaying = FALSE;
	if ( !m_bInited ) return FALSE;
	if ( !mciSendString ( NULL, 0, NULL, "stop %s", m_csMedaiAlias ) )
		return FALSE;
//	if ( !mciSendString ( NULL, 0, NULL, "seek %s to start", m_csMedaiAlias ) )
//		return FALSE;
	return TRUE;
}

BOOL CHwPlayer::mciSendString (
		LPTSTR lpszReturnString,  
		UINT cchReturn,       
		HWND hwndCallback,
		LPCTSTR lpszCommand, ...
	)
{
	char szCmd[256] = {0};
	va_list args;
	va_start( args, lpszCommand );
	_vsnprintf( szCmd, sizeof(szCmd)-1, lpszCommand, args);
	va_end( args );
	MCIERROR mciError = ::mciSendString ( szCmd, lpszReturnString, cchReturn, hwndCallback );
	if ( mciError )
	{
		DispatchMciError ( mciError );
		return FALSE;
	}

	return TRUE;
}

BOOL CHwPlayer::mciSendCommand (
		MCIDEVICEID IDDevice,  
		UINT uMsg,             
		DWORD fdwCommand,      
		DWORD dwParam          
	)
{
	MCIERROR mciError = ::mciSendCommand ( IDDevice, uMsg, fdwCommand, dwParam );
	if ( mciError )
	{
		DispatchMciError ( mciError );
		return FALSE;
	}

	m_csLastError = "No Error";

	return TRUE;
}

BOOL CHwPlayer::PauseOrResume()
{
	if ( !m_bPlaying ) return FALSE;
	BOOL bRet = FALSE;
	if ( m_bPause )
		bRet = mciSendString ( NULL, 0, NULL, "resume %s", m_csMedaiAlias );	
	else
		bRet = mciSendString ( NULL, 0, NULL, "pause %s", m_csMedaiAlias );
	m_bPause = !m_bPause;

	return bRet;
}

LONG CHwPlayer::GetLength()
{
	char chLength[50];
	if ( !m_bIsVideo )	//music file ,get total length in milliseconds
	{
		if ( !mciSendString ( NULL,0,NULL, "set %s time format milliseconds", m_csMedaiAlias ) )
			return -1;	// can not get length: ASF file can not get length
	}
	else //video file ,get total frames
	{
		if ( !mciSendString ( NULL,0,NULL, "set %s time format frames", m_csMedaiAlias ) )
			return -1;	// can not get length: ASF file can not get length
	}
	
	if ( !mciSendString ( chLength,sizeof(chLength),NULL, "status %s length", m_csMedaiAlias ) )
		return -1;	// can not get length: ASF file can not get length

	CString strLength;
	strLength.Format("%s",chLength);
	return (LONG)(atoi(strLength) );
}

//
// Get the handle of the VIDEO play window.
// this function only for Video media who has a play window
//
HWND CHwPlayer::GetHwnd()
{
	if ( !IsVideo() )	//music media
		return NULL;
	
	char chHandle[100];
	if ( !mciSendString ( chHandle,sizeof(chHandle),NULL, "status %s window handle", m_csMedaiAlias ) )
		return NULL;
	CString strHandle;
	strHandle.Format("%s",chHandle);
	return (HWND)(atoi((LPCTSTR)strHandle));
}

BOOL CHwPlayer::IsVideo()
{
	return m_bIsVideo;
}

BOOL CHwPlayer::SetVolume(int iFactor)
{
	//??? 不知道如何做
	ShellExecute ( NULL, "open", "sndvol32.exe", NULL, NULL, SW_SHOWNORMAL );	//u
	return FALSE;
}

BOOL CHwPlayer::SetFullScreen()
{
	if ( !m_bIsVideo )			//not video file
		return FALSE;
	if ( !mciSendString ( NULL,0,NULL, "window %s state minimized", m_csMedaiAlias ) )
		return FALSE;
	if ( !mciSendString ( NULL,0,NULL, "play %s fullscreen", m_csMedaiAlias ) )
		return FALSE;
	return TRUE;
}

int CHwPlayer::GetFrameRate()
{
	ASSERT ( m_bIsVideo );
	char chFrameRate[50];
	if ( !mciSendString ( chFrameRate,sizeof(chFrameRate),NULL, "status %s frame rate", m_csMedaiAlias ) )
		return -1;

	CString strFrameRate;
	strFrameRate.Format ( "%s",chFrameRate );
	int temp = atoi((LPCTSTR)strFrameRate)/1000;
	return temp;
}

//
// return media file's total length in seconds
//
LONG CHwPlayer::GetTotalSec()
{
	if ( m_bIsVideo )
	{
		int frameRate = GetFrameRate();
		if ( frameRate >= 0 )
			return GetLength() / frameRate;
		else		//asf file can not get framerate
			return 3600; //an hour
	}
	else
		return GetLength()/1000;
}

CString CHwPlayer::GetLastErrorStr()
{
	return m_csLastError;
}

int CHwPlayer::GetLastErrorStr(LPTSTR lpErrString, int nMaxCount)
{
	if ( lpErrString )
	{
		strncpy ( lpErrString, m_csLastError, nMaxCount );
		return m_csLastError.GetLength();
	}

	return -1;
}

BOOL CHwPlayer::GetAudioDiskInfo()
{
	MCI_STATUS_PARMS	mciStatusParms;
	m_bCDIsValid = FALSE;

	mciStatusParms.dwItem = MCI_STATUS_READY;
    if ( !mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD) (LPVOID) &mciStatusParms) )
	{
		goto failed;
	}
	if ( !mciStatusParms.dwReturn )
	{
		OutputError ( "Get CD information failed" );
		goto failed;
	}

	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
    if ( !mciSendCommand ( m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD) (LPVOID) &mciStatusParms ) )
	{
		goto failed;
	}

	if ( mciStatusParms.dwReturn < 1 )
	{
		OutputError ( "Get CD track failed" );
		goto failed;
	}

	m_nMaxTrackNum = mciStatusParms.dwReturn;
	if ( !CheckValidity () ) goto failed;

	return TRUE;

failed:
	Close ();
	return FALSE;

}

BOOL CHwPlayer::CheckValidity()
{
	MCI_STATUS_PARMS	mciStatusParms;
	mciStatusParms.dwItem = MCI_CDA_STATUS_TYPE_TRACK;
	mciStatusParms.dwTrack = 1;
    if ( !mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD) (LPVOID) &mciStatusParms) )
		return FALSE;
	if ( mciStatusParms.dwReturn != MCI_CDA_TRACK_AUDIO )
		return FALSE;
	m_bCDIsValid = TRUE;

	return TRUE;
}

BOOL CHwPlayer::Play(int track, int Min, int Sec, bool looping, HWND hWnd)
{
	MCI_PLAY_PARMS		mciPlayParms;
	MCI_STATUS_PARMS	mciStatusParms;
	if ( track <= 0 || track > m_nMaxTrackNum )
	{
		OutputError ( "Track %d is error", track );
		return FALSE;
	}
	m_nCurTrack = track;
	
	if ( !m_bInited )
	{
		m_csLastError = "CD-ROM not yet initialized";
		return FALSE;
	}

	SetCallBackWin ( hWnd );
	if ( !GetAudioDiskInfo() ) return FALSE;
	
	if ( !m_bCDIsValid )
	{
		if ( !GetAudioDiskInfo() ) return FALSE;
		if (!m_bCDIsValid) return FALSE;
		{
			m_csLastError = "Invalid CD";
			return FALSE;
		}
	}

	// don't try to play a non-audio track
	mciStatusParms.dwItem = MCI_CDA_STATUS_TYPE_TRACK;
	mciStatusParms.dwTrack = m_nCurTrack;
    if ( !mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD) (LPVOID) &mciStatusParms) )
		return FALSE;
	if (mciStatusParms.dwReturn != MCI_CDA_TRACK_AUDIO )
		return FALSE;

	// get the length of the track to be played
	mciStatusParms.dwItem = MCI_STATUS_LENGTH;
	mciStatusParms.dwTrack = m_nCurTrack;
    if ( !mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD) (LPVOID) &mciStatusParms) )
		return FALSE;
	m_nTrackLength = mciStatusParms.dwReturn;

    mciPlayParms.dwFrom = MCI_MAKE_TMSF(m_nCurTrack, Min, Sec, 0);
	mciPlayParms.dwTo = (mciStatusParms.dwReturn << 8) | m_nCurTrack;
    mciPlayParms.dwCallback = (DWORD)hWnd;
    if ( !mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM | MCI_TO, (DWORD)(LPVOID) &mciPlayParms) )
		return FALSE;

	m_bPlaying = TRUE;
	m_bPause = FALSE;

	return TRUE;
}

DWORD CHwPlayer::CDGetTrackLength(int Track, TCHAR pzStr[])
{
	MCI_SET_PARMS		sMCISet;
	MCI_STATUS_PARMS	sMCIStatus;

	sMCISet.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	if ( !mciSendCommand (m_wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD) (LPVOID) &sMCISet) )
		return 0;

	sMCIStatus.dwItem = MCI_STATUS_LENGTH;
	sMCIStatus.dwTrack = Track;
	if ( !mciSendCommand (m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK | MCI_WAIT, (DWORD) (LPVOID) &sMCIStatus) )
		return FALSE;

	sMCIStatus.dwReturn /= 1000;

	int pzMin = sMCIStatus.dwReturn/60;
	int pzSec = sMCIStatus.dwReturn%60;

	if(pzSec < 10)
	{
		wsprintf(pzStr, "Time:  %d:0%d", pzMin, pzSec);
	}
	else
	{
		wsprintf(pzStr, "Time:  %d:%d", pzMin, pzSec);
	}

	sMCISet.dwTimeFormat = MCI_FORMAT_TMSF;
	if ( !mciSendCommand (m_wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD) (LPVOID) &sMCISet) )
		return FALSE;

	return sMCIStatus.dwReturn;
}

BOOL CHwPlayer::CloseCDDoor()
{
	if ( !m_bInited ) return FALSE;
	if ( !m_bIsPlayCD ) return FALSE;
	return mciSendCommand(m_wDeviceID, MCI_SET, MCI_SET_DOOR_CLOSED, (DWORD)NULL);
}

BOOL CHwPlayer::Eject()
{
	if ( !m_bInited ) return FALSE;
	if ( !m_bIsPlayCD ) return FALSE;
	return mciSendCommand(m_wDeviceID, MCI_SET, MCI_SET_DOOR_OPEN, (DWORD)NULL);
}


bool CHwPlayer::isPlaying()
{
	return m_bPlaying;
}
