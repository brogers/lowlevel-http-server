# Vendored: Unity

[Unity](https://github.com/ThrowTheSwitch/Unity) test framework, **v2.7.0**
(MIT, see `LICENSE.txt`).

Only the three files needed to build and use the framework are vendored:

```
unity.c
unity.h
unity_internals.h
```

## Updating

```sh
V=vX.Y.Z
for f in src/unity.c src/unity.h src/unity_internals.h LICENSE.txt; do
  curl -sSfLo "tests/vendor/unity/$(basename "$f")" \
    "https://raw.githubusercontent.com/ThrowTheSwitch/Unity/$V/$f"
done
```

Then bump the version noted above and in `tests/CMakeLists.txt`.
