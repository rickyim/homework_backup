/*reference 
//https://gist.github.com/kbjorklu/6317308 
//course material of mmt@ bb.ustc.edu.cn*/
#include "stdafx.h"
#include <Windows.h>
#include <string>
#include<mmsystem.h>
#pragma comment(lib, "Winmm.lib")
using namespace std;
int main() {
	LPWSTR lp = _T("E:\\courses\\vocal\\exp_2\\test_16.wav");
	MMCKINFO MMCKInfoParent;
	MMCKINFO MMCKInfoChild;
	HMMIO hmmio = mmioOpen(lp, NULL, MMIO_READ | MMIO_ALLOCBUF);
	if (!hmmio) {
		cout << "cannot open" << endl;
		return 0;
	}
	MMCKInfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmioDescend(hmmio, &MMCKInfoParent, NULL, MMIO_FINDRIFF);
	cout << "RIFF chunck " << endl;
	cout << "RIFF ckid " << MMCKInfoParent.ckid << endl;
	cout << "RIFF fcctype " << MMCKInfoParent.fccType << endl;
	cout << "RIFF offset " << MMCKInfoParent.dwDataOffset << endl;
	cout << "RIFF_cksize " << MMCKInfoParent.cksize << endl;
	MMCKInfoChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if ((mmioDescend(hmmio, &MMCKInfoChild, &MMCKInfoParent, MMIO_FINDCHUNK)))
	{
		cout << "cannot find fmk chunk" << endl;
		return 0;
	}
	//READ Format Chunk
	/*
	HGLOBAL m_hFormat = GlobalAlloc(GMEM_MOVEABLE, MMCKInfoChild.cksize);
	if (!m_hFormat)
	{
		cout << "failed alloc memory" << endl;
		return 0;
	}

	
	WAVEFORMATEX* PCMWaveFmtRecord;
	if (!(PCMWaveFmtRecord = (WAVEFORMATEX*)GlobalLock(m_hFormat))) {
		cout << "cannot allocate memory for format" << endl;
		return 0;
	}
	*/
	PCMWAVEFORMAT PCMWaveFmtRecord;
	if ((unsigned long)mmioRead(hmmio, (HPSTR)&PCMWaveFmtRecord, MMCKInfoChild.cksize) != MMCKInfoChild.cksize) {
		cout << "cannot reak format chunk" << endl;
		return 0;
	}
	cout << "format chunck " << endl;
	cout << "format ckid " << MMCKInfoChild.ckid << endl;
	cout << "format fcctype " << MMCKInfoChild.fccType << endl;
	cout << "format offset " << MMCKInfoChild.dwDataOffset << endl;
	cout << "format_cksize " << MMCKInfoChild.cksize << endl;
	mmioAscend(hmmio, &MMCKInfoChild, 0);
	//find data
	MMCKInfoChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if ((mmioDescend(hmmio, &MMCKInfoChild, &MMCKInfoParent, MMIO_FINDCHUNK)))
	{
		cout << "cannot find data chunk" << endl;
		return 0;
	}
	//size of data
	DWORD DataSize = MMCKInfoChild.cksize;
	DWORD DataOffset = MMCKInfoChild.dwDataOffset;
	cout << "datasize" << DataSize << "Offset" << DataOffset << endl;
	if (DataSize == 0L) {
		cout << "no data" << endl;
		return 0;
	}
	//Allocate data memory
	HANDLE hData;
	LPBYTE lpData;
	hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, DataSize);
	if (!hData) {
		cout << "Out of memory" << endl;
		return 0;
	}
	if ((lpData = (LPBYTE)GlobalLock(hData)) == NULL) {
		cout << "Fail to lock memory" << endl;
		return 0;
	}

	mmioAscend(hmmio, &MMCKInfoChild, 0);
	//read in the data CHUNCK

	if (mmioSeek(hmmio, DataOffset, SEEK_SET) < 0) {
		cout << "cannot read the data in 1" << endl;
	}

	DWORD m_WaveLong;
	m_WaveLong = mmioRead(hmmio, (LPSTR)lpData, DataSize);
	cout << "m_WaveLong" << m_WaveLong << endl;
	if (m_WaveLong < 0) {
		cout << "cannot read the data in 2" << endl;
		return 0;
	}
	//open the device
	HWAVEOUT hWaveOut;
	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, (LPCWAVEFORMATEX)&PCMWaveFmtRecord, NULL, NULL, CALLBACK_NULL) != 0) {
		cout << "unable to Open the file format" << endl;
		return 0;
	}
	//prepare the data
	LPWAVEHDR lpWaveHdr;
	HGLOBAL     hWaveHdr;
	hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
		(DWORD) sizeof(WAVEHDR));
	lpWaveHdr = (LPWAVEHDR)GlobalLock(hWaveHdr);
	//WAVEHDR pWaveOutHdr;
	lpWaveHdr->lpData = (LPSTR)lpData;
	lpWaveHdr->dwBufferLength = m_WaveLong;
	lpWaveHdr->dwFlags = 0L;
	lpWaveHdr->dwLoops = 0L;
	if (waveOutPrepareHeader(hWaveOut, lpWaveHdr, sizeof(WAVEHDR)) != 0) {
		cout << "fail to prepare the wave data buffer" << endl;
		return 0;
	}
	//play the wave
	int wResult;
	wResult = waveOutWrite(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
	if (wResult != 0)
	{
		cout << "fail to write the data buffer" << endl;
		return 0;
	}
	cout << "press any key" << endl;
	do {} while (!(lpWaveHdr->dwFlags & WHDR_DONE));
	waveOutUnprepareHeader(hWaveOut, lpWaveHdr, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
	//Sleep(60 * 1000);
	return 0;
}
//below is his file
/*
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

int main()
{
	HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	char buffer[8000 * 60] = {};
	
	// See http://goo.gl/hQdTi
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((((t * (t >> 8 | t >> 9) & 46 & t >> 8)) ^ (t & t >> 13 | t >> 6)) & 0xFF);
	
	WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
	waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
	Sleep(60 * 1000);
}
*/
