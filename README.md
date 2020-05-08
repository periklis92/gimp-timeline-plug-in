# GIMP Timeline
## A plug-in for GIMP 2.0 that inserts a timeline in your workspace in order to make animation easier

#### How to use:
<p>
1.Create a new image file. <br>
2.Go to Filters/Animation/Timeline... <br>
3.Click on Add Keyframe to add a new keyframe and start drawing. <br>
</p>

#### Installation:
##### Linux:
<p>Running the make file will build and install 
</p>

#### General Instructions:
<p>
  
***Frames*** are layer groups created by the plug-in.<br>
However if you add a layer group yourself it won't be recognized by the plug-in.<br>

***Onion Skin*** will make the previous frame (layer group) slightly visible.<br>
You can modify its opacity with the slider underneath.

***Remove Keyframe*** will remove the currently selected keyframe.

***Play*** opens up a new window that render the frame in order.

***Create Sprite Sheet*** will create a new image where the layers will be positioned one after the other.<br>
The number next to the button indicates how many image will be put in a row before a new row is made.

***Copy last*** will copy the last frame in the timeline whenever you create a new frame.
</p>

!["Preview"](/timeline-preview.png)
