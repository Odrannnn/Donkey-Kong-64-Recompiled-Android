[CmdletBinding()]
param(
    [string] $WorkspaceRoot,
    [switch] $Clean
)

# Historical compatibility entry point. The implementation is version-driven.
& (Join-Path $PSScriptRoot 'Build-Release.ps1') @PSBoundParameters
exit $LASTEXITCODE
