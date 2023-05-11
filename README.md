
AVC Project plan
# Members
## Jacob Lum

|**Email**|**Discord**|**Role**|
| :- | :- | :- |
|lumjaco@myvuw.ac.nz|Ultra Foot#3495|Software Architect/Coordinator|
## Jesse Teumohenga

|**Email**|**Discord**|**Role**|
| :- | :- | :- |
|teumohjess1@myvuw.ac.nz|usi#3147|Hardware Developer/Tester|
## Luna Wilkes

|**Email**|**Discord**|**Role**|
| :- | :- | :- |
|wilkesluna@myvuw.ac.nz|mayalaran#2878|Software Developer/Tester|


# Software Plan
## <a name="_useful_functions"></a>Useful functions
- [Turn function](#_turn_function.)
- [Straight movement function](#_straight_movement_function)
- [Centring function](#_centring_function)
- [Black pixel detection](#_black_pixel_detection)
- [Quadrant 3 detector](#_quadrant_detector)
- Makefile
## <a name="_quadrant_1"></a>Quadrant 1
- [Gate opener](#_gate_opener)
## Quadrant 2
- <a name="_quadrant_3"></a>Quadrant 2 navigator
## Quadrant 3
- [Branching path detection](#_branching_path_detection)
- [Branching path navigator](#_branching_path_navigator)
- [Branching path solving](#_branching_path_solver)
- <a name="_quadrant_4"></a>[Quadrant 4 detector](#_quadrant_4_detector)
## Quadrant 4
- [Cylinder distance estimator](#_cylinder_distance_estimator)
- [“Ball” pusher](#_ball_pusher)
- [Cylinder approacher](#_cylinder_approacher)
- [Find next cylinder](#_find_next_cylinder)
- [Quadrant 4 solver](#_quadrant_4_solver)
# Hardware Plan
- Support method
- Steering method
- Camera support
- Main chassis
# Road Map

|**Start Date**|**Task**|**Person**|**End Date**|**Outcome**|
| :-: | :-: | :-: | :-: | :-: |
|10/5/23|Project Plan|Luna/Jacob|13/5/23|Project Plan is done to all active team member’s satisfaction.|
|10/5/23|Open gate|Luna|13/5/23|Robot is able to open gate|
|12/5/23|Basic movement functions|Luna|15/5/23|Basic functions for movement (move straight for certain distance, turn left/right by certain amount)|
|12/5/23|Basic black line detection|Luna|17/5/23|Robot is able to detect the black line, and ideally move along it enough to get through quadrant 2|
|12/5/23|Hardware prototype|Jesse|20/5/23|Hardware in good enough shape that features can be tested. This is possibly going to be quite difficult|
|15/5/23|Basic code structure/quadrant detection|Jacob|20/5/23|Main function has structure. It also has decision making to switch algorithms depending on quadrant (complex). This also would include moving the camera once in the 4th quadrant to a vertical position|
|17/5/23|Colour detection and movement|Jesse|23/5/23|Robot is able to build on black line detection and movement functions to detect coloured cylinders and move to them.|
|24/5/23|First full test|Full team |24/5/23|Full test of whole course. 1 week before deadline|
|25/5/23|<p>Testing/Polishing/</p><p>Fixing/Patching Week</p>|Full Team|30/5/23|Continue testing, patching issues, and polishing software and hardware before final deadline|
|10/5/23|Finished!|All|31/5/23-3/6/23|Project is completed and ready for final test|

# <a name="_turn_function"></a>Turn function.
From [useful functions](#_useful_functions)

This function should take in an angle to turn to and turn to it while the robot does not move forward. The direction the robot should turn should be an angle in degrees (Or radians if we want that). This can be achieved by having one wheel turn forward while the other turns backwards which should make the robot turn on the spot. Note that this function can be made before the robot is physically complete and simply calibrated once it’s made.

# <a name="_straight_movement_function"></a>Straight movement function
From [useful functions](#_useful_functions)

This function should make the robot go forwards by a specified amount (shocking I know).

This can be achieved by making the wheels spin in the same direction. Once again, this function can be fully calibrated once the robot is made.

# <a name="_centring_function"></a>Centring function
From [useful functions](#_useful_functions)

<a name="_gate_opener"></a>This function should take a row and a colour as a parameter. This function should then centre the robot on the line based on where the line is on the row specified. It might be a good idea to break this function up into one which detects the line and one that centres the robot on it as that might be more useful for Quadrant 2.  Take the inner product and then use the turn function based on how much it needs to turn.


# <a name="_black_pixel_detection"></a>Black pixel detection
From [useful functions](#_useful_functions)

Implemented by Jacob on 10/05/2023.

Should take the x and the y of the pixel to check if its black and then return a bool.

Should check if its black by checking if RGB are below a certain thresh hold and are relatively equal.

# <a name="_quadrant_detector"></a>Quadrant 3 detector
From [useful functions](#_useful_functions)

This function should check if the program has reached the start of quadrant 3.

It should return a bool. It seems currently there is a red hexagon which denotes the transition from quadrant 2 to 3. I do not have confirmation that this will stay so if it is removed a different method might be required. But for now it can just keep the camera in place and check if there are a certain number of red pixels. Is there is it should return true.



# Gate opener
From [Quadrant 1](#_quadrant_1)

This function was completed by Luna on 10/05/2023.

This function should open the gate when called. First it connects to the server using the address and pot number. Then it gets the password from the server and sends it back to open the gate.

# Quadrant 2 solver
From Quadrant 2

This function should Using the move forward function and the centering function to navigate through quadrant 1 and quadrant 2. It should also frequently call Quadrant 3 detector to determine if it has change quadrants.
# Branching path detection
From [Quadrant 3](#_quadrant_3)

Potential method for it:

Bounding box. Measure each of the extremes of where black pixels are found (Highest, lowest, leftmost, rightmost.) This allows us to create a bounding box. WE can then use this box to determine which paths are available. This should be relatively simple to code and fast to run. However, it will require another function which will actually execute on the chosen path and follow it. Note this function should not determine which path to take but instead return the available paths.


# <a name="_branching_path_navigator"></a>Branching path navigator
From [Quadrant 3](#_quadrant_3)

This function should take in a direction and move down it in the branching path. For making a turn. Go forwards while repeating the branching path detection function. When the chosen path is no longer detection turn in that direction and then go forward. The centring function should move the robot back on the line afterwards. For going forwards, Keep going forwards until the branching path detection no longer detects a branching path. 

# <a name="_branching_path_solver"></a>Branching path solver
From [Quadrant 3](#_quadrant_3)

This function should navigate through quadrant 3 when called. It should call branching path detector and navigator in order to determine when it has reached a branching path and move through it. This function should determine which path branching path navigator should take.

Methods: Hard code the route. This method should be simple to implement but, is vulnerable to getting lost while navigating. If this occurs the robot won’t be able to pass quadrant 3

Maze navigator. This method would likely be always choosing the leftmost route. This means it could still solve the maze even if the robot somehow got lost. The issue with this method is that it would require a special function to detect when the robot reaches the correct end otherwise the robot will simply ignore the end of the quadrant. The robot will also require a method for navigating dead ends in the maze which the previous wouldn’t. We could solve the end detection issue by raising the camera and looking for the green cylinder which should be directly visible from the correct end of the path.
# <a name="_quadrant_4_detector"></a>Quadrant 4 detector
From [quadrant 3](#_quadrant_3)

This function should raise the camera (Increase the pitch). Until it is perpendicular to the ground. This function should return a Boolean. It should look count the number of green pixels (The green cylinder is directly visible from the end of Q3) and it they are above a certain threshold return true.


# <a name="_cylinder_distance_estimator"></a>Cylinder distance estimator
From [quadrant 4](#_quadrant_4)

This function should estimate the distance from the cylinder based on the number of orange, green, or blue pixels. This function should take the type of cylinder it is looking for as an input and return the number of pixels of that type as an output. \

# <a name="_ball_pusher"></a>Ball pusher
From [quadrant 4](#_quadrant_4)

This function should make the robot keep approaching the ball when called until it pushes it off the edge of the course.


# <a name="_cylinder_approacher"></a>Cylinder approacher
From [quadrant 4](#_quadrant_4)

This function should be given a colour as an input and approach the cylinder matching it but not touch it. It should use the distance estimator to estimate the distance then move towards it and keep doing that until it reaches the required range.



# <a name="_find_next_cylinder"></a>Find next cylinder
From [quadrant 4](#_quadrant_4)

This function should take a colour of the expected cylinder and turn the robot around until it has a certain number of pixel of that type on camera. Function should normally have cylinder approacher called after it to center the robot on the cylinder.


# <a name="_quadrant_4_solver"></a>Quadrant 4 solver
From [quadrant 4](#_quadrant_4)

This function should navigate the robot through quadrant 4. It should make the robot move near each cylinder in order using cylinder approacher and find next cylinder to navigate near them. Then it should identify the “ball” using find next cylinder then use the ball pusher function to push it off the edge. 

