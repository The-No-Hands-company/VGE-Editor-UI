# File movement verification script
$ErrorActionPreference = "Stop"

# Function to verify file copy
function Verify-FileCopy {
    param (
        [string]$sourcePath,
        [string]$destPath
    )
    
    if (!(Test-Path $sourcePath)) {
        Write-Host "Source file not found: $sourcePath" -ForegroundColor Red
        return $false
    }
    
    if (!(Test-Path $destPath)) {
        Write-Host "Destination file not created: $destPath" -ForegroundColor Red
        return $false
    }
    
    $sourceHash = Get-FileHash $sourcePath
    $destHash = Get-FileHash $destPath
    
    if ($sourceHash.Hash -ne $destHash.Hash) {
        Write-Host "File hash mismatch for: $sourcePath" -ForegroundColor Red
        return $false
    }
    
    return $true
}

# Function to move file with verification
function Move-FileWithVerification {
    param (
        [string]$sourcePath,
        [string]$destPath
    )
    
    Write-Host "Moving: $sourcePath -> $destPath" -ForegroundColor Yellow
    
    # Create destination directory if it doesn't exist
    $destDir = Split-Path -Parent $destPath
    if (!(Test-Path $destDir)) {
        New-Item -ItemType Directory -Path $destDir -Force
    }
    
    # Copy file
    Copy-Item -Path $sourcePath -Destination $destPath -Force
    
    # Verify copy
    if (Verify-FileCopy $sourcePath $destPath) {
        Write-Host "Successfully moved and verified: $sourcePath" -ForegroundColor Green
        # Remove source file only after successful verification
        Remove-Item -Path $sourcePath
        return $true
    }
    
    return $false
}

# File movement mappings
$movements = @(
    @{
        Source = "Source/UI/Core/UISystem.*"
        Destination = "Source/Public/Core/"
    },
    @{
        Source = "Source/UI/Core/UIElement.*"
        Destination = "Source/Public/Core/"
    },
    @{
        Source = "Source/UI/Core/UIObject.*"
        Destination = "Source/Public/Core/"
    },
    @{
        Source = "Source/UI/Core/UIWidget.*"
        Destination = "Source/Public/Core/"
    },
    @{
        Source = "Source/UI/Controls/*"
        Destination = "Source/Public/Controls/"
    },
    @{
        Source = "Source/UI/Dialogs/*"
        Destination = "Source/Public/Dialogs/"
    },
    @{
        Source = "Source/UI/Layout/*"
        Destination = "Source/Public/Layout/"
    },
    @{
        Source = "Source/UI/Rendering/*"
        Destination = "Source/Public/Rendering/"
    }
)

# Execute movements
foreach ($move in $movements) {
    $sourceFiles = Get-ChildItem -Path $move.Source
    foreach ($file in $sourceFiles) {
        $destPath = Join-Path $move.Destination $file.Name
        Move-FileWithVerification $file.FullName $destPath
    }
}

Write-Host "`nFile movement complete!" -ForegroundColor Green
