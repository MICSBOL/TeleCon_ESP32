#!/usr/bin/env bash
# Build user guide, fast guide, firmware-sketch guide, connection-mode guide,
# and the Classic Simple code guide (EN/ES).
# Fast guide is delivered only as fast_guide_eng.pdf / fast_guide_esp.pdf.
# Firmware sketches are delivered as firmware_sketches_eng.pdf / firmware_sketches_esp.pdf.
# Connection modes are delivered as connection_modes_eng.pdf / connection_modes_esp.pdf.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"

build_lang() {
  local lang="$1"
  local fast_alias="$2"
  local sketch_alias="$3"
  local modes_alias="$4"
  local src="$ROOT/$lang/src"
  local build="$ROOT/$lang/build"
  mkdir -p "$build"
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_user_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_user_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_fast_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_fast_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_firmware_sketches.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_firmware_sketches.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_connection_modes.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_connection_modes.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_classic_simple_code.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_classic_simple_code.tex >/dev/null)
  cp "$build/telecon_fast_guide.pdf" "$build/$fast_alias"
  rm -f "$build/telecon_fast_guide.pdf"
  cp "$build/telecon_firmware_sketches.pdf" "$build/$sketch_alias"
  rm -f "$build/telecon_firmware_sketches.pdf"
  cp "$build/telecon_connection_modes.pdf" "$build/$modes_alias"
  rm -f "$build/telecon_connection_modes.pdf"
  echo "OK $lang: telecon_user_guide.pdf + $fast_alias + $sketch_alias + $modes_alias + telecon_classic_simple_code.pdf"
}

build_lang en fast_guide_eng.pdf firmware_sketches_eng.pdf connection_modes_eng.pdf
build_lang es fast_guide_esp.pdf firmware_sketches_esp.pdf connection_modes_esp.pdf
