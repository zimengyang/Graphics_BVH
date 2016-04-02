# Graphics_BVH
CIS 560 project - BVH acceleration structure & Anti-Aliasing

Apply Bounding Volume Hierarchy to accelerate render.

**Sample BVH for Wahoo**
<img src="Renders/BVH_wahoo_level13.jpg" height="600"></img>

**Sample renders**
------------------------------
Anti-aliasing, 1 sample per pixel VS 4 sample per pixel<br>
<img src="./Renders/wahoo_1sample_random_1538ms.bmp" height="300"></img>
<img src="./Renders/wahoo_16sample_uniform_25021ms.bmp" height="300"></img>

<br>

Dragon with reflective brdf material: 400px*400px, render time: 3291ms<br>
<img src="./Renders/dragon_material2_3291ms.bmp" height="600"></img> 
    
<br>

Dragon with normal map and texture: 400px*400px, render time: 2880ms<br> 
<img src="./Renders/dragon_texture_2808ms.bmp" height="600"></img> 
    


