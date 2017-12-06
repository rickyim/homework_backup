//https://gist.github.com/kbjorklu/6317308 of great help
//there are also other samples you can find on google of baidu
//but the key is that you should not reset the device and you should wait at the end 
#include "stdafx.h"
#include <Windows.h>
#include <string>
#include<mmsystem.h>
#pragma comment(lib, "Winmm.lib")
using namespace std;
int main() {
	LPWSTR lp = _T("E:\\courses\\vocal\\exp_2\\test_16k.wav");
	MMCKINFO MMCKInfoParent;
	MMCKINFO MMCKInfoChild;
	HMMIO hmmio = mmioOpen(lp, NULL, MMIO_READ|MMIO_ALLOCBUF);
	if (!hmmio) {
		cout << "cannot open" << endl;
		return 0;
	}
	MMCKInfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmioDescend(hmmio, &MMCKInfoParent, NULL, MMIO_FINDRIFF);
	cout << "format chunck" << endl;
	cout << MMCKInfoParent.ckid << endl;
	cout << MMCKInfoParent.fccType << endl;
	cout << MMCKInfoParent.dwDataOffset << endl;
	cout << "parent_cksize" << MMCKInfoParent.cksize << endl;
	MMCKInfoChild.ckid = mmioFOURCC('f', 'm', 't',' ');
	if ((mmioDescend(hmmio, &MMCKInfoChild, &MMCKInfoParent, MMIO_FINDCHUNK)))
	{
		cout << "cannot find fmk chunk" << endl;
		return 0;
	}
	//READ Format Chunk
	HGLOBAL m_hFormat= GlobalAlloc(GMEM_MOVEABLE, MMCKInfoChild.cksize);
	if (!m_hFormat)
	{
		cout<<"failed alloc memory"<<endl;
		return 0;
	}
	
	WAVEFORMATEX* PCMWaveFmtRecord;
	if (!(PCMWaveFmtRecord = (WAVEFORMATEX*)GlobalLock(m_hFormat))) {
		cout << "cannot allocate memory for format" << endl;
		return 0;
	}
	if ((unsigned long)mmioRead(hmmio, (HPSTR)PCMWaveFmtRecord, MMCKInfoChild.cksize) != MMCKInfoChild.cksize) {
		cout << "cannot reak format chunk" << endl;
		return 0;
	}
	cout << "format chunck" << endl;
	cout << MMCKInfoChild.ckid<<endl;
	cout << MMCKInfoChild.fccType<<endl;
	cout << MMCKInfoChild.dwDataOffset << endl;
	cout << "format_cksize" << MMCKInfoChild.cksize << endl;
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
	cout << "datasize" << DataSize << "Offset"<<DataOffset<<endl;
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
	cout << "m_WaveLong" <<m_WaveLong<< endl;
	if (m_WaveLong < 0) {
		cout << "cannot read the data in 2" << endl;
		return 0;
	}
	//open the device
	/*
	WAVEOUTCAPS pwoc;
	if(waveOutGetDevCaps(WAVE_MAPPER, &pwoc, sizeof(WAVEOUTCAPS)) != 0) {
		cout << "unable to get devices" << endl;
		return 0;
	}
	*/
	HWAVEOUT hWaveOut;
	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, PCMWaveFmtRecord, NULL,NULL, CALLBACK_NULL) != 0) {
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
	lpWaveHdr->dwLoops = 1L;
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
	//do {
//
	//} while (!pWaveOutHdr.dwFlags & WHDR_DONE);
	cout << "press any key" << endl;
	//waveOutReset(hWaveOut);//have to comment this
	waveOutClose(hWaveOut);
	Sleep(60*1000);//have to wait, otherwise no voice!
	return 0;
}
