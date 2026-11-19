#!/bin/sh
# levtest.sh N : load level N (0-based, DTU.MSN order) by patching savegame 0, walk around, log
N=$1; D=lev$N; G=game-$D
cd "$(dirname "$0")"; rm -rf "$D" "$G"; cp -r "${PRISTINE:-pristine-full}" "$G"
python3 -c "
d=bytearray(open('$G/SG0.SG','rb').read()); d[41]=$N+1; open('$G/SG0.SG','wb').write(d)"
timeout 80 ./runtime/build/dtu-headless --game "$PWD/$G" --cd "$PWD/$G/_CD/DTU/_CDFILES" --out "$D" --seconds 40 --shots 3 --fps 60 \
  --key 5:1C --key 7:1C --key 9:1C --key 22:148:3 --key 25.5:14D:1.0 --key 27:148:2 --key 29.5:1E:1.0 --key 31:14B:2.0 --key 33.5:148:3 -- -notitles ${NOSOUND--nosound} >/dev/null 2>&1
echo "$N exit $?"; rm -rf "$G"
