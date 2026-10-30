# SpidyVR Webwing — experimental glider preview

Unofficial GPL-3.0 custom build based on iplusdvr/iplusds-SpidyVR and IlyaMez/SpidyVR.

## Included

- Complete Windows PC VR mod package with the optional web-wing locomotion INI.
- Full corresponding modified source and original license/notices.
- Deliberate arms-open deployment, gradual fall recovery, horizontal steering,
  speed limits, pose hysteresis and immediate cancellation on tracking loss,
  web use, landing or suspended flight ownership.
- Additional regression tests and an additions-only source audit.

## Install

Extract the complete player ZIP into a new folder. Run the included launcher
with your supported Spider-Man Remastered game and OpenXR headset setup.
The INI belongs beside `build/windows-ninja/spidy_ray_bridge.dll`; the player
ZIP contains it there. Set `Enabled=0` and restart the session to disable.
Use [the glider guide](GLIDER.md) for the gesture and limitations.

## Test status and limitations

Windows VR/Observer build succeeded. Four CTest targets passed, including 14
glider cases; existing core cases (378), launcher checks (24), automatic rotation,
and all six Python protocol suites (133 tests) passed.

Real game/headset testing is pending. This is a prerelease, not a verified
headset-ready release. No visible wing model is included. Gliding is available
only in flight already controlled by the swing solver after releasing a web.
The standard launcher's update action can replace custom binaries.

## Separate Turkish resources

- [Calypso Turkish text/subtitle patch](https://www.calypsoceviri.com/turkce-yamalar/marvels-spider-man-remastered-turkce-yama-indir)
- [TanProduction dubbed story video](https://www.youtube.com/watch?v=4XCW4cMBtRw)

These third-party files are not redistributed here. The patch has not been
tested together with Webwing, and the dubbed video is not a game voice pack.

No game assets are distributed. This custom build is not affiliated with the
original mod authors, Marvel, Sony, Insomniac Games or Nixxes.
