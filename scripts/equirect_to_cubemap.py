#!/usr/bin/env python3
"""Converts an equirectangular (latitude-longitude) panorama into the six faces of an OpenGL cube map.

The faces are written as px.jpg, nx.jpg, py.jpg, ny.jpg, pz.jpg and nz.jpg (positive/negative x, y, z) and are meant to
be uploaded, without flipping, to GL_TEXTURE_CUBE_MAP_POSITIVE_X ... GL_TEXTURE_CUBE_MAP_NEGATIVE_Z. The +y axis points
up and the centre of the panorama is seen when looking along -z.
"""

import argparse
from pathlib import Path

import numpy as np
from PIL import Image

Image.MAX_IMAGE_PIXELS = None

parser = argparse.ArgumentParser(description="Convert an equirectangular panorama into cube map faces")
parser.add_argument("panorama", help="equirectangular image, twice as wide as high")
parser.add_argument("out_dir", help="directory for the six face images")
parser.add_argument("-s", "--size", type=int, default=1024, help="width and height of each face in pixels")
parser.add_argument("-q", "--quality", type=int, default=90, help="JPEG quality")
args = parser.parse_args()

pano = np.asarray(Image.open(args.panorama).convert("RGB"), dtype=np.float32)
pano_h, pano_w, _ = pano.shape

# Texture coordinates (s, t) of the face pixel centres, mapped to [-1, 1]. Row 0 of an uploaded image is t = 0.
c = (np.arange(args.size, dtype=np.float32) + 0.5) / args.size * 2.0 - 1.0
tc, sc = np.meshgrid(c, c, indexing="ij")
one = np.ones_like(sc)

# The inverse of the face selection table of the OpenGL specification (section "Cube Map Texture Selection"):
# the direction (x, y, z) that is looked up at the face coordinates (sc, tc).
faces = {
    "px": (one, -tc, -sc),
    "nx": (-one, -tc, sc),
    "py": (sc, one, tc),
    "ny": (sc, -one, -tc),
    "pz": (sc, -tc, one),
    "nz": (-sc, -tc, -one),
}


def sample(u, v):
    """Bilinear lookup in the panorama, u in [0, 1) wraps around, v in [0, 1] goes from the top to the bottom."""
    x = u * pano_w - 0.5
    y = np.clip(v * pano_h - 0.5, 0, pano_h - 1)
    x0 = np.floor(x).astype(int)
    y0 = np.floor(y).astype(int)
    fx = (x - x0)[..., None]
    fy = (y - y0)[..., None]
    x1 = (x0 + 1) % pano_w
    x0 = x0 % pano_w
    y1 = np.minimum(y0 + 1, pano_h - 1)
    top = pano[y0, x0] * (1 - fx) + pano[y0, x1] * fx
    bottom = pano[y1, x0] * (1 - fx) + pano[y1, x1] * fx
    return top * (1 - fy) + bottom * fy


out_dir = Path(args.out_dir)
out_dir.mkdir(parents=True, exist_ok=True)
for name, (x, y, z) in faces.items():
    norm = np.sqrt(x * x + y * y + z * z)
    x, y, z = x / norm, y / norm, z / norm
    # Longitude zero (the centre of the panorama) along -z, longitude growing towards +x.
    u = 0.5 + np.arctan2(x, -z) / (2 * np.pi)
    v = np.arccos(np.clip(y, -1.0, 1.0)) / np.pi
    face = np.clip(sample(u, v) + 0.5, 0, 255).astype(np.uint8)
    Image.fromarray(face).save(out_dir / f"{name}.jpg", quality=args.quality)
    print(f"Wrote {out_dir / name}.jpg")
