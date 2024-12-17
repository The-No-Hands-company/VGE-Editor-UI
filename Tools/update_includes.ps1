# Include path update script
$ErrorActionPreference = "Stop"

# Function to update include paths in a file
function Update-IncludePaths {
    param (
        [string]$filePath
    )
    
    Write-Host "Updating includes in: $filePath" -ForegroundColor Yellow
    
    $content = Get-Content $filePath -Raw
    
    # Update include paths based on new module structure
    $replacements = @{
        # Core module updates
        '#include "Platform/' = '#include "Runtime/Core/Public/Platform/'
        '#include "Core/' = '#include "Runtime/Core/Public/'
        
        # Framework module updates
        '#include "Input/' = '#include "Runtime/Framework/Public/Input/'
        '#include "Layout/' = '#include "Runtime/Framework/Public/Layout/'
        '#include "Rendering/' = '#include "Runtime/Framework/Public/Rendering/'
        '#include "Types/' = '#include "Runtime/Framework/Public/Types/'
        
        # UI module updates
        '#include "Controls/' = '#include "Runtime/UI/Public/Controls/'
        '#include "Interfaces/' = '#include "Runtime/UI/Public/Interfaces/'
        '#include "Widgets/' = '#include "Runtime/UI/Public/Widgets/'
        
        # Editor module updates
        '#include "Editor/' = '#include "Editor/VersaEditor/Public/'
        
        # Clean up relative paths
        '#include "../' = '#include "'
        '#include "./' = '#include "'
        
        # Legacy path updates
        '#include "Elements/' = '#include "Runtime/UI/Public/Controls/'
        '#include "Events/' = '#include "Runtime/Core/Public/'
        '#include "UI/Core/' = '#include "Runtime/Core/Public/'
        '#include "UI/Controls/' = '#include "Runtime/UI/Public/Controls/'
        '#include "UI/Dialogs/' = '#include "Editor/VersaEditor/Public/'
        '#include "UI/Layout/' = '#include "Runtime/Framework/Public/Layout/'
        '#include "UI/Rendering/' = '#include "Runtime/Framework/Public/Rendering/'
    }
    
    foreach ($key in $replacements.Keys) {
        $content = $content -replace [regex]::Escape($key), $replacements[$key]
    }
    
    # Add core include if needed
    if ($content -match '#include' -and $content -notmatch '#include "Runtime/Core/Public/UICore.h"' -and $content -match 'namespace VGE') {
        $insertPoint = $content.IndexOf("#pragma once") + "#pragma once".Length
        $content = $content.Insert($insertPoint, "`n`n#include ""Runtime/Core/Public/UICore.h""")
    }
    
    # Save updated content
    $content | Set-Content $filePath -NoNewline
    Write-Host "Updated includes in: $filePath" -ForegroundColor Green
}

# Find all header and source files
$files = Get-ChildItem -Path "Source" -Recurse -Include *.h,*.cpp

# Update includes in each file
foreach ($file in $files) {
    Update-IncludePaths $file.FullName
}

Write-Host "`nInclude path updates complete!" -ForegroundColor Green
