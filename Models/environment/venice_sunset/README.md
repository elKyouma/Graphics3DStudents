# Venice Sunset

Cube map faces of the [Venice Sunset](https://polyhaven.com/a/venice_sunset) HDRI by Greg Zaal, from Poly Haven,
licensed [CC0](https://creativecommons.org/publicdomain/zero/1.0/).

The faces were made from the tonemapped 8k JPG with

```sh
python3 scripts/equirect_to_cubemap.py venice_sunset.jpg Models/environment/venice_sunset
```

`px.jpg` ... `nz.jpg` are the faces `GL_TEXTURE_CUBE_MAP_POSITIVE_X` ... `GL_TEXTURE_CUBE_MAP_NEGATIVE_Z`, with +y up
and the sun along -z.
