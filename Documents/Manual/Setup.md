## Setup

Make sure View Plugin Content is enabled in your view options.

### Game Framework Setup

1. Create a player controller deriving from RTSPlayerController.
1. Create a player state deriving from RTSPlayerState.
1. Create a game state deriving from RTSGameState.
1. Create a game mode deriving from RTSGameMode.
1. Use your player controller, player state and game state in your game mode.
1. Create an AI controller deriving from RTSPawnAIController.
1. Set the Pawn Behavior Tree Asset of the new controller to BT_RTSPawnBehaviorTree.
1. Set the Pawn Blackboard Asset of the new controller to BB_RTSPawnBlackboard.
1. Create a player start deriving from RTSPlayerStart.
1. Create one or more resource types deriving from RTSResourceType.

### Camera Setup

#### Creating The Camera

1. Create a pawn blueprint.
1. Add a Camera component.
1. Set the Location of the Camera component as desired (e.g. X = 0, Y = 0, Z = 1500).
1. Set the Rotation of the Camera component as desired (e.g. X = 0, Y = -75, Z = 0).
1. Use the pawn as Default Pawn Class in your game mode.

#### Setting Up Camera Movement

1. Bind the axis MoveCameraLeftRight (e.g. to Left and Right keys).
1. Bind the axis MoveCameraUpDown (e.g. to Up and Down keys).
1. Bind the axis ZoomCamera (e.g. to the mouse wheel axis).
1. At your RTSPlayerController, set the CameraSpeed (e.g. to 1000).
1. At your RTSPlayerController, set the CameraScrollThreshold (e.g. to 20).
1. At your RTSPlayerController, set CameraZoomSpeed, MinCameraDistance and MaxCameraDistance as desired.

### Input Setup

At Edit > Project Settings > Engine > Input ...

1. Bind the action Select (e.g. to left mouse button).