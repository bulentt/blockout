#pragma once

template <typename T>
void SafeRelease(T*& obj)
{
	if (obj)
	{
		obj->Release();
		obj = nullptr;
	}
}

template <typename T>
void SafeDelete(T*& obj)
{
	delete obj;
	obj = nullptr;
}

template <typename T>
void SafeDeleteArray(T*& arr)
{
	delete [] arr;
	arr = nullptr;
}
