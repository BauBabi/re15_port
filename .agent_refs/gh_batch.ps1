$gh = "C:\Users\mjoedicke\Downloads\ghidra_11.4.2_PUBLIC_20250826\ghidra_11.4.2_PUBLIC\support\analyzeHeadless.bat"
$root = "C:\workspace\git\reAi"
foreach ($s in @("STAGE2","STAGE3","STAGE4","STAGE5","STAGE6","DEBUG")) {
  $bin = "$root\info\Re1.5\PSX\BIN\$s.BIN"
  if (-not (Test-Path $bin)) { continue }
  $raw = "$root\.agent_refs\${s}_stripped.bin"
  $bytes = [System.IO.File]::ReadAllBytes($bin)
  [System.IO.File]::WriteAllBytes($raw, $bytes[0x800..($bytes.Length-1)])
  $proj = "$root\.agent_refs\gh_$s"
  if (Test-Path $proj) { Remove-Item -Recurse -Force $proj }
  New-Item -ItemType Directory -Force $proj | Out-Null
  $perdir = "$root\RE_15_Quellcode_Overlays\${s}_full"
  $out = "$root\.agent_refs\${s}_decomp.c"
  $a = @($proj,$s,"-import",$raw,"-processor","MIPS:LE:32:default","-loader","BinaryLoader","-loader-baseAddr","0x80100000","-scriptPath","$root\scripts","-postScript","ghidra_decomp_dump.py",$out,"0x80100000","0x80130000",$perdir,"-deleteProject")
  & $gh @a *> "$root\.agent_refs\gh_$s.log"
}
"ALL OVERLAYS DONE" | Out-File "$root\.agent_refs\gh_batch_done.txt"
