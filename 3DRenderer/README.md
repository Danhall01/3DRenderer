Made by: Daniel Häll

Date: 2022-01-31

Email: Danne.01@hotmail.com

Discord: Dan_#0001

Requirements:
	[x] Deferred rendering
	[ ] Shadow mapping
	[ ] Level of detail using tessellation
	[x] OBJ-parser
	[ ] Frustum culling using a quadtree/octree
	[ ] Dynamic cubic environment mapping
	[ ] GPU-based billboarded particle system (rain)


Shader Flags:
   Shader Inputlayout
0b 0000  | 0000       : Deferred shaders | pos normal tex

Light types
0 : spotlight
1 : directional Light


TODO (Polish)
- Create an input class for main (camera movement)
- Optimize OBJ parser
- Create a Timer class
- Move DrawTargets into the renderer
- Remove all braches in render loop during release mode
- Remove gbuffer and instead load directly into the arrays

TODO (now):
- remove artifacts from mistake
- create new VS and PS (empty) for shadow pass
- Run the shadow pass without Depth Stencil and OM
- Loop for each light, and add result into shadow map array
- Gather data in CS and calculate shadows there
- read: https://takinginitiative.wordpress.com/2011/05/15/directx10-tutorial-10-shadow-mapping/