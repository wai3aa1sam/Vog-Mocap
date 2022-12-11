# Vog-Mocap

Vog-Mog is a demo application that based on my game engine Vog.<br />
Motion capture (Mocap) is my FYP topic.<br />
Currently only support OpenGL.<br />
Kinect and IMU are used for the motion capture hardwares<br />

Features:<br />
• Deferred-Rendering<br />
• Entity-Component-System(ECS)<br />
• Basic Physics system <br />
• Basic animation loading<br />
• Model loading<br />
• Shading features (Bloom / Blur, Shadow Mapping, Trail)<br />
• Simple user Interface<br />
• Basic character control system with serial data<br />

The Demo game:

png:
![alt text](https://github.com/wai3aa1sam/Vog-Mocap/blob/main/media/png/demo_game_debug.png?raw=true)
![alt text](https://github.com/wai3aa1sam/Vog-Mocap/blob/main/media/png/demo_game2.png?raw=true)

gif:
(with our motion capture device)
![](https://github.com/wai3aa1sam/Vog-Mocap/blob/main/media/gifs/demo-game.gif)

(editor test)
![](https://github.com/wai3aa1sam/Vog-Mocap/blob/main/media/gifs/demo-game-raw.gif)

system-overview: 
![alt text](https://github.com/wai3aa1sam/Vog-Mocap/blob/main/media/png/system-overview-v2.png?raw=true)

How-to-build
only Windows support
Windows: 
1. clone the project
2. open generateProjects.bat
3. open and compile the visual studio project
4. done<br />
**default premake setting do not include Kinect SDK

References: <br />
• Mostly Shading Features: 
- https://learnopengl.com

• Engine Architecture and main design: 
- https://github.com/TheCherno/Hazel

• OpenGL Init References:
- https://github.com/Eversmile12/OpenGLNative/blob/master/OpenGL/OpenGL/src/WinMain.cpp
- https://github.com/SimpleTalkCpp/SimpleTalkCpp_Tutorial/blob/main/Graphic/Advance%20OpenGL/Advance%20OpenGL%20003/MyOpenGLWindow.h

• ECS:
- https://wickedengine.net/2019/09/29/entity-component-system/
- https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/
- https://austinmorlan.com/posts/entity_component_system/#the-component
- https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html

• Physics:
- Physics for Game Developers, 2nd Edition
- Game Physics Cookbook (2017)
- https://blog.winter.dev/2020/designing-a-physics-engine/
- https://wickedengine.net/2020/04/26/capsule-collision-detection/

• Kinect References: 
- Microsoft Kinect v2 Native sample
- https://github.com/UnaNancyOwen/Kinect2Sample
- https://gist.github.com/UnaNancyOwen/e0421bbf25e7fe527764
