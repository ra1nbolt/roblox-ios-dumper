# RobloxDumper

An iOS tweak that extracts useful offsets and runtime information from the
Roblox client. RobloxDumper locates `RobloxLib` in memory, resolves known
signatures, and exports the results in both C++ header and JSON formats.

> [!WARNING]
> This project is intended for security research and educational use. Roblox
> updates may change internal structures and break existing signatures. Use it
> only on devices and software you are authorized to inspect.

## Features

- Resolves Roblox and Luau offsets from ARM64 signatures
- Extracts client product, version, and Git hash metadata
- Dumps selected TaskScheduler, Instance, Job, and Lua state fields
- Produces ready-to-use `.h` and `.json` output files
- Writes a timestamped execution log for troubleshooting

## Requirements

- A jailbroken arm64e iOS device capable of running the Roblox app
- [Theos](https://theos.dev/docs/installation)
- A working roothide-compatible Theos toolchain
- `make`, Git, and an SSH connection to the target device

The project vendors Capstone and nlohmann/json. It also expects
[fmt](https://github.com/fmtlib/fmt) in `lib/fmt`.

## Building

1. Clone the repository:

   ```sh
   git clone https://github.com/ra1nbolt/roblox-ios-dumper.git
   cd roblox-ios-dumper
   ```

2. If `lib/fmt` is empty, clone fmt into it:

   ```sh
   git clone https://github.com/fmtlib/fmt.git lib/fmt
   ```

3. Update the local values at the top of `makefile`:

   ```make
   export THEOS = /path/to/theos
   THEOS_DEVICE_IP = your.device.ip.address
   ```

4. Build the package:

   ```sh
   make package
   ```

5. Install it on the configured device:

   ```sh
   make install
   ```

The tweak targets the `com.roblox.robloxmobile` bundle and restarts Roblox
after installation.

## Output

Launch Roblox after installing the tweak. Generated files are written to the
app's Documents directory:

```text
Documents/
├── dump/
│   ├── dump_<version>.h
│   └── dump_<version>.json
└── log_<date>_<time>.txt
```

The JSON output includes client metadata and categorized Roblox/Luau offsets.
The header exposes the same offsets as `uintptr_t` constants under the
`offsets` namespace.

## Updating signatures

Signature definitions live in `src/signatures/signatures.cpp`. They are tied to
Roblox's current ARM64 binary and may need to be updated after a client release.
Check the generated log when an address cannot be resolved.

## Contributing

Issues and pull requests are welcome. When reporting a broken signature,
include the Roblox app version, Git hash, device architecture, and relevant log
messages. Do not attach copyrighted application binaries.
