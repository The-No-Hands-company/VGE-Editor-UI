# Create directories
Write-Host "Setting up GLAD..."

$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$gladDir = Join-Path $scriptPath "..\ThirdParty\glad"
$includeDir = Join-Path $gladDir "include"
$sourceDir = Join-Path $gladDir "src"

# Try to find Python installation
$pythonPath = "C:\Users\$env:USERNAME\AppData\Local\Programs\Python\Python313\python.exe"
if (!(Test-Path $pythonPath)) {
    $pythonPath = "C:\Program Files\Python313\python.exe"
}
if (!(Test-Path $pythonPath)) {
    Write-Host "Error: Could not find Python 3.13 installation. Please ensure Python is installed and try again."
    exit 1
}

if (!(Test-Path -Path $includeDir)) {
    New-Item -ItemType Directory -Path $includeDir -Force -ErrorAction Stop
}
if (!(Test-Path -Path $sourceDir)) {
    New-Item -ItemType Directory -Path $sourceDir -Force -ErrorAction Stop
}

# Install required dependencies
Write-Host "Installing dependencies..."
& $pythonPath -m pip install --upgrade pip --quiet
& $pythonPath -m pip install glad --quiet

# Generate GLAD files
Write-Host "Generating GLAD files..."
& $pythonPath -m glad --api="gl:core=4.6" --out-path="$gladDir" c

Write-Host "GLAD setup complete!" 