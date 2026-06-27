macroScript RE2MVAS category:"RE2MV"
tooltip:"RE2MV Animation Script..." Icon:#("Maxscript", 3)
(
rollout Main_Form "RE2MVAS v0.1b" width:416 height:172
(
button btn1 "Import ANI" pos:[8,8] width:128 height:24
button btn2 "Export ANI" pos:[144,8] width:128 height:24 enabled:false
button btn3 "About" pos:[280,8] width:128 height:24
progressBar pb1 "ProgressBar" pos:[136,40] width:272 height:14
label lbl1 "Part: 0" pos:[8,40] width:56 height:16
label lbl2 "Frame: 0" pos:[72,40] width:56 height:16
---Import ANI---
on btn1 pressed do
(
	---Global Variable---
global Part_Count 
global Part_Link_Count
global Part_Index
global Frame_Count
global All_Object
global POS_File
---Open File---
POS_File = getOpenFileName types:"RE2MV ANI(*.ANI)|*.ANI|"
if POS_File != undefined then 
(
--POS_File = openFile "C:\LEON.POS"
POS_File = openFile POS_File
----Set Part---
All_Object = geometry as array
---Part Count---
Part_Count = readValue POS_File
lbl1.caption = "Part: " + Part_Count as String
---Reset Part Angle---
fn Reset_Part pt =
(
All_Object[pt].rotation.x_rotation = 0
All_Object[pt].rotation.y_rotation = 0
All_Object[pt].rotation.z_rotation = 0
)
---Reset All Part Angle And Position---
for i = 1 to Part_Count do 
(
Reset_Part i
All_Object[i].pos = [0, 0, 0]
deleteKeys All_Object[i] #allKeys
)
---Link Part---
for i = 1 to Part_Count do 
(
Part_Link_Count = readValue POS_File
for ii = 1 to Part_Link_Count do 
(
if Part_Link_Count > 0 then 
(
Part_Index = readValue POS_File
All_Object[Part_Index +1].parent = All_Object[i]
)
)
)
---Set Part Pos---
for i = 1 to Part_Count do 
(
local pos_x = readValue POS_File
local pos_y = readValue POS_File
local pos_z = readValue POS_File
move All_Object[i] [pos_x, pos_y, pos_z]
)
---Rotate Part Angle---
fn Rotate_Part pt =
(
local ang_x = readValue POS_File
local ang_y = readValue POS_File
local ang_z = readValue POS_File
rotate All_Object[pt] (angleaxis ang_x [1,0,0])
rotate All_Object[pt] (angleaxis ang_z [0,0,1])
rotate All_Object[pt] (angleaxis ang_y [0,1,0])
)
---Animate Part---
Frame_Count = readValue POS_File
lbl2.caption = "Frame: " + Frame_Count as String
Frame_Count -= 1
animationRange = interval 0 Frame_Count
with animate on
(
---Main Body Position---
for i = 0 to Frame_Count do
( 
local pos_x = readValue POS_File
local pos_y = readValue POS_File
local pos_z = readValue POS_File
at time i All_Object[1].pos = [pos_x, pos_y, pos_z]
---Reset Angle---
for ii = 1 to Part_Count do at time i Reset_Part ii
---Read Angle---
Temp = Part_Count
while Temp > 0 do
( 
at time i Rotate_Part Temp
Temp -= 1
)
---Proggres---
pb1.value = 100.*i/Frame_Count 
)
)
sliderTime = 0
----Close File----
close POS_File
)
)
---Export ANI---
on btn2 pressed do
(
	
)
---About---
on btn3 pressed do
(
MessageBox "RE2MVAS v0.1b By Leo2236!" title: "About" beep: false
)
)
createdialog Main_Form width:416 height:62
)
