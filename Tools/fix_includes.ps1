# Fix include paths in all source files
$sourceDir = "Source"
$files = Get-ChildItem -Path $sourceDir -Recurse -Include *.cpp,*.h

Write-Host "Starting include path fixes..."
$fixCount = 0

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    $originalContent = $content
    
    # Fix duplicate Runtime references
    $content = $content -replace '#include "Runtime/Runtime/Core/', '#include "Runtime/Core/'
    $content = $content -replace '#include "Runtime/Runtime/Framework/', '#include "Runtime/Framework/'
    $content = $content -replace '#include "Runtime/Runtime/UI/', '#include "Runtime/UI/'
    
    # Fix missing Runtime prefix
    $content = $content -replace '#include "Core/Public/', '#include "Runtime/Core/Public/'
    $content = $content -replace '#include "Framework/Public/', '#include "Runtime/Framework/Public/'
    $content = $content -replace '#include "UI/Public/', '#include "Runtime/UI/Public/'
    
    # Fix corrupted line breaks in includes
    $content = $content -replace '#include "R\r?\n\r?\n#include', '#include'
    
    # Fix relative paths that should be from Runtime
    $content = $content -replace '#include "\.\./Core/', '#include "Runtime/Core/'
    $content = $content -replace '#include "\.\./Framework/', '#include "Runtime/Framework/'
    $content = $content -replace '#include "\.\./UI/', '#include "Runtime/UI/'
    
    # Fix paths with extra slashes
    $content = $content -replace 'Runtime//Core', 'Runtime/Core'
    $content = $content -replace 'Runtime//Framework', 'Runtime/Framework'
    $content = $content -replace 'Runtime//UI', 'Runtime/UI'
    
    # Fix paths with missing Public/Private
    $content = $content -replace '#include "Runtime/Core/([^/]+\.h)"', '#include "Runtime/Core/Public/$1"'
    $content = $content -replace '#include "Runtime/Framework/([^/]+\.h)"', '#include "Runtime/Framework/Public/$1"'
    $content = $content -replace '#include "Runtime/UI/([^/]+\.h)"', '#include "Runtime/UI/Public/$1"'
    
    # Write back only if content changed
    if ($content -ne $originalContent) {
        Set-Content -Path $file.FullName -Value $content -NoNewline
        $fixCount++
        Write-Host "Fixed includes in: $($file.Name)"
    }
}

Write-Host "Include paths have been fixed in $fixCount files." 