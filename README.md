# polygon_mesh

1. All Vertex Attributes are automatically expanded with z = 0 and w = 1
2. Store vertices as vec3, but declared in shader as vec4
3. Store normal as vec3, but declared in shader as vec3, and expand as vec4(vNormal, 0,0) manually