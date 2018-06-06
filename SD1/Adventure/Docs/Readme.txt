Know Issue:
- Just today I found out that because of Map::GetEntityTypeFromDefinitionName() function, newly added Actors
in XML wont work. I hardcoded return EntityType with this function, which was dumb.
  ( Take a look at Maps.cpp: Line 255 )
- HeatMapGeneration could be optimised. It is lagging in Debug Build.


How to Use:
- Use XboxController's Left stick to move, A-Button to shoot
- Start_Button to Pause the Game
- Right_Shoulder_Button for Victory GameState
- Left_Shoulder_Button for Defeat GameState


Things completed from Gold_AssignmentDoc:
[x] Refector class Actor 
	[x] Understand how Goblin works, in your code
	[x] Determine how you will retain same functionallity in class Actor
	[x] Implement it
[x] Understand glDrawArray
[x] Implement glDrawArray
[x] Optimize Map::RenderTiles()
[x] Entity sorted draw order
[x] Fix Entity sort thing..
----------------------------
Total: 20 Points

[x] Fix Game Crash on debug
[x] Fix Entity deletion on death
[x] Two sprites per tile		   - 05
	[x] Get TileDefination populating correctly
	[x] Change XML Data according to "Tarrain_32x32.png"
	[x] Make it work
[x] Implement Tags class		   - 05
[x] Use tag in Tiles and Entities classes
[x] Understand TileExtraInfo structure
[x] Write class TileExtraInfo		   
[x] Use TileExtraInfo with Tiles	   - 03
[x] HeatMap				   - 03
[x] Basic AI Behaviour			   - 10
	[x] Create Simple AIBehaviour classes for Wander, Follow & Shoot
	[x] Set that classes with Actor
	[x] Make UtilitySystem work
	[x] Make AIBehaviour work
	[x] Fetch Behaviours tag from XML
	[x] Use that data to setup AIBehaviours
---------------------------
Total: 26 Points


[~] AI Pathfinding ( using Dijkstra )	   - 05
	[x] Figure out how you will generate HeatMaps (going for Approach 2)
	[x] Figure out where you will store it (every actor should have a pointer to its heatMap)
	[x] When you'll update it (every frame I will update it)
	[x] How you'll access it (From actor's pointer I will access it)
	[x] Create HeatMap on Player
	[x] Display HeatMap on Map's Tiles 
	[x] How you'll use it?
	[~] Implement it in AI_Behavior
[~] MapGenStep Spawn Actor		   - 03
	[x] Have it working under Adventure
	[ ] Make a MapGenStep for same task
----------------------------
Total: 8 Points (Max)

Grand Total: 54 Points (Max)