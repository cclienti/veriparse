#!/usr/bin/env bash
# Render a docs/ markdown file to PDF: A4, DejaVu fonts, 2-level TOC, and a
# thin rule between table rows (pandoc's booktabs default draws none).
#
# Usage: scripts/adr-pdf.sh docs/adr-NNNN-name.md [output.pdf]
# Needs pandoc and xelatex on the host.

set -euo pipefail

src=${1:?usage: adr-pdf.sh <doc.md> [out.pdf]}
out=${2:-${src%.md}.pdf}
src=$(realpath "$src")
out=$(realpath -m "$out")

build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT

# A dashed row separator drawn with leaders, glued to the row above it
# (penalty 10000 on both sides of the box, a break allowed only after):
# longtable then keeps the dash at the bottom of a broken page and never
# repeats it under the continuation header, which arydshln does.
cat > "$build/header.tex" <<'LATEX'
\newcommand{\rowdash}{\noalign{%
  \penalty10000\vskip2.5pt
  \hbox to\hsize{\leaders\hbox to 5pt{\hss\rule{2.5pt}{0.4pt}\hss}\hfill}%
  \penalty10000\vskip2.5pt\penalty0}}
LATEX

pandoc "$src" -s -t latex -o "$build/doc.tex" \
    --include-in-header "$build/header.tex" \
    --shift-heading-level-by=-1 \
    --toc --toc-depth=2 \
    -V mainfont="DejaVu Serif" \
    -V monofont="DejaVu Sans Mono" \
    -V monofontoptions="Scale=0.80" \
    -V papersize=a4 \
    -V geometry:margin=1.5cm \
    -V fontsize=10pt \
    -V colorlinks=true

# Insert the dashed separator after every table body row except the last,
# whose separation is already the table's bottom rule.
python3 - "$build/doc.tex" <<'EOF'
import sys

path = sys.argv[1]
lines = open(path).read().split("\n")
out = []
i = 0
n = len(lines)
while i < n:
    line = lines[i]
    if line.strip().startswith(r"\begin{longtable}"):
        block = [line]
        i += 1
        marker_idx = None
        while i < n and not lines[i].strip().startswith(r"\end{longtable}"):
            block.append(lines[i])
            if lines[i].strip() in (r"\endlastfoot", r"\endhead"):
                marker_idx = len(block) - 1
            i += 1
        end_line = lines[i] if i < n else r"\end{longtable}"
        start = (marker_idx + 1) if marker_idx is not None else 1
        row_ends = [j for j in range(start, len(block))
                    if block[j].rstrip().endswith("\\\\")]
        for j in row_ends[:-1]:
            block[j] = block[j] + "\n\\rowdash"
        out.extend(block)
        out.append(end_line)
        i += 1
        continue
    out.append(line)
    i += 1

open(path, "w").write("\n".join(out))
EOF

# Twice: the first run only collects the TOC entries.
(cd "$build" && xelatex -interaction=nonstopmode doc.tex >/dev/null
 xelatex -interaction=nonstopmode doc.tex >/dev/null)

cp "$build/doc.pdf" "$out"
echo "$out"
