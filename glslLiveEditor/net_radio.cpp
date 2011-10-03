#include "net_radio.h"
#include "bass.h"
#include <process.h>
#include <string.h>
#include <stdio.h>

static HWND win=NULL;
static DWORD cthread=0;
static HSTREAM chan=NULL;

static char proxy[100] = "";

static void error(const char* errorString) {
	// TODO: do something to let the user know about the error
}

static void doMeta() {
	const char *meta = BASS_ChannelGetTags(chan, BASS_TAG_META);
	if (meta) {
		const char *p = strstr(meta, "StreamTitle='");
		if (p) {
			char *p2 = _strdup(p + 13);
			strchr(p2, ';')[-1] = 0;
			// TODO: do something to report p2 to the user
			free(p2);
		}
	} else {
		meta = BASS_ChannelGetTags(chan, BASS_TAG_OGG);
		if (meta) {
			const char *artist=NULL, *title=NULL, *p=meta;
			for (;*p;p+=strlen(p)+1) {
				if (!_strnicmp(p,"artist=",7)) // found the artist
					artist=p+7;
				if (!_strnicmp(p,"title=",6)) // found the title
					title=p+6;
			}
			if (artist) {
				char text[100];
				_snprintf_s(text,sizeof(text),"%s - %s",artist,title);
				// TODO: report text to user
			} else if (title) {
				// TODO: report title to user
			}
		}
	}
}

static void CALLBACK metaSync(HSYNC handle, DWORD channel, DWORD data, void *user) {
	doMeta();
}

static void CALLBACK endSync(HSYNC handle, DWORD channel, DWORD data, void *user) {
	// TODO: tell the user the music has stopped playing
}

static void CALLBACK statusProc(const void *buffer, DWORD length, void *user) {
	if (buffer && !length) {
		// TODO: buffer contains some text, do something with it to let the user know the status
	}
}

static void __cdecl openUrl(char* url) {
	if (chan != NULL) {
		BASS_StreamFree(chan);
	}
	chan = BASS_StreamCreateURL(url, 0, BASS_STREAM_BLOCK|BASS_STREAM_STATUS|BASS_STREAM_AUTOFREE, statusProc, 0);
	free(url);
	if (!chan) {
		// TODO: tell the user failed to open stream
	} else {

	}
	cthread = 0;
}

void net_radio_init(HWND hwnd) {
	// check the correct BASS was loaded
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		error("An incorrect version of BASS.DLL was loaded");
		return;
	}
	win = hwnd;
	// setup output device
	if (!BASS_Init(-1,44100,0,win,NULL)) {
		error("Can't initialize device");
		DestroyWindow(win);
	}
	BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST,1); // enable playlist processing
	BASS_SetConfig(BASS_CONFIG_NET_PREBUF,0); // minimize automatic pre-buffering, so we can do it (and display it) instead
	BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY,proxy); // setup proxy server location
}

void net_radio_openUrl(const char* url) {
	if (cthread) {
		MessageBeep(0);
		return;
	}
	char *temp;
	temp = _strdup(url);
	cthread = _beginthread((void (_cdecl*)(void*))openUrl, 0, temp);
}

// returns true when buffering is done
bool net_radio_updateBuffering() {
	QWORD progress=BASS_StreamGetFilePosition(chan,BASS_FILEPOS_BUFFER)
		*100/BASS_StreamGetFilePosition(chan,BASS_FILEPOS_END); // percentage of buffer filled
	if (progress>75 || !BASS_StreamGetFilePosition(chan,BASS_FILEPOS_CONNECTED)) { // over 75% full (or end of download)
		KillTimer(win,0); // finished prebuffering, stop monitoring
		{ // get the broadcast name and URL
			const char *icy=BASS_ChannelGetTags(chan,BASS_TAG_ICY);
			if (!icy) icy=BASS_ChannelGetTags(chan,BASS_TAG_HTTP); // no ICY tags, try HTTP
			if (icy) {
				for (;*icy;icy+=strlen(icy)+1) {
					if (!_strnicmp(icy,"icy-name:",9)) {
						//MESS(31,WM_SETTEXT,0,icy+9);
						// TODO pass this text to user
					}
					if (!_strnicmp(icy,"icy-url:",8)) {
						//MESS(32,WM_SETTEXT,0,icy+8);
						// TODO pass this text to user
					}
				}
			} else {
				//MESS(31,WM_SETTEXT,0,"");
				// TODO pass this text to user
			}
		}
		// get the stream title and set sync for subsequent titles
		doMeta();
		BASS_ChannelSetSync(chan,BASS_SYNC_META,0,&metaSync,0); // Shoutcast
		BASS_ChannelSetSync(chan,BASS_SYNC_OGG_CHANGE,0,&metaSync,0); // Icecast/OGG
		// set sync for end of stream
		BASS_ChannelSetSync(chan,BASS_SYNC_END,0,&endSync,0);
		// play it!
		BASS_ChannelPlay(chan,FALSE);
		// done buffering, so return true
		return true;
	} else {
		char text[20];
		sprintf_s(text,"buffering... %d%%",progress);
		// TODO tell the user 'text' how much has buffered

		// not finished buffering, so return false
		return false;
	}
}

void net_radio_free() {
	BASS_Free();
}

void net_radio_getFftData1024Floats(float* data) {
	BASS_ChannelGetData(chan, data, BASS_DATA_FFT2048);
}
