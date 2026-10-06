# Dependency installation

- The repository owner installs and upgrades system dependencies with `apt` on Linux and `brew` on macOS.
- Agents may inspect installed/available packages and provide exact installation commands. Do not run package installation or upgrade commands yourself.
- Prefer the required system dependency setup. Ask the owner before substituting a workaround, patching a vendor for a toolchain incompatibility, or suppressing a diagnostic to make a build pass.
