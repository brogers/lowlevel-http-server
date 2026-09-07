# cJSON (vendored)

- Version: **v1.7.19**
- Upstream: https://github.com/DaveGamble/cJSON
- License: MIT (see `LICENSE`)

Only `cJSON.c` / `cJSON.h` are vendored (not `cJSON_Utils`). Build wiring lives in
`vendor/CMakeLists.txt`; first-party code includes it flat as `<cJSON.h>`.

## Updating

```sh
tag=v1.7.19
base=https://raw.githubusercontent.com/DaveGamble/cJSON/$tag
curl -sSL -o vendor/cjson/cJSON.c "$base/cJSON.c"
curl -sSL -o vendor/cjson/cJSON.h "$base/cJSON.h"
curl -sSL -o vendor/cjson/LICENSE "$base/LICENSE"
```

Then bump the version above and the comment in `vendor/CMakeLists.txt`.
