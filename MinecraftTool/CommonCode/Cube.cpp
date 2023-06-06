#include "Cube.h"
#include "ReadException.h"
#include <iostream>
#include <fstream>
#include <codecvt>
#include <cwctype>

//rapidjson
#include <rapidjson.h>
#include <document.h>
#include <stream.h>
#include <istreamwrapper.h>
#include <filereadstream.h>

void Cube::ParseCubes(const std::wstring& fileName, std::vector<Cube>& cubes)
{
	if (std::fstream is{ fileName })
	{
		rapidjson::IStreamWrapper isw{ is };
		rapidjson::Document sceneDoc{};
		sceneDoc.ParseStream(isw);

		if (!sceneDoc.IsArray())
		{
			throw ReadException{ "Root object is not an array!" };
		}

		for (rapidjson::Value::ConstValueIterator it{ sceneDoc.Begin() }; it != sceneDoc.End(); ++it)
		{
			const rapidjson::Value& jsonCube = *it;
			Cube cube{};

			//Read type
			if (!jsonCube.HasMember("layer"))
			{
				throw ReadException{ "Value has no layer member!" };
			}

			const rapidjson::Value& type{ jsonCube["layer"] };

			if (!type.IsString())
			{
				throw ReadException{ "Reading type value failed!" };
			}

			cube.type = type.GetString();

			//Read opacity
			if (!jsonCube.HasMember("opaque"))
			{
				throw ReadException{ "Reading opaque value failed!" };
			}

			const rapidjson::Value& isOpaque{ jsonCube["opaque"] };

			if (!isOpaque.IsBool())
			{
				throw ReadException{ "Value has no opaque member!" };
			}

			cube.isOpaque = isOpaque.GetBool();

			//Read positions
			if (!jsonCube.HasMember("positions"))
			{
				throw ReadException{ "Value has no position member!" };
			}

			const rapidjson::Value& positions{ jsonCube["positions"] };

			if (!positions.IsArray())
			{
				throw ReadException{ "Reading positions array failed!" };
			}

			for (rapidjson::Value::ConstValueIterator locationIt{ positions.Begin() }; locationIt != positions.End(); ++locationIt)
			{
				const rapidjson::Value& location = *locationIt;

				if (!(location.IsArray() && location.Size() == 3))
				{
					throw ReadException{ "Reading cube location failed!" };
				}

				const rapidjson::Value& x = location[0];
				const rapidjson::Value& y = location[1];
				const rapidjson::Value& z = location[2];

				if (!(x.IsInt() && y.IsInt() && z.IsInt()))
				{
					throw ReadException{ "Reading x,y,z coordinates failed!" };
				}

				cube.x = x.GetInt();
				cube.y = y.GetInt();
				cube.z = z.GetInt();

				cubes.push_back(cube);
			}
		}
	}
	else
	{
		throw ReadException{ "Incorrect input file!" };
	}
}

void Cube::CubesToObj(const std::wstring& outputFile, std::vector<Cube>& cubes)
{
	FILE* pOFile = nullptr;
	_wfopen_s(&pOFile, outputFile.data(), L"w+,ccs=UTF-8");

	if (pOFile != nullptr)
	{
		// it was possible to create the file for writing.
		const wchar_t* text = L"#∂ is the symbol for partial derivative.\n";
		fwrite(text, wcslen(text) * sizeof(wchar_t), 1, pOFile);

		fwprintf_s(pOFile, L"mtllib minecraftMats.mtl\n");

		//Write normals
		fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0, 0.0, 1.0);
		fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0, 0.0, -1.0);
		fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0, 1.0, 0.0);
		fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 0.0, -1.0, 0.0);
		fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", 1.0, 0.0, 0.0);
		fwprintf_s(pOFile, L"vn %.4f %.4f %.4f\n", -1.0, 0.0, 0.0);

		//Write texture coordinates
		fwprintf_s(pOFile, L"vt %.4f %.4f\n", 0.0, 0.0); //BottomLeft (1)
		fwprintf_s(pOFile, L"vt %.4f %.4f\n", 1.0, 0.0); //BottomRight (2)
		fwprintf_s(pOFile, L"vt %.4f %.4f\n", 0.0, 1.0); //TopLeft (3)
		fwprintf_s(pOFile, L"vt %.4f %.4f\n", 1.0, 1.0); //TopRight (4)

		Cube::SetNeighbors(cubes);

		int index{};
		std::string currentType{ "" };

		//Convert the normal string to a wstring
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

		for (const Cube& cube : cubes)
		{
			//Layer is different
			if (cube.type != currentType)
			{
				currentType = cube.type;

				//Change texture
				std::wstring wstr{ converter.from_bytes(cube.type) };
				wstr[0] = std::towupper(wstr[0]);

				fwprintf_s(pOFile, L"usemtl %s\n", wstr.data());
			}

			cube.WriteCube(pOFile, index);
		}

		fclose(pOFile);
		std::cout << "OBJ file generated successfully!" << std::endl;
	}
	else
	{
		std::cout << "Output file could not be generated" << std::endl;
	}
}

void Cube::WriteCube(FILE* pOFile, int& index) const
{
	int indices[8]{ -1,-1,-1,-1,-1,-1,-1,-1 };
	int currentIndex{ 0 };

	WriteVertices(pOFile, indices, currentIndex);

	//Write faces														
	if (!hasFrontNeighbor)
	{
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[0], 1, 2, index + indices[6], 4, 2, index + indices[4], 2, 2);
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[0], 1, 2, index + indices[2], 3, 2, index + indices[6], 4, 2);
	}

	if (!hasBackNeighbor)
	{
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[1], 2, 1, index + indices[5], 1, 1, index + indices[7], 3, 1);
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[1], 2, 1, index + indices[7], 3, 1, index + indices[3], 4, 1);
	}

	if (!hasBottomNeighbor)
	{
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[0], 1, 4, index + indices[4], 2, 4, index + indices[5], 4, 4);
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[0], 1, 4, index + indices[5], 4, 4, index + indices[1], 3, 4);
	}

	if (!hasTopNeighbor)
	{
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[2], 1, 3, index + indices[7], 4, 3, index + indices[6], 2, 3);
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[2], 1, 3, index + indices[3], 3, 3, index + indices[7], 4, 3);
	}

	if (!hasLeftNeighbor)
	{
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[0], 2, 6, index + indices[3], 3, 6, index + indices[2], 4, 6);
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[0], 2, 6, index + indices[1], 1, 6, index + indices[3], 3, 6);
	}

	if (!hasRightNeighbor)
	{
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[4], 1, 5, index + indices[6], 3, 5, index + indices[7], 4, 5);
		fwprintf_s(pOFile, L"f %d/%d/%d %d/%d/%d %d/%d/%d\n", index + indices[4], 1, 5, index + indices[7], 4, 5, index + indices[5], 2, 5);
	}

	index += currentIndex;
}

void Cube::SetNeighbors(std::vector<Cube>& cubes)
{
	for (Cube& cube : cubes)
	{
		for (const Cube& compareCube : cubes)
		{
			//Hide faces only if both cubes are opaque or both are transparent
			//No neighbor means visible
			if (cube.isOpaque != compareCube.isOpaque) continue;

			//Hide left/right
			if (cube.y == compareCube.y && cube.z == compareCube.z)
			{
				if (cube.x + 1 == compareCube.x)
				{
					cube.hasRightNeighbor = true;
				}
				else if (cube.x - 1 == compareCube.x)
				{
					cube.hasLeftNeighbor = true;
				}
			}

			//Hide back/front
			if (cube.x == compareCube.x && cube.z == compareCube.z)
			{
				if (cube.y + 1 == compareCube.y)
				{
					cube.hasBackNeighbor = true;
				}
				else if (cube.y - 1 == compareCube.y)
				{
					cube.hasFrontNeighbor = true;
				}
			}

			//Hide top/bottom
			if (cube.x == compareCube.x && cube.y == compareCube.y)
			{
				if (cube.z + 1 == compareCube.z)
				{
					cube.hasTopNeighbor = true;
				}
				else if (cube.z - 1 == compareCube.z)
				{
					cube.hasBottomNeighbor = true;
				}
			}
		}
	}
}

void Cube::WriteVertices(FILE* pOFile, int* indices, int& currentIndex) const
{
	
	if (!hasBackNeighbor)
	{
		indices[1] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 0, y + 1); //bottom - left - back (1)

		indices[3] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 1, y + 1); //top - left - back (3)

		indices[5] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 0, y + 1); //bottom - right - back (5)

		indices[7] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 1, y + 1); //top - right - back (7) 
	}

	if (!hasFrontNeighbor)
	{
		indices[0] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 0, y + 0); //bottom - left - front (0)

		indices[2] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 1, y + 0); //top - left - front (2)

		indices[4] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 0, y + 0); //bottom - right - front (4)

		indices[6] = ++currentIndex;
		fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 1, y + 0); //top - right - front (6)
	}

	if (!hasBottomNeighbor)
	{
		if (hasBackNeighbor)
		{
			indices[1] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 0, y + 1); //bottom - left - back (1)

			indices[5] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 0, y + 1); //bottom - right - back (5)
		}

		if (hasFrontNeighbor)
		{
			indices[4] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 0, y + 0); //bottom - right - front (4)

			indices[0] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 0, y + 0); //bottom - left - front (0)
		}
	}

	if (!hasTopNeighbor)
	{
		if (hasBackNeighbor)
		{
			indices[3] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 1, y + 1); //top - left - back (3)

			indices[7] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 1, y + 1); //top - right - back (7) 
		}

		if (hasFrontNeighbor)
		{
			indices[2] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 1, y + 0); //top - left - front (2)

			indices[6] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 1, y + 0); //top - right - front (6)
		}
	}

	if (!hasLeftNeighbor)
	{
		if (indices[3] == -1)
		{
			indices[3] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 1, y + 1); //top - left - back (3)
		}

		if (indices[2] == -1)
		{
			indices[2] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 1, y + 0); //top - left - front (2)
		}

		if (indices[0] == -1)
		{
			indices[0] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 0, y + 0); //bottom - left - front (0)
		}

		if (indices[1] == -1)
		{
			indices[1] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 0, z + 0, y + 1); //bottom - left - back (1)
		}
	}

	if (!hasRightNeighbor)
	{
		if (indices[6] == -1)
		{
			indices[6] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 1, y + 0); //top - right - front (6)
		}

		if (indices[7] == -1)
		{
			indices[7] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 1, y + 1); //top - right - back (7) 
		}

		if (indices[5] == -1)
		{
			indices[5] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 0, y + 1); //bottom - right - back (5)
		}

		if (indices[4] == -1)
		{
			indices[4] = ++currentIndex;
			fwprintf_s(pOFile, L"v %d %d %d\n", x + 1, z + 0, y + 0); //bottom - right - front (4)
		}
	}
	
}
