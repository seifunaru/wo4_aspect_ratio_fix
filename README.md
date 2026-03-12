![banner](https://i.imgur.com/EGGX3NS.png)

<h1>Description</h1>
Very simple dll hook to address some of the aspect ratio issues in Koei Tecmo's Warriors Orochi 4.
This is a very early build with just the most minor and simple patches.

<h2>Features</h2>

- Hooks to the game camera's projection matrix and stretches the projection to a given aspect ratio.
- Scans the current screen resolution to autommaticaly figure out the aspect value it must patch.

Cons:
- It doesn't apply any patches to the actual rendering buffer.
- It doesn't apply any patches to the in-game UI.

<h1>How to Install</h1>
Drag and drop the dinput8.dll next to your game's executable (next to WO4.exe, root game folder).  

<h2>Compatibility</h2>
This patch is compatible with ZMOD. In order to run both at the same time: rename zmod's *dinupt8.dll* to *zmod.dll* and use my dinput8.dll.
