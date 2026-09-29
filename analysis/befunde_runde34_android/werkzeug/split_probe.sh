set -euo pipefail
HERE="$1"
source "$HERE/python_finden.sh" || { echo "kein Python"; exit 9; }
eval "$(awk '/^die\(\) \{/{print} /^verify_split\(\) \{/{f=1} f{print} f&&/^}$/{f=0}' "$HERE/make_package.sh")"
cd "$(dirname "$0")"
rc=0; verify_split re15_port_v0.8.19_android.zip 1 || rc=$?; echo "  verify_split android (1): rc=$rc"
rc=0; verify_split re15_port_v0.8.19_linux_steamdeck_x64.zip 3606 || rc=$?; echo "  verify_split linux (3606): rc=$rc"
rc=0; verify_split re15_port_v0.8.19_linux_steamdeck_x64.zip 9999 || rc=$?; echo "  verify_split linux (9999, Negativ): rc=$rc"
rc=0; "$PY" "$HERE/zip_exec_bit.py" pruefen re15_port_v0.8.19_linux_steamdeck_x64.zip re15_pc run.sh || rc=$?; echo "  zip_exec_bit pruefen linux: rc=$rc"
rc=0; "$PY" "$HERE/zip_exec_bit.py" pruefen re15_port_v0.8.19_android.zip re15_pc || rc=$?; echo "  zip_exec_bit pruefen android re15_pc (Negativ): rc=$rc"
mv re15_port_v0.8.19_android.z01 weg.z01
rc=0; verify_split re15_port_v0.8.19_android.zip 1 || rc=$?; echo "  verify_split android ohne .z01 (Negativ): rc=$rc"
mv weg.z01 re15_port_v0.8.19_android.z01
