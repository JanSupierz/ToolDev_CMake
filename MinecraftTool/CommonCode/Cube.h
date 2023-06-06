#pragma once
#include <string>
#include <vector>

class Cube
{
public:
	static void ParseCubes(const std::wstring& fileName, std::vector<Cube>& cubes);
	static void CubesToObj(const std::wstring& outputFile, std::vector<Cube>& cubes);

private:
	std::string type;
	int x, y, z;
	bool isOpaque;

	bool hasTopNeighbor{};
	bool hasBottomNeighbor{};
	bool hasLeftNeighbor{};
	bool hasRightNeighbor{};
	bool hasFrontNeighbor{};
	bool hasBackNeighbor{};

	void WriteCube(FILE* pOFile, int& index) const;

	static void SetNeighbors(std::vector<Cube>& cubes);

	void WriteVertices(FILE* pOFile, int* indices, int& currentIndex) const;
};
