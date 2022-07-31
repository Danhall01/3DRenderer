Made by: Daniel Häll

Date: 2022-01-31

Email: Danne.01@hotmail.com

Discord: Dan_#0001

Requirements:
	[ ]Deferred rendering
	[ ]Shadow mapping
	[ ]Level of detail using tessellation
	[x]OBJ-parser
	[ ]Dynamic cubic environment mapping
	[ ]Frustum culling using a quadtree/octree
	[ ]GPU-based billboarded particle system (rain)


Shader Flags:
   Shader Inputlayout
0b 0000  | 0000       : Deferred shaders | pos normal tex





TODO (bigger picture):
- Light buffer
- Shadow Mapping
- LOD
- Frustum Culling
- particle system
- cubemaping
- Showcase tools

TODO (Polish)
- Create an input class for main (camera movement)
- Optimize OBJ parser
- Create a Timer class
- Move DrawTargets into the renderer
- Remove all braches in render loop during release mode
- Remove gbuffer and instead load directly into the arrays

TODO (now):
- phong lighting for every light for each compute shader
- add light to scene
