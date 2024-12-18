# Setup development environment for VersaUI

# Check prerequisites
function Check-Prerequisites {
    Write-Host "Checking prerequisites..."
    
    # Check Node.js
    try {
        $nodeVersion = node --version
        Write-Host "Node.js version: $nodeVersion"
    }
    catch {
        Write-Error "Node.js is not installed!"
        exit 1
    }
    
    # Check npm
    try {
        $npmVersion = npm --version
        Write-Host "npm version: $npmVersion"
    }
    catch {
        Write-Error "npm is not installed!"
        exit 1
    }
}

# Setup development environment
function Setup-DevEnvironment {
    Write-Host "Setting up development environment..."
    
    # Install dependencies
    npm install
    
    # Setup git hooks
    npx husky install
    
    # Build development version
    npm run build:dev
    
    # Run tests
    npm test
}

# Initialize documentation
function Init-Documentation {
    Write-Host "Setting up documentation..."
    
    # Create necessary directories
    New-Item -ItemType Directory -Force -Path "docs/api"
    New-Item -ItemType Directory -Force -Path "docs/examples"
    New-Item -ItemType Directory -Force -Path "docs/tutorials"
    
    # Generate API documentation
    npm run docs
}

# Main setup process
try {
    Check-Prerequisites
    Setup-DevEnvironment
    Init-Documentation
    
    Write-Host "Development environment setup complete!"
    Write-Host "You can now start developing with 'npm run dev'"
}
catch {
    Write-Error "Setup failed: $_"
    exit 1
} 