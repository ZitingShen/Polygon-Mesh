# polygon_mesh

1. All Vertex Attributes are automatically expanded with z = 0 and w = 1
2. Store vertices as vec3, but declared in shader as vec4
3. Store normal as vec3, but declared in shader as vec3, and expand as vec4(vNormal, 0,0) manually

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