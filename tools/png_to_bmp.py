"""PNG -> 24-bit BMP converter for the WinAPI engine.

The engine loads only 24-bit BMP (LoadImageW) and treats magenta (255, 0, 255)
as transparent (TransparentBlt). Animator::CreateAnimations skips .png files,
so a BMP must sit next to every PNG that should be animated.

    python -I tools/png_to_bmp.py <folder> [--force] [--dry-run]

- Walks <folder> recursively and writes <name>.bmp next to each <name>.png.
- Pixels with alpha < 128 become magenta; all others become opaque RGB.
- An opaque pixel that is exactly magenta is nudged to (254, 0, 254) so it
  does not turn transparent by accident.
- Existing BMPs are skipped unless --force is given. PNGs are never deleted.
"""
import argparse
import os
import sys

from PIL import Image

MAGENTA = (255, 0, 255)
NUDGED = (254, 0, 254)
ALPHA_CUTOFF = 128


def convert(png_path, bmp_path):
    """Write bmp_path. Returns (transparent pixel count, nudged pixel count)."""
    with Image.open(png_path) as src:
        rgba = src.convert("RGBA")

    src_bytes = rgba.tobytes()
    rgb = bytearray(len(src_bytes) // 4 * 3)
    transparent = 0
    nudged = 0
    for i in range(len(src_bytes) // 4):
        r, g, b, a = src_bytes[i * 4:i * 4 + 4]
        if a < ALPHA_CUTOFF:
            r, g, b = MAGENTA
            transparent += 1
        elif (r, g, b) == MAGENTA:
            r, g, b = NUDGED
            nudged += 1
        rgb[i * 3:i * 3 + 3] = bytes((r, g, b))
    out = Image.frombytes("RGB", rgba.size, bytes(rgb))
    out.save(bmp_path, "BMP")  # RGB mode -> 24-bit, uncompressed
    return transparent, nudged


def main():
    parser = argparse.ArgumentParser(description="Convert PNGs to magenta-keyed 24-bit BMPs next to them.")
    parser.add_argument("folder")
    parser.add_argument("--force", action="store_true", help="overwrite BMPs that already exist")
    parser.add_argument("--dry-run", action="store_true", help="only list what would be written")
    args = parser.parse_args()

    if not os.path.isdir(args.folder):
        print("not a folder: %s" % args.folder, file=sys.stderr)
        return 2

    converted = skipped = nudged_files = 0
    total_bytes = 0
    for root, dirs, files in os.walk(args.folder):
        dirs.sort()
        for name in sorted(files):
            base, ext = os.path.splitext(name)
            if ext.lower() != ".png":
                continue
            png_path = os.path.join(root, name)
            bmp_path = os.path.join(root, base + ".bmp")
            if os.path.exists(bmp_path) and not args.force:
                print("skip (exists) %s" % bmp_path)
                skipped += 1
                continue
            if args.dry_run:
                print("would write %s" % bmp_path)
                converted += 1
                continue

            transparent, nudged = convert(png_path, bmp_path)
            size = os.path.getsize(bmp_path)
            total_bytes += size
            converted += 1
            note = ""
            if nudged:
                nudged_files += 1
                note = "  (%d opaque magenta pixels nudged to 254,0,254)" % nudged
            print("wrote %s  %d bytes, %d transparent px%s" % (bmp_path, size, transparent, note))

    verb = "would write" if args.dry_run else "wrote"
    print("%s %d BMP(s), skipped %d, %d bytes total, %d file(s) with nudged magenta"
          % (verb, converted, skipped, total_bytes, nudged_files))
    return 0


if __name__ == "__main__":
    sys.exit(main())
