# Create build directory for FreeType
$freetypeBuildDir = "ThirdParty\freetype\build"
New-Item -ItemType Directory -Path $freetypeBuildDir -Force

# Configure FreeType with CMake
Push-Location $freetypeBuildDir
cmake -G "Visual Studio 17 2022" -A x64 ..\freetype-VER-2-13-1
cmake --build . --config Debug
Pop-Location

# Create lib directory and copy the built library
$freetypeLibDir = "ThirdParty\freetype\lib"
New-Item -ItemType Directory -Path $freetypeLibDir -Force
Copy-Item "$freetypeBuildDir\Debug\freetyped.lib" "$freetypeLibDir\freetype.lib" 