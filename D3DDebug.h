#pragma once

#ifdef _DEBUG
	#ifndef HR
	#define HR(x)                                             \
	{                                                         \
		HRESULT hr = (x);                                     \
		if(FAILED(hr))                                        \
		{                                                     \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, #x, true); \
		}                                                     \
	}
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif
