$ErrorActionPreference = 'Stop'
$editorPath = 'C:\Program Files\Epic Games\UE_5.6\Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$projectPath = 'C:\Unreal_Project\CX\CX.uproject'
$ownedProcesses = @()
try {
    $serverArgs = @($projectPath, '/Game/VehicleTemplate/Maps/VehicleAdvExampleMap', '-server', '-nullrhi', '-unattended', '-nosplash', '-port=17777', '-abslog=C:\Unreal_Project\CX\Saved\Logs\CXNetServer.log')
    $server = Start-Process -FilePath $editorPath -ArgumentList $serverArgs -WindowStyle Hidden -PassThru
    $ownedProcesses += $server
    Write-Output ('Started test server PID ' + $server.Id)
    $deadline = (Get-Date).AddSeconds(45)
    do {
        Start-Sleep -Milliseconds 500
        $ready = (Test-Path 'Saved/Logs/CXNetServer.log') -and (Select-String -Path 'Saved/Logs/CXNetServer.log' -Pattern 'listening on port 17777' -Quiet)
    } until ($ready -or $server.HasExited -or (Get-Date) -gt $deadline)
    if (!$ready) { throw 'Server did not start listening' }
    foreach ($clientNumber in 1..2) {
        $clientArgs = @($projectPath, ('127.0.0.1:17777?Name=CXTest' + $clientNumber), '-game', '-nullrhi', '-unattended', '-nosplash', ('-abslog=C:\Unreal_Project\CX\Saved\Logs\CXNetClient' + $clientNumber + '.log'))
        $client = Start-Process -FilePath $editorPath -ArgumentList $clientArgs -WindowStyle Hidden -PassThru
        $ownedProcesses += $client
        Write-Output ('Started test client PID ' + $client.Id)
    }
    $deadline = (Get-Date).AddSeconds(45)
    do {
        Start-Sleep -Milliseconds 500
        $joined = @(Select-String -Path 'Saved/Logs/CXNetServer.log' -Pattern 'Join succeeded:').Count
    } until ($joined -ge 2 -or (Get-Date) -gt $deadline)
    if ($joined -lt 2) { throw ('Only ' + $joined + ' clients joined') }
    Write-Output 'Two separate clients joined server successfully'
    Start-Sleep -Seconds 5
    foreach ($logName in 'CXNetServer','CXNetClient1','CXNetClient2') {
        $fatal = Select-String -Path ('Saved/Logs/' + $logName + '.log') -Pattern 'Fatal error|Unhandled Exception|Ensure condition failed|LogScript: Warning: Accessed None|LogNet: Warning:.*RPC'
        if ($fatal) { $fatal | ForEach-Object { Write-Output $_.Line }; throw ('Runtime errors in ' + $logName) }
    }
    Write-Output 'CX_NETWORK_SMOKE PASS: 2 clients, no fatal/ensure/script/RPC errors'
} finally {
    foreach ($ownedProcess in $ownedProcesses) {
        if (!$ownedProcess.HasExited) { Stop-Process -Id $ownedProcess.Id -ErrorAction SilentlyContinue }
    }
}
