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
- Shadow mapping

- Do normal application initialization (create device and output swap chain).
- Create shadow map render target (probably good to start out with the R32 DXGI format texture)
- For frame rendering, do the shadow pass and write only depth into the shadow map. The view/projection matrices are determined by the location and nature of your light.
- Then bind your output render target, and bind the shadow map as a shader resource view to your pixel shader. Sample it and do the depth comparison to determine if it is in shadow or not.