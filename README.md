![banner](https://i.imgur.com/EGGX3NS.png)

# Description
Very simple dll hook to address some of the aspect ratio issues in Koei Tecmo's Warriors Orochi 4.

This is a very early build with just the most minor and simple patches.

## Features

- Hooks to the game camera's projection matrix and stretches the projection to a given aspect ratio.
- Scans the current screen resolution to automaticaly figure out the aspect value it must patch.

Cons:
- It doesn't apply any patches to the actual rendering buffer.
- It doesn't apply any patches to the in-game UI.

# How to Install
- Drag and Drop the dinput8.dll next to the game's exe.
- Start the game.
- Run [Borderless Gaming](https://github.com/andrewmd5/Borderless-Gaming/releases).
- Add the process *WO4.exe* to Borderless Gaming.
- Enjoy!

## Compatibility
This patch is compatible with ZMOD. In order to run both at the same time: rename zmod's *dinupt8.dll* to *zmod.dll* and use my dinput8.dll.

 ## Recommendations
This patch tweaks the camera's projection matrix, not the render buffer. This means the game will run at stretched 16:9 instead of a 1:1 relation with your actual current monitor resolution. The miss-match will generate a minor quality loss in resolution.

This can be mitigated by using Lossless Scaling.
