# Web-wing glider — additive custom build

This is a modified, unofficial build of iplusd's SpidyVR fork. Original authors,
copyright, GPL-3.0 license and version numbers are unchanged.

## Enable and fly

Copy `spidy-glider.ini` next to `build/windows-ninja/spidy_ray_bridge.dll` in
your extracted mod folder, then start a new VR session. The custom player ZIP
already includes this file. Delete no game files. `Enabled=0` disables the feature;
without this INI it is also disabled. Restart the session after changing it.

1. Swing on a web, build up forward momentum, then release both webs.
2. While descending, spread both hands to either side of your head, approximately
   shoulder height (at least 1.05 metres apart), for 0.2 seconds. Keep grip and
   trigger buttons released. The glider opens over another 0.3 seconds.
3. Look horizontally where you want to turn; the movement stick adds steering.
   Turns are limited to about 80 degrees per second, without rolling the camera.
4. Bring your hands together, lower them, or press a grip/trigger to fold.
   Web firing, climbing, landing and loss of tracking/focus also fold it.

The glider preserves horizontal speed; it cannot launch you, climb indefinitely,
or accelerate you forward. At less than 4 m/s horizontal speed it stalls.
Its default descent is 4 m/s. Recovery from a fast fall is gradual: leave plenty
of height. The existing overall swing speed limit still applies.

## Boundaries

This version adds locomotion, not visible wing meshes or a new game animation.
The native game adapter only deploys during a flight the swing already owns,
after web release. It does not take over ordinary jumps or native freefall.
Collision remains entirely with the existing game movement module. No game
addresses, wire-format structs, headset projection or old menu rows are changed.

The standalone solver supports the same movement in simulations. Automated
checks cover deployment, cancellation, stalls, descent, speed, steering, frame
rates, native authoritative positions and collisions. Automated checks cannot
verify headset comfort, game animation or real game collision behaviour.

## Build and review

Use the existing bootstrap/build scripts with `-Observer`. On a machine with
newer Visual Studio Build Tools, `tools/build-glider.ps1` discovers the toolchain
and calls the existing native build without modifying it. CTest includes the new
`glider` target. Run every `tests/*_protocol_tests.py` as well.

`tools/audit-additions.py` verifies every tracked original file is still an ordered
subsequence of its Git HEAD version (no original line removed or replaced),
checks the diff and prints the SHA-256 of each added/extended file.
