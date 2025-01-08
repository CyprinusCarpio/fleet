## FLEET
**FLEET** is a *fleet* of extra additions for the FLTK library.

It's designed to build on the capabilities of FLTK 1.4 to add new, universal and useful widgets.

#### Disclaimer
This is pre-Alpha software. It has a few bugs and many things are not yet implemented. Breaking changes are guaranteed in future. Documentation is sparse. Use for any reason other than testing is not recommended.

#### New widgets:
- `Fle_Flat_Button`  
A button with hot tracking, that draws a frame only when the user is interating with it.
- `Fle_Dock_Host`  
A complex group that allows the program user to change it's appearance by moving/detaching/attaching various groups at different places.
- `Fle_Dock_Group`  
A container for any FLTK widget that allows it to be attached/detached to a `Fle_Dock_Host`. There are vertical and horizontal dock groups.

#### Docking in-depth
FLEET brings the widget docking ability known from other toolkits to FLTK. At the core of this design is the `Fle_Dock_Host`, which governs the positioning, scale and behaviour of the attached groups, and contains the **work widget** in the middle. The dock host can allow attaching of groups in 4 **directions**: top, right, bottom, left. The directions are divided into **lines** of dock groups. A dock group has a few key properties:
- Size: the width of the group in the case of horizontal groups, and height in the case of vertical groups.
- Min Size: the dock host is designed so as not to allow scaling a group below this size.
- Preferred Size: when the user changes the size of a group by moving/attaching it, it becomes the preferred size for the group. The dock host will not scale it above this size unless it's required to fill empty space in a line.
- Breadth: the height of the group in the case of horizontal groups, and the width in the case of vertical groups.
- Min Breadth: the dock host is designed so as not to allow scaling a group below this breadth.
- Detachability: a detachable group may be detached from the dock host and live in it's own window that can be moved around.
- Flexibility: a flexible group allows the user to change it's breadth.

![image](./docking_guide.png)

All groups in a single line must be either flexible or not, therefore a line in itself can be called flexible or not.

There are several ways for the user to change those properties:
- Scaling between groups: the user can drag the border between two groups to move them.
- Scaling between lines: the user can drag the border between two lines. This will change the breadth of all the groups in both affected lines. However, this cannot change the total breadth of a direction. Flexible lines are not affected by this.
- Scaling the work widget: the user can drag the border of the work widget. This allows for the total breadth of a direction to change. For this to be allowed, at least one of the lines on the choosen direction must be flexible.

If a group is detachable, it may be detached by the user and floated in a non-modal window. When the user is moving the detached group, a timer is set. Once it elapses, a check is made to determine if and where the detached group should be attached. If the group is non-detachable, then if re-attaching it normally fails, the group is repositioned inside the host automatically, making it impossible to permanently detach it, only move it around.

#### Code style
The preferred style for FLEET development is Allman style indentation, snake_case and no `auto` keyword.

#### Usage
Currently FLEET is heavily WIP, and exists as a simple demo program to test and develop it's features. 

The only dependency is FLTK 1.4.
