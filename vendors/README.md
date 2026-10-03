# Legacy dependencies

* `STLport-4.5.3`: original upstream 4.5.3 tarball preserved in the
  [Debian archive](https://archive.debian.org/debian/pool/main/s/stlport4.5/stlport4.5_4.5.3.orig.tar.gz).
  Archive SHA-256: `8299d3cf53546cb61d68fe6d719e13cf4ca705a6dcda5a80335e915d3ea96194`.
  EA's repository-supplied `stlport.diff` has been applied to `stlport/` with
  `git apply --ignore-space-change -p0` (the patch uses Windows line endings).
  Copyright and license notices remain in the original headers and README.
* `zlib-1.1.4`: original release from the [zlib archive](https://zlib.net/fossils/zlib-1.1.4.tar.gz).
  Archive SHA-256: `9e3e973174f9910fd51539ef9ce94c86a3943d4f897fab8e9adf4b19e6a8291e`.
  Sources are unchanged and compiled with the original `Z_PREFIX` definition.
  The license is in `README` and `zlib.h`.
* `gamespy-2011`: API headers from the original BSD-licensed GameSpy SDK,
  [mirror commit d1deb2d](https://github.com/nitrocaster/GameSpy/tree/d1deb2d1a951cf77933dda040b8d311cc09815a7).
  Archive SHA-256: `20a5cf85e855ae085364bb125083d0a605a5bcb491d524da3406d4a0e8d87463`.
  Headers only, for the offline build. License: `gamespy-2011/LICENSE`.

DirectX, Miles and enabled proprietary integrations remain external SDKs.
