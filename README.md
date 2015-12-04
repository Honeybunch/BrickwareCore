<img src="http://i.imgur.com/83X6Kfn.png" alt="brickware_name" height="100">
<img src="http://i.imgur.com/Qje9ZhI.png" alt="brickware_logo" height="100">

The Core library for [Brickware Engine](http://github.com/Honeybunch/Brickware-Engine)

If you were to build a game with this you would follow the model in the main engine repo and pull this and its dependencies into a new project as submodules.

------

## How to Build

This repo is not meant to be edited on its own but to be included as a submodule in another project. This will not build without being linked against all of the necessary dependencies. The included CMakeLists.txt does not make it easy to setup the project to be built on its own yet.

Yes you do still need to link against the graphics library you're using here and in the project you derive this from because BrickwareGraphics links dynamically by default.

###Dependencies

### Brickware
 * [Brickware Graphics](http://github.com/Honeybunch/BrickwareGraphics)
 * [Brickware Math](http://github.com/Honeybunch/BrickwareMath)
 * [Brickware Utils](http://github.com/Honeybunch/BrickwareUtils)

#### OpenGL
 * GLEW
 * GLFW3
 * GL core library

#### DirectX
 * Windows 8 SDK

#### Logos
Artwork on this README is courtesy of [Kyle Labriola](http://www.kylelabriola.com/)
