|**Start Date**|**Task**|**Person**|**End Date**|**Outcome**|**Progress As Of 22-May**|
| :-: | :-: | :-: | :-: | :-: | :-: |
|10/5/23|Project Plan|Luna/Jacob|13/5/23|Project Plan is done to all active team member’s satisfaction.|**Completed**|
|10/5/23|Open gate|Luna|13/5/23|Robot is able to open gate||**Completed**|
|12/5/23|Basic movement functions|Luna|17/5/23|Basic functions for movement (move straight for certain distance, turn left/right by certain amount)|**Completed but needs testing**|
|12/5/23|Basic black line detection|Luna|17/5/23|Robot is able to detect the black line, and ideally move along it enough to get through quadrant 2|**Completed but needs testing**|
|12/5/23|Hardware prototype|Jesse|20/5/23|Hardware in good enough shape that features can be tested. This is possibly going to be quite difficult|**Completed but needs testing**|
|15/5/23|Basic code structure/quadrant detection|Jacob|20/5/23|Main function has structure. It also has decision making to switch algorithms depending on quadrant (complex). This also would include moving the camera once in the 4th quadrant to a vertical position| Main function is currently very clean and well structured. A function to detect when the quadrant change is has been made and will need to be called in quadrant 2 and 3 in order to determine when to move on. The camera move function has not yet been made.  |
|17/5/23|Colour detection and movement|Jesse|23/5/23|Robot is able to build on black line detection and movement functions to detect coloured cylinders and move to them.|| |
|24/5/23|First full test|Full team |24/5/23|Full test of whole course. 1 week before deadline|| |
|25/5/23|<p>Testing/Polishing/</p><p>Fixing/Patching Week</p>|Full Team|30/5/23|Continue testing, patching issues, and polishing software and hardware before final deadline|| |
|10/5/23|Finished!|All|31/5/23-3/6/23|Project is completed and ready for final test|| |

**Analysis of Project Progress**\
*What has gone according to plan*
- A lot of the deadlines pre-May 22 have been completed (although some were slightly behind schedule). 
- A fair bit of development has not gone according to plan, partially due to external limitations, and some due to poor estimations of the project timeline.
- Creating the functions to detect colours has progressed well. Implementing the camera move function has proved to be more difficult than anticipated with determining how much the camera needs to turn. Main function has remained clean. 
- No function for quadrant 3 has been created yet although many functions which would be used in it such as branching path detection.


*Bottlenecks and things slowing development.*
- The primary factor slowing development of the project is our limited team size. With only three people. Our attentions are all quite split, and it's hard to get things done quickly.
- One thing slowing development is the difficulty of working with the raspberry pi. Sometimes it won't be accessible via ssh without doing strange workarounds like ssh-ing into the barrets server first. 
  - Another issue is that sometimes ssh-ing will be impossible unless you briefly attach an hdmi cable to the pi.
- Something else slowing development is that the hardware prototype was not ready for testing until very recently. In retrospect, it would've been ideal to all focus on the hardware at first and then work on the software.

*How to advance the project*
- Test what we have as soon as possible, and put a lot more focus on making the robot is ready to go once the programming is done.
- Don't overcomplicate things. A lot of issues with the project so far has been caused by trying to use concepts and techniques that are too advanced. With a time crunch and limited manpower it's best to keep things simple.