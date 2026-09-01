[CmdletBinding()]
param(
    [string] $WorkspaceRoot,
    [switch] $Clean
)

# The implementation derives the manifest version, protocol and tick export at
# runtime. Keep this stable entry point in documentation and automation.
& (Join-Path $PSScriptRoot 'Build-Release-Impl.ps1') @PSBoundParameters
exit $LASTEXITCODE
