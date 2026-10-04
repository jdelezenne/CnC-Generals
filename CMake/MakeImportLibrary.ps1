param(
    [Parameter(Mandatory = $true)][string]$Dll,
    [Parameter(Mandatory = $true)][string]$Output
)
$ErrorActionPreference = 'Stop'
$dllPath = (Resolve-Path -LiteralPath $Dll).Path
$outputPath = [IO.Path]::GetFullPath($Output)
$headers = & dumpbin /headers $dllPath
if ($LASTEXITCODE -ne 0 -or -not ($headers -match '14C machine')) {
    throw 'Expected a Win32/x86 DLL and a working VC6 dumpbin.'
}
$exports = & dumpbin /exports $dllPath
if ($LASTEXITCODE -ne 0) { throw 'dumpbin failed.' }
$names = @($exports | ForEach-Object {
    if ($_ -match '^\s+(\d+)\s+[0-9a-f]+\s+[0-9a-f]+\s+(\S+)') {
        $ordinal = $matches[1]
        $name = $matches[2]
        # VC6 LIB adds the leading x86 underscore. Import by the DLL's ordinal
        # so that the loader still resolves the original decorated export.
        if ($name -match '^_.+@\d+$') { $name = $name.Substring(1) }
        "$name @$ordinal NONAME"
    }
})
if ($names.Count -eq 0) { throw 'No named exports found.' }
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $outputPath) | Out-Null
$defPath = [IO.Path]::ChangeExtension($outputPath, '.def')
@(('LIBRARY "' + [IO.Path]::GetFileName($dllPath) + '"'), 'EXPORTS') + $names |
    Set-Content -LiteralPath $defPath -Encoding ASCII
& lib /nologo "/def:$defPath" /machine:I386 "/out:$outputPath"
if ($LASTEXITCODE -ne 0) { throw 'VC6 lib failed.' }
