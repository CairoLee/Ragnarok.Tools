Modes:

Right mouse button is to move around

rightmousebutton + drag             move
rightmousebutton + drag + shift     rotate camera
RMB + drag + shift  +ctrl           rotate camera (other axes)
O                                   show/hide objects
winkey+w                            show/hide water
l                                   show/hide lightmaps
insert/delete                       change the viewinglevel of the quadtree
U                                   undo
T                                    open texture dialog

Texture Edit
------------
Select a part of a texture on the right and click on the map to put it there
+ / - (non-numpad)                  scale the brush
[ / ]                               scroll through the textures
spacebar                            rotate the current brush
h                                   flip the brush horizontally
v                                   flip the brush vertically
g                                   show / hide grid
backspace                           clears a part of the floor (for indoors)

Global heightmap edit
---------------------
left-drag                           select an area
ctrl+left-drag                      add an area to the selection
alt+left-drag                       remove an area from the selection
pageup                              raise selection
pagedown                            lower selection

Detail heightmap edit
---------------------
leftclick                           raise brush
rightclick+leftclick                lower brush
ctrl+leftclick                      raise brush with maximum
ctrl+rightclick+leftclick           lower brush with minimum
c                                   copy
p                                   paste
f                                   merge cellheights (used for sloping)
s                                   smoooooooooothen the tiles under the current brush (could crash at edges of the map)


Wall edit
---------
,                                   insert wall on the horizontal axes
.                                   insert wall on the vertical axes
h                                   flip walltexture horizontally (on the horizontal axes)
ctrl+h                              flip walltexture on entire wall horizontally (on the horizontal axes)
v                                   flip walltexture vertically (on the horizontal axes)
ctrl+v                              flip walltexture on entire wall vertically (on the horizontal axes)
alt+h                               flip walltexture horizontally (on the vertical axes)
alt+ctrl+h                          flip walltexture on entire wall horizontally (on the vertical axes)
alt+v                               flip walltexture vertically (on the vertical axes)
alt+ctrl+v                          flip walltexture on entire wall vertically (on the vertical axes)

pageup                              move right part of walltexture to the right
pagedown                            move right part of walltexture to the left
home
end

c                                   copy
p                                   paste
g                                   mark cells with a wall purple (to detect 0-height walls)

leftclick                           mark a cell as "bottom"
rightclick                          mark a cell as "top"
w                                   align textures on the horizontal axes
alt+w                               align textures on the vertical axes
                                    note on aligning: if no markers are placed, the texture is vertically scaled from the highest border 'till 
                                    the lowest border because RO doesn't support vertical texture repeating. If the markers are placed, then
                                    it's scaled from bottom to top




Object edit
-----------
ctrl+leftclick                      add object
c                                   copy model, size & rotation
p                                   paste model, size & rotatation
leftclick                           select object
rightclick                          deselect object
backspace                           delete model
left-drag                           move selected object
shift+left-drag                     move selected object (on grid)
ctrl-left-drag                      rotate selected object
shift+ctrl-left-drag                rotate selected object (on grid)
alt-left-drag                       scale selected object
pageup/pagedown                     move on y-axis
left/right                          move on x-axis
up/down                             move on z-axis
ctrl+pageup/pagedown                rotate on y-axis
ctrl+left/right                     rotate on x-axis
ctrl+up/down                        rotate on z-axis
alt+pageup/pagedown                 scale on y-axis
alt+left/right                      scale on x-axis
alt+up/down                         scale on z-axis
o                                   show object handles (red blocks)
ctrl+p                              paste height
shift+p                             paste new object with the same height as the copied object
r                                   reset rotation
1,2,3,4,5,6,7,8,9,0                 Change grid-size
shift+1,2,3,4,5,6,7,8,9,0           Change grid-x-offset
ctrl+1,2,3,4,5,6,7,8,9,0            Change grid-y-offset
Enter                               Open object properties window


gat edit
--------
left-click                          change brush to selected tile
[,]                                 change selected tile
pageup/pagedown                     move gat tile up/down
c                                   copy height
p                                   paste height
+/-                                 change brushsize
space                               map the gattiles to the floor on the brush


effects edit
------------
left-click                          select an effect
rightclick                          deselect effect
ctrl+click                          make a new effect
backspace                           delete an effect
pageup / pagedown                   change the effect looping-time (check console for the time)
drag                                move an effect
ctrl+drag                           move an effect up / down
Enter                               Open effects properties window

group object edit
-----------------
in Select mode:

left-drag                           make a new selection
ctrl+left-drag                      add objects to the selection
alt+left-drag                       remove objects from the selection
[ / ]                               switch select/edit mode

In Editing mode:
left-drag                           move the selection
ctrl+left-drag                      rotate the selection around it's center
alt+left-drag                       scale the selection around it's center
D                                   duplicate the selection

Light Edit
----------
left-click                          select a light
rightclick                          deselect effect
left-drag                           move a light
ctrl+left-drag                      move a light up/down
Enter                               open light dialog