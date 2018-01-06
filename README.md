![alt text](https://i.imgur.com/on6yZiF.png)

# TickTickBoom

UE4 Gamejam winner January 2016

Michael Allar playthrough:
https://youtu.be/AL5_Fe3GXqE

UE4 Community livestream:
https://youtu.be/LoH0fD1kcRo?t=28m37s



### Notes:

Updated to UE4.18.0.  Rebuilt and polished with Blueprints since the game jam entry, no C++ necessary, however it does require Allegorithmics Substance plugin.  Blueprints are tidy and well commented, should answer most questions about flow.

BP_GameMode initializes and progresses all things in the game.  Check out BeginPlay().

All assets are assembled and animated procedurally.  See BP_Gameboard constructor for assembly.  BP_Button's handle their own animation via timelines.

BP_CameraActor moves and zooms to actors screen space bounding boxes.  Similar to zoom extents functions in editor and most 3D programs.
