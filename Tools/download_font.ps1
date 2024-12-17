$fontUrl = "https://github.com/googlefonts/roboto/raw/main/src/hinted/Roboto-Regular.ttf"
$outputFile = "..\Fonts\Roboto-Regular.ttf"

# Create a web client with TLS 1.2 support
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
$webClient = New-Object System.Net.WebClient

# Download the file
Write-Host "Downloading font from $fontUrl"
try {
    $webClient.DownloadFile($fontUrl, $outputFile)
    Write-Host "Font downloaded successfully to $outputFile"
} catch {
    Write-Host "Failed to download font: $_"
}
