Write-Host "Setting up dependencies for UI.Sandbox..."

# Create ThirdParty directory
$thirdPartyDir = "ThirdParty"
New-Item -ItemType Directory -Path $thirdPartyDir -Force

# GLFW
$glfwVersion = "3.3.8"
$glfwUrl = "https://github.com/glfw/glfw/releases/download/$glfwVersion/glfw-$glfwVersion.zip"
$glfwZip = "$thirdPartyDir\glfw.zip"
Invoke-WebRequest -Uri $glfwUrl -OutFile $glfwZip
Expand-Archive -Path $glfwZip -DestinationPath $thirdPartyDir -Force
Remove-Item $glfwZip
Move-Item -Path "$thirdPartyDir\glfw-$glfwVersion" -Destination "$thirdPartyDir\glfw" -Force

# GLM
$glmVersion = "0.9.9.8"
$glmUrl = "https://github.com/g-truc/glm/releases/download/$glmVersion/glm-$glmVersion.zip"
$glmZip = "$thirdPartyDir\glm.zip"
Invoke-WebRequest -Uri $glmUrl -OutFile $glmZip
Expand-Archive -Path $glmZip -DestinationPath $thirdPartyDir -Force
Remove-Item $glmZip

# FreeType
$freetypeVersion = "VER-2-13-1"
$freetypeUrl = "https://github.com/freetype/freetype/archive/refs/tags/$freetypeVersion.zip"
$freetypeZip = "$thirdPartyDir\freetype.zip"
Invoke-WebRequest -Uri $freetypeUrl -OutFile $freetypeZip
Expand-Archive -Path $freetypeZip -DestinationPath $thirdPartyDir -Force
Remove-Item $freetypeZip
Move-Item -Path "$thirdPartyDir\freetype-$freetypeVersion" -Destination "$thirdPartyDir\freetype" -Force

Write-Host "`nGlad needs to be generated from https://glad.dav1d.de/ with these settings:"
Write-Host "- Language: C/C++"
Write-Host "- Specification: OpenGL"
Write-Host "- Profile: Core"
Write-Host "- Version: 4.6"
Write-Host "- Generate a loader: Yes"
Write-Host "`nAfter generating, extract to $thirdPartyDir\glad"

Write-Host "`nAll dependencies have been downloaded and set up successfully!"