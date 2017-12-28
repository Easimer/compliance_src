#include "stdafx.h"
#include "tier0/icommandline.h"
#include <filesystem.h>
#include "../vmfexport/class.h"

int main(int argc, char** argv)
{
	CommandLine()->CreateCmdLine(argc, argv);
	
	Msg("VMFExport Test\nCreating a class\n");
	
	CVMFClass test("ctest");
	Msg("Creating a property\n");
	CVMFProperty test2("test", "test");
	Msg("Adding property\n");
	test.AddProperty(test2);

	Msg("Opening outfile\n");

	FileHandle_t f = g_pFullFileSystem->Open("test.vmf", "w");

	if (!f)
	{
		Msg("Cannot open test.vmf\n");
		return 1;
	}

	test.Write(f);

	Msg("Written file\n");

	g_pFullFileSystem->Close(f);
	Msg("Closed file, finishing\n");
	
	return 0;
}