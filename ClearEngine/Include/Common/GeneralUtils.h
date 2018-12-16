#pragma once

#include <vector>
#include <list>
#include <fstream>
#include <iostream>
#include "Common/DirectX.h"

class GeneralUtils
{

public:
	static std::wstring GetPath(std::wstring filePath)
	{
		int lastDashPos = 0;
		int count = 0;
		std::wstring::iterator pos = filePath.begin();
		while (pos != filePath.end())
		{
			if (*pos == L'/')
			{
				lastDashPos = count;
			}
			count++;
			++pos;
		}

		return filePath.substr(0, lastDashPos + 1);
	};

	static std::wstring GetFileNameFromPath(std::wstring& filePath)
	{
		int lastDashPos = 0;
		int count = 0;
		std::wstring::iterator pos = filePath.begin();
		while (pos != filePath.end())
		{
			if (*pos == L'/' || *pos == '\\')
			{
				lastDashPos = count;
			}
			count++;
			++pos;
		}

		return filePath.substr(lastDashPos + 1, filePath.length());
	};

	static std::wstring RemoveFileNameExtension(std::wstring& fileName)
	{
		int lastDashPos = 0;
		int count = 0;
		std::wstring::iterator pos = fileName.begin();
		while (pos != fileName.end())
		{
			count++;
			if (*pos == L'.')
			{
				break;
			}
			++pos;
		}
		return fileName.substr(0, count);
	};

	static void WStringToString(const std::wstring& ws, std::string& s)
	{
		s.assign(ws.begin(), ws.end());
	};

	static void StringToWString(const std::string& s, std::wstring& ws)
	{
		ws.assign(s.begin(), s.end());
	};

	static bool FileIsEmpty(std::string fileName)
	{
		int length;
		std::ifstream filestr;

		filestr.open(fileName, std::ios::binary); // open your file
		filestr.seekg(0, std::ios::end); // put the "cursor" at the end of the file
		length = filestr.tellg(); // find the position of the cursor
		filestr.close(); // close your file

		if (length == 0 )
		{
			return true;
		}
		else
		{
			return false;
		}
	};

	template<typename T>
	static void DeleteAllVectorPointers(std::vector<T*>& vector)
	{
		if (vector.empty())
		{
			return;
		}
		std::vector<T*>::iterator iter;
		for (iter = vector.begin(); iter != vector.end(); iter++)
		{
			delete *iter;
		}
		vector.clear();
	}

	template<typename T>
	static void RemoveVectorPointer(std::vector<T*>& vector, T* value, bool deleteValue)
	{
		std::vector<T*>::iterator iter;
		for (iter = vector.begin(); iter != vector.end(); ++iter)
		{
			if (*iter == value)
			{
				if (deleteValue)
				{
					delete *iter;
				}
				vector.erase(iter);
				break;
			}
		}
	}

	template<typename T>
	static void RemoveVectorPointers(std::vector<T*>& vector, T** values, unsigned int valueCount, bool deleteValues)
	{
		std::vector<T*>::iterator iter;
		bool match = false;
		for (iter = vector.begin(); iter != vector.end();)
		{
			for (int i = 0; i < valueCount; i++)
			{
				if (*iter == values[i])
				{
					if (deleteValues)
					{
						delete *iter;
					}
					iter = vector.erase(iter);
					match = true;
					break;
				}
			}
			if (match)
			{
				match = false;
			}
			else
			{
				++iter;
			}
		}
	}

	template<typename T, typename U>
	static void ClearMapWithSecondEntryPointers(std::map<T, U*> map, bool isGraphicsResource = false)
	{
		std::map<T, U*>::iterator iter;
		for (iter = map.begin(); iter != map.end();)
		{
			if (!iter->second)
			{
				continue;
			}
			//The Release function needs to be called for DirectX graphics resources.
			if (isGraphicsResource)
			{
				((ID3D11Resource*)iter->second)->Release();
			}
			else
			{
				delete iter->second;
			}
			iter = map.erase(iter);
		}
	}
};
