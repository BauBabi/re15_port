$gh = "C:\Users\mjoedicke\Downloads\ghidra_11.4.2_PUBLIC_20250826\ghidra_11.4.2_PUBLIC\support\analyzeHeadless.bat"
$root = "C:\workspace\git\reAi"
foreach ($n in 2,3,4,5,6) {
  $img = "$root\.agent_refs\stage${n}_reloc.bin"
  if (-not (Test-Path $img)) { continue }
  $proj = "$root\.agent_refs\ghr_$n"
  if (Test-Path $proj) { Remove-Item -Recurse -Force $proj }
  New-Item -ItemType Directory -Force $proj | Out-Null
  $perdir = "$root\RE_15_Quellcode_Overlays\STAGE${n}_full"
  if (Test-Path $perdir) { Remove-Item -Recurse -Force $perdir }
  New-Item -ItemType Directory -Force $perdir | Out-Null
  $out = "$root\.agent_refs\stage${n}_decomp.c"
  $a = @($proj,"s$n","-import",$img,"-processor","MIPS:LE:32:default","-loader","BinaryLoader","-loader-baseAddr","0x80100000","-scriptPath","$root\scripts","-postScript","ghidra_decomp_dump.py",$out,"0x80100000","0x80130000",$perdir,"-deleteProject")
  & $gh @a *> "$root\.agent_refs\ghr_$n.log"
  "STAGE$n done: $((Get-ChildItem $perdir\*.c -ErrorAction SilentlyContinue).Count) funcs" | Out-File -Append "$root\.agent_refs\ghr_progress.txt"
}
"ALL RELOCATED OVERLAYS DECOMPILED" | Out-File -Append "$root\.agent_refs\ghr_progress.txt"
