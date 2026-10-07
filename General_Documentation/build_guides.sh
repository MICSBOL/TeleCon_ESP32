#!/usr/bin/env bash
# Build user guide, fast guide, and firmware-sketch guide (EN/ES).
# Fast guide is delivered only as fast_guide_eng.pdf / fast_guide_esp.pdf.
# Firmware sketches are delivered as firmware_sketches_eng.pdf / firmware_sketches_esp.pdf.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"

build_lang() {
  local lang="$1"
  local fast_alias="$2"
  local sketch_alias="$3"
  local src="$ROOT/$lang/src"
  local build="$ROOT/$lang/build"
  mkdir -p "$build"
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_user_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_user_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_fast_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_fast_guide.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_firmware_sketches.tex >/dev/null)
  (cd "$src" && pdflatex -interaction=nonstopmode -output-directory="$build" telecon_firmware_sketches.tex >/dev/null)
  cp "$build/telecon_fast_guide.pdf" "$build/$fast_alias"
  rm -f "$build/telecon_fast_guide.pdf"
  cp "$build/telecon_firmware_sketches.pdf" "$build/$sketch_alias"
  rm -f "$build/telecon_firmware_sketches.pdf"
  echo "OK $lang: telecon_user_guide.pdf + $fast_alias + $sketch_alias"
}

build_lang en fast_guide_eng.pdf firmware_sketches_eng.pdf
build_lang es fast_guide_esp.pdf firmware_sketches_esp.pdf
