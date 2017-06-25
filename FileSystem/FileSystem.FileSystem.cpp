
#include "FileSystem.FileSystem.h"

#include "FileSystem.CMakeListsFile.h"
#include "FileSystem.Directory.h"
#include "FileSystem.File.h"
#include "FileSystem.FileBase.h"

#include <string.h>

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

namespace nFileSystem
{

cFileSystem::~cFileSystem()
{
}


cFileSystem::cFileSystem()
{
}


// static
cDirectory*
cFileSystem::ReadDirectory( const std::string& iPath )
{
	DIR* directory = opendir( iPath.c_str() );
	if( directory == NULL )
	{
		printf( "Error while opening directory\n" );
		return  0;
	}

	::nFileSystem::cDirectory* mainDirectory = new ::nFileSystem::cDirectory( iPath );

	struct dirent *files;
	struct stat fileStat;

	while( ( files = readdir( directory ) ) != NULL )
	{
		if( !strcmp( files->d_name, "." ) || !strcmp( files->d_name, ".." ) )
			continue;

		std::string filePath = iPath;
		filePath.append( "/" );
		filePath.append( files->d_name );

		if( stat( filePath.c_str(), &fileStat ) == -1 )
		{
			printf( "Couldn't get file information for %s\n", files->d_name );
			break;
		}

		if( S_ISDIR( fileStat.st_mode ) )
		{
			::nFileSystem::cDirectory* directory = ReadDirectory( filePath );
			mainDirectory->AddContent( directory );
		}
		else
		{
			if( !strcmp( files->d_name, "CMakeLists.txt" ) )
			{
				::nFileSystem::cCMakeListsFile* makeFile = new ::nFileSystem::cCMakeListsFile( filePath );
				mainDirectory->SetMakeFile( makeFile );
			}
			else
			{
				::nFileSystem::cFile* file = new ::nFileSystem::cFile( filePath );
				mainDirectory->AddContent( file );
			}
		}
	}

	mainDirectory->ReadAllPropertiesFromCMakeListsFile();

	return  mainDirectory;
}

}

