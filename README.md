# polygon_mesh

1. All Vertex Attributes are automatically expanded with z = 0 and w = 1
2. Store vertices as vec3, but declared in shader as vec4
3. Store normal as vec3, but declared in shader as vec3, and expand as vec4(vNormal, 0,0) manually
4. All (vertex and face) normals are computed when reading files and stored in MESH objects
5. Using one vbo and one draw call per mesh

Keyboard Control:

q: quit program
p: toggle perspective/parallel view
a: pause/unpause auto-rotation (animation)
e: enter edge mode
r: enter vertex mod
t: enter filled triangle mode
f: enter flat shading mode
s: enter smooth shading mode
d: enter phong shading mode
up/down: zoom in/out

Extra Credits:

1. two~three xyzdragons could be animated with no significant framerate drop on Qin's macbook pro.
	a) Long reading time (as we are doing all computations while reading)
	b) When more than one xyz dragons are present, framerate drops when switched to vertex mode and especially in edge mode
Tech specs of Qin's macbook:
	2.9 GHz Intel Core i5
	16 GB 1867 MHz DDR3
	Intel Iris Graphics 6100 1536 MB

2. Drag/Release to rotate universe
	a) May require further tuning - it is not easy to maneuver but it is working