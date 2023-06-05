#include "Cube.h"
#include "ReadException.h"

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	std::vector<Cube> cubes{};

	try
	{
		Cube::ParseCubes(argv[1], cubes);
	}

	catch(const ReadException& exception)
	{
		exception.What();
		return -1;
	}

	std::wstring outputFile{ L"scene.obj" };

	//Change the path, if given in the argument
	if (argc > 2)
	{
		outputFile = argv[2];
	}

	//Save cubes to a file
	Cube::CubesToObj(outputFile, cubes);

	return 0;
}

