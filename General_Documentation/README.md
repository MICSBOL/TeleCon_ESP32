# TeleCon4ESP32 user documentation

Five documents (EN + ES each):

| Document | Role | Source | Delivered PDF |
|----------|------|--------|---------------|
| **User guide** | Full product doc | `*/src/telecon_user_guide.tex` | `*/build/telecon_user_guide.pdf` |
| **Fast guide** | First Classic Simple session (Android Codes asset) | `*/src/telecon_fast_guide.tex` | `en/build/fast_guide_eng.pdf`, `es/build/fast_guide_esp.pdf` |
| **Firmware sketches** | Which `.ino` to flash (Bluetooth DevKit, CAM video-only, one-board CAM) | `*/src/telecon_firmware_sketches.tex` | `en/build/firmware_sketches_eng.pdf`, `es/build/firmware_sketches_esp.pdf` |
| **Connection modes** | What each phone connection setting means | `*/src/telecon_connection_modes.tex` | `en/build/connection_modes_eng.pdf`, `es/build/connection_modes_esp.pdf` |
| **Classic Simple code** | Line-level guide of `TeleCon_Classic_Simple` and where to edit it | `*/src/telecon_classic_simple_code.tex` | `*/build/telecon_classic_simple_code.pdf` |

`fast_guide_eng.pdf` / `fast_guide_esp.pdf` and
`firmware_sketches_eng.pdf` / `firmware_sketches_esp.pdf` are the names the
Android app loads from assets. `connection_modes_eng.pdf` /
`connection_modes_esp.pdf` are the connection-mode guides. LaTeX still
compiles from `telecon_fast_guide.tex`, `telecon_firmware_sketches.tex`,
`telecon_connection_modes.tex`, and `telecon_classic_simple_code.tex`.

Firmware authors: see [Protocol_Documentation](../Protocol_Documentation/README.md).
Arduino pin maps and `YOUR CODE HERE` stay in the sketch `README.md` next to
the `.ino`.

Screenshots live in `figures/` (shared) plus language-specific overrides in
`figures/en/` and `figures/es/` (e.g. S04/S05/S10/S13). The `\shot` macro
prefers the language folder, then the shared file. After saving desktop
captures, round corners with:
`python3 scripts/mask_window_corners.py path/to/shot.png`
(or `--all` for every `figures/**/*.png` under this repo).

Captured so far include S01 home, S02 codes, S02b serial, S03 settings,
S04 Bluetooth enable dialog, S05 Bluetooth scan, S06 connected home,
S07 control panel, S08 help, S09 Wi‑Fi SoftAP, S10 panel joystick settings,
S11 center picker, S12 RC vehicle, S13 vehicle settings, S14 coin unlock,
S15 coin pricing. S04/S05/S09/S10/S13/S15 have separate EN and ES UI captures.

## Build PDF

```bash
./build_guides.sh
```

Or manually:

```bash
cd en/src
pdflatex -output-directory=../build telecon_user_guide.tex
pdflatex -output-directory=../build telecon_fast_guide.tex
pdflatex -output-directory=../build telecon_firmware_sketches.tex
pdflatex -output-directory=../build telecon_firmware_sketches.tex
pdflatex -output-directory=../build telecon_connection_modes.tex
pdflatex -output-directory=../build telecon_connection_modes.tex
cp ../build/telecon_fast_guide.pdf ../build/fast_guide_eng.pdf
cp ../build/telecon_firmware_sketches.pdf ../build/firmware_sketches_eng.pdf
cp ../build/telecon_connection_modes.pdf ../build/connection_modes_eng.pdf
rm -f ../build/telecon_fast_guide.pdf ../build/telecon_firmware_sketches.pdf ../build/telecon_connection_modes.pdf

cd ../../es/src
pdflatex -output-directory=../build telecon_user_guide.tex
pdflatex -output-directory=../build telecon_fast_guide.tex
pdflatex -output-directory=../build telecon_firmware_sketches.tex
pdflatex -output-directory=../build telecon_firmware_sketches.tex
pdflatex -output-directory=../build telecon_connection_modes.tex
pdflatex -output-directory=../build telecon_connection_modes.tex
cp ../build/telecon_fast_guide.pdf ../build/fast_guide_esp.pdf
cp ../build/telecon_firmware_sketches.pdf ../build/firmware_sketches_esp.pdf
cp ../build/telecon_connection_modes.pdf ../build/connection_modes_esp.pdf
rm -f ../build/telecon_fast_guide.pdf ../build/telecon_firmware_sketches.pdf ../build/telecon_connection_modes.pdf
```
