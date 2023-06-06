#include "Cube.h"
#include "ReadException.h"
#include <sstream>

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	std::vector<Cube> cubes{};

	if (argc == 1)
	{
		std::cout << "No input selected\n";
		return 0;
	}

	try
	{
		Cube::ParseCubes(argv[1], cubes);
	}

	catch(const ReadException& exception)
	{
		std::cout << exception.What() << '\n';
		return 0;
	}

	std::wstring outputFile{ L"scene.obj" };

	//Change the path, if given in the argument
	if (argc > 2)
	{
		outputFile = argv[2];
	}

	//Save cubes to a file
	Cube::CubesToObj(outputFile, cubes);

	if (argc > 3 && wcscmp(argv[3], L"Print") == 0)
	{
		std::ifstream inputStream{ outputFile };
		std::stringstream buffer{};

		std::string line{};
		float x{}, y{}, z{};
		char slash{};

		while (inputStream.good())
		{
			inputStream >> line;

			if (line == "v")
			{
				inputStream >> x >> y >> z;
				buffer << "v " << x << ' ' << y << ' ' << z << '\n';
			}
			else if (line == "vn")
			{
				inputStream >> x >> y >> z;
				buffer << "vn " << x << ' ' << y << ' ' << z << '\n';
			}
			else if (line == "vt")
			{
				inputStream >> x >> y;
				buffer << "vt " << x << ' ' << y << '\n';
			}
			else if (line == "f")
			{
				buffer << "f ";

				//Repeat 3 times
				for (int index{}; index < 3; ++index)
				{
					inputStream >> x >> slash >> y >> slash >> z;
					buffer << x << '/' << y << '/' << z << ' ';
				}

				buffer << '\n';
			}
		}

		std::cout << buffer.str();
	}

	return 0;
}

