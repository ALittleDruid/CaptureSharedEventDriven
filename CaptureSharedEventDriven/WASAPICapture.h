// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved
//
#pragma once
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <MMDeviceAPI.h>

//
//  WASAPI Capture class.
class CWASAPICapture : public IAudioSessionEvents, IMMNotificationClient
{
public:
	//  Public interface to CWASAPICapture.
	CWASAPICapture(IMMDevice *Endpoint, bool EnableStreamSwitch, ERole EndpointRole);
	bool Initialize(UINT32 EngineLatency);
	void Shutdown();
	bool Start(BYTE *CaptureBuffer, size_t BufferSize);
	void Stop();
	WORD ChannelCount() { return _MixFormat->nChannels; }
	UINT32 SamplesPerSecond() { return _MixFormat->nSamplesPerSec; }
	UINT32 BytesPerSample() { return _MixFormat->wBitsPerSample / 8; }
	size_t FrameSize() { return _FrameSize; }
	WAVEFORMATEX *MixFormat() { return _MixFormat; }
	size_t BytesCaptured() { return _CurrentCaptureIndex; }
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

private:
	~CWASAPICapture(); // Destructor is private to prevent accidental deletion.
	LONG _RefCount{1};
	//
	//  Core Audio Capture member variables.
	//
	IMMDevice *_Endpoint;
	IAudioClient *_AudioClient{nullptr};
	IAudioCaptureClient *_CaptureClient{nullptr};

	HANDLE _CaptureThread{nullptr};
	HANDLE _ShutdownEvent{nullptr};
	HANDLE _AudioSamplesReadyEvent{nullptr};
	WAVEFORMATEX *_MixFormat{nullptr};
	size_t _FrameSize{0};
	UINT32 _BufferSize{0};

	//
	//  Capture buffer management.
	//
	BYTE *_CaptureBuffer{nullptr};
	size_t _CaptureBufferSize{0};
	size_t _CurrentCaptureIndex{0};

	static DWORD __stdcall WASAPICaptureThread(LPVOID Context);
	DWORD DoCaptureThread();
	//
	//  Stream switch related members and methods.
	//
	bool _EnableStreamSwitch;
	ERole _EndpointRole;
	HANDLE _StreamSwitchEvent{nullptr}; // Set when the current session is disconnected or the default device changes.
	HANDLE _StreamSwitchCompleteEvent{nullptr}; // Set when the default device changed.
	IAudioSessionControl *_AudioSessionControl{nullptr};
	IMMDeviceEnumerator *_DeviceEnumerator{nullptr};
	LONG _EngineLatencyInMS{0};
	bool _InStreamSwitch{false};

	bool InitializeStreamSwitch();
	void TerminateStreamSwitch();
	bool HandleStreamSwitchEvent();

	STDMETHOD(OnDisplayNameChanged)(LPCWSTR /*NewDisplayName*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnIconPathChanged)(LPCWSTR /*NewIconPath*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnSimpleVolumeChanged)(float /*NewSimpleVolume*/, BOOL /*NewMute*/, LPCGUID /*EventContext*/) { return S_OK; }
	STDMETHOD(OnChannelVolumeChanged)(DWORD /*ChannelCount*/, float /*NewChannelVolumes*/[], DWORD /*ChangedChannel*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnGroupingParamChanged)(LPCGUID /*NewGroupingParam*/, LPCGUID /*EventContext*/) { return S_OK; };
	STDMETHOD(OnStateChanged)(AudioSessionState /*NewState*/) { return S_OK; };
	STDMETHOD(OnSessionDisconnected)(AudioSessionDisconnectReason DisconnectReason);
	STDMETHOD(OnDeviceStateChanged)(LPCWSTR /*DeviceId*/, DWORD /*NewState*/) { return S_OK; }
	STDMETHOD(OnDeviceAdded)(LPCWSTR /*DeviceId*/) { return S_OK; };
	STDMETHOD(OnDeviceRemoved)(LPCWSTR /*DeviceId(*/) { return S_OK; };
	STDMETHOD(OnDefaultDeviceChanged)(EDataFlow Flow, ERole Role, LPCWSTR NewDefaultDeviceId);
	STDMETHOD(OnPropertyValueChanged)(LPCWSTR /*DeviceId*/, const PROPERTYKEY /*Key*/) { return S_OK; };
	//
	//  IUnknown
	//
	STDMETHOD(QueryInterface)(REFIID iid, void **pvObject);

	//
	//  Utility functions.
	//
	bool InitializeAudioEngine();
	bool LoadFormat();
};
