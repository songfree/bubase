// HwPlayer.h: interface for the CHwPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HWPLAYER_H__B491AAE1_6D79_405F_BBC1_F28CCF26DD66__INCLUDED_)
#define AFX_HWPLAYER_H__B491AAE1_6D79_405F_BBC1_F28CCF26DD66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#define CURRENTPOS -1
#define NORMAL -1
#define FAST -2
#define SPECIALFAST -3
#define SLOW -4
#define _OPENTYPE "Music Files(*.mp3;*.wav;*.cda)|*.mp3;*.wav;*.cda|Video Files(*.avi;*.asf;*.wmv)|*.avi;*.asf;*.wmv|Mpeg Video Files(*.dat;*.mpg;*.mpeg)|*.dat;*.mpg;*.mpeg;*.mpe|"
#define DRIVER_ROOT_LENGTH 2


class CHwPlayer  
{
public:
	bool isPlaying();
	BOOL Eject();
	BOOL CloseCDDoor();
	DWORD CDGetTrackLength(int Track, TCHAR pzStr[]);
	BOOL Play(int track, int Min, int Sec, bool looping, HWND hWnd);
	int GetTrackLength()
	{
		return m_nTrackLength;
	}
	
	int GetNumTracks()
	{
		return m_nMaxTrackNum;
	}

	int GetCurTrack ()
	{
		return m_nCurTrack;
	}
	
	BOOL IsPlayCD ()
	{
		return m_bIsPlayCD;
	}

	BOOL GetCDValidity()
	{
		return ( m_bIsPlayCD && m_bCDIsValid );
	}
	
	void SetCallBackWin(HWND hWnd)
	{
		m_hWnd = hWnd;
	}

	BOOL Open ( LPCTSTR lpszMedia );
	int GetLastErrorStr ( LPTSTR lpErrString, int nMaxCount );
	CString GetLastErrorStr ();
	LONG GetTotalSec();
	int GetFrameRate();
	BOOL SetFullScreen();
	BOOL SetVolume(int iFactor);
	BOOL IsVideo();
	HWND GetHwnd();
	LONG GetLength();
	BOOL PauseOrResume();
	BOOL Stop();
	BOOL Close();
	BOOL Play ( int iStartPos=CURRENTPOS );
	CHwPlayer();
	virtual ~CHwPlayer();

private:
	BOOL GetAudioDiskInfo();
	BOOL CheckValidity();
	BOOL OpenMediaFile ( LPCTSTR lpszMediaFile );
	BOOL OpenCD ( LPCTSTR lpszCDROM );
	BOOL mciSendString (
		LPTSTR lpszReturnString,  
		UINT cchReturn,       
		HWND hwndCallback,
		LPCTSTR lpszCommand, ...
	);
	BOOL mciSendCommand (
		MCIDEVICEID IDDevice,  
		UINT uMsg,             
		DWORD fdwCommand,      
		DWORD dwParam          
	);
	void DispatchMciError(MCIERROR mciError);
	void OutputError ( LPCTSTR lpszErr, ... );

	BOOL m_bIsPlayCD;
	CString m_csMedaiAlias;
	MCIDEVICEID m_wDeviceID;
	CString m_csLastError;
	CString m_strMedia;
	CString m_mediaSuffix;

	BOOL m_bInited;
	BOOL m_bPause;
	BOOL m_bPlaying;
	BOOL m_bIsVideo;
	BOOL m_bCDIsValid;
	int m_nMaxTrackNum;
	DWORD m_nTrackLength;
	int m_nCurTrack;
	HWND m_hWnd;
};

#endif // !defined(AFX_HWPLAYER_H__B491AAE1_6D79_405F_BBC1_F28CCF26DD66__INCLUDED_)
