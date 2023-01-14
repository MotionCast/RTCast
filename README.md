# RTCast

A custom WebRTC module using [`libdatachannel`][libdc] to connect a [Pilot][pilot] with [Linear][linear].


## Requirements

- C++17
- CMake >=3.10
- [libdatachannel][libdc]
- [googletest][gtest]


## Build

```bash
$ cmake -B build -DBUILD_TESTS=1 -DBUILD_DOCS=1
$ cmake --build build # will build docs in ./docs
```


## Testing

```bash
# Build first, then run the following
$ ctest --test-dir build --output-on-failure
```


## Authorship

Jacob SP \<jacob@moca.st>


## Licence

![GPLv2][shield-gpl]

This software is distributed under the [General Public License v2.0][license], more information available at the [Free Software Foundation][gnu].

<!-- LINKS -->

[libdc]: https://github.com/paullouisageneau/libdatachannel	"libdatachannel"
[gtest]: https://github.com/google/googletest	"gtest"

[license]: LICENSE "General Public License"
[gnu]: https://www.gnu.org/licenses/old-licenses/gpl-2.0.html "Free Software Foundation"

<!-- MOCAST -->
[pilot]: https://github.com/MotionCast/Pilot	"Pilot"
[linear]: https://github.com/MotionCast/Linear	"Linear"

<!-- SHIELDS -->

[shield-gpl]: https://img.shields.io/github/license/MotionCast/RTCast
