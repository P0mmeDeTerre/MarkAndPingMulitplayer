# MarkAndPingMulitplayer

<details>
   <summary>Mark Team component</summary>
   
A `Mark Team Component` must be used in a `GameMode`
To add the component, click the `Add` button on the `Components` window in your gamemode.

![image](https://github.com/user-attachments/assets/c9a2a593-2708-4d38-8bba-7d69868e2ff9)

and search for `MarkTeam`

![image](https://github.com/user-attachments/assets/3d44b3b9-bd4d-4944-93a7-fc4fcf30c9f6)

A member must be in only one team.

### Add Team
To add a team in your `MarkTeamComponent` you can use the `Add Team` node from the component

![image](https://github.com/user-attachments/assets/845a7638-2b95-4a1c-a103-a7e1437ff24e)

* `Name` : Team name
* `Color` : Color of the ping/mark
* `Icons` : Icons for members
* `Ping Lifetime` : Lifetime of a ping

Icons is an array of textures. This is used when a player from this team pings something. Each player on a team can have their own icon to help differentiate them from one another. This is optional, but you need to provide an array to this function. If no icons are set or not enough icons are provided for the members who need them (e.g., if you set 2 icons but have 3 players on the team), the third player will use the default icon set in `PingComponet`.

### Remove Team
Remove a team from `Mark Team Component`

* `Team Name` : Team name

### Add Member
To add a member to a specific team you can use the `Add Member` node

![image](https://github.com/user-attachments/assets/351720b9-08b9-4409-9559-7d31dad2b36d)

* `Team Name` : Team name
* `Member` : Player Controller reference from the event `OnPostLogin`

You are not required to use the pin from the `OnPostLogin` event, but you must provide a reference to the `PlayerController` of the member.

![image](https://github.com/user-attachments/assets/da6f669b-0bc0-4ddc-b428-32f3526f69d6)

### Remove Member
To remove a member from a team, the function will automatically determine which team the member is in (note that a member can only belong to one team).
![image](https://github.com/user-attachments/assets/4e735be1-c01f-42d1-a1f2-aeaeb7171712)

* `Member` : A reference to the `PlayerController` belonging to the member

![image](https://github.com/user-attachments/assets/1c6ec2ea-66a3-48b7-b846-0b764d336a5a)

</details>

<details>
   <summary>Ping Component</summary>

   The `PingComponent` must be placed in the `PlayerController` of the player.

   ![image](https://github.com/user-attachments/assets/6e9847d1-c759-42e6-9215-d8b9a5530549)
   
   The only settings of the `PingComponent` is the `IconMaterial`
   
   ![image](https://github.com/user-attachments/assets/9fab236b-e0b6-4a44-aa26-89d7b54a4707)
   
   You can replace the `IconMaterial` by your own one but you need to keep those settings
   This material is only used for your own ping. If you want to change all material, you also need to change it on the `MarkComponent`
   
    * `Texture` : Texture Parameter
    * `Color` : Vector Parameter
      
   </details>


<details>
   <summary>Mark Component</summary>
This component is used to send a mark/ping to the server.

### Settings

![image](https://github.com/user-attachments/assets/83aea200-d6bc-40fc-99e2-ea021fc5266e)

* `Ping Material Instance` : The material instance for other player's ping
    * `Texture` : Texture Parameter
    * `Color` : Vector Parameter

* `Ping Color` : Color of the ping (only if `Use Team Ping Color` is set to false).
* `Cooldown` : Cooldown between to ping/mark.
* `Length` : Length of line trace (starting from your camera).
* `Debug Draw Line` : Show a debug line trace (only on the server).
* `Use Team Ping Color` : If set to true, the ping will use the color of the team.
* `Use Team Mark Color` : If set to true, the mark will use the color of the team.
* `Ping Sound` : Play a sound when ping.
* `Ping Widget Settings` : Settings of the ping widget
   * `Offset` : Offset of the widget (default is 0.5 0.5 (centered))
   * `Icon Size` : Size of the icon
   * `Space` : Space between icon and text
   * `Is Text Visible` : If true, the widget will display a text below the icon showing the distance in meters.
   * `Deciaml Places` : Number of decimal places to display (e.g. 2 will display '3.87 M').
   * `Update Time` : Time interval for updating the widget.


### Events

![image](https://github.com/user-attachments/assets/d1fb1ef4-ae9c-4ff2-aec4-b9a21d36c0b2)

* `OnTeamUpdate` : Called when the server update the icons member
* `OnPing` : Called when the player ping

### Functions

#### `Ping`
![image](https://github.com/user-attachments/assets/51d231f8-b0d5-45ca-93c6-655e763f0c55)

Sends a ping request to the server. If the line trace hit results detect a `MarkedComponent`, the ping will turn into a mark.

 * `Follow Actor` : Follows the actor who is marked (no effect if it’s a ping).


#### `GetWidgetComponent`
Return the `WidgetComponent` of the `MarkComponent` (not from the `PlayerController`)

#### `GetLocalPingWidget`
Return the widget from the `PlayerController`

#### `GetPingMaterialInstance`
Return the `MaterialInstance` of the ping

#### `GetPingColor`
Return the ping color

#### `IsTeamPingColorEnabled`
Return UseTeamPingColor

#### `IsTeamMarkColorEnabled`
Return UseTeamMarkColor

#### `UpdatePingWidgetTeamColor`
Set new value for `UseTeamPingColor` and force update on pings

#### `UpdatePingWidgetTeamColor`
Set new value for `UseTeamMarkColor` and force update on marks

#### `UpdateSettings`
Update settings on player controller's widget and all other ping widgets
   * `Offset` : Offset of the widget
   * `Icon Size` : Size of the icon
   * `Space` : Space between icon and text
   * `Is Text Visible` : If true, the widget will display a text below the icon showing the distance in meters.
   * `Deciaml Places` : Number of decimal places to display (e.g. 2 will display '3.87 M').
   * `Update Time` : Time interval for updating the widget.

</details>

<details>
   <summary>Marked Component</summary>

Use this component on actors that you want to mark.

### Settings
![image](https://github.com/user-attachments/assets/34975c42-f32b-4801-b7ab-65b549f886d9)

* `Icon` : Texture of your icon
* `Lifetime` : Lifetime of the mark
* `Mark Color` : Color of the mark
* `Offset` : Offset of the widget
* `Texture Size` : Size of the texture (icon) in the mark material
* `Mark Sound` : sound played when actor is marked
* `Ignore Itself` : If true and controlling the marked actor, the widget will not be shown. (e.g., if you are marked while playing as the actor who gets marked, you will not see the widget).
* * `Mark Widget Settings` : Settings of the ping widget
   * `Offset` : Offset of the widget (default is 0.5 0.5 (centered))
   * `Icon Size` : Size of the icon
   * `Space` : Space between icon and text
   * `Is Text Visible` : If true, the widget will display a text below the icon showing the distance in meters.
   * `Deciaml Places` : Number of decimal places to display (e.g. 2 will display '3.87 M').
   * `Update Time` : Time interval for updating the widget.



### Event
* `OnMark` : Called when the actor is marked

### Functions

#### ``
#### `GetIcon`
Return the icon.

#### `SetIcon`
Set a new icon

#### `GetLifetime`
Return the lifetime

#### `SetLifetime`
Set a new lifetime

#### `GetMarkColor`
Return the mark color

#### `SetMarkColor`
Set a new mark color

#### `GetIgnoreItself`
Return GetIgnoreItself

#### `SetIgnoreItself`
Set IgnoreItself

#### `GetOffset`
Return the offset

#### `SetOffset`
Set a new offset

#### `GetTextureSize`
Return the texture size

#### `SetTextureSize`
Set a new texture size

#### `GetMarkSound`
Return the mark sound

#### `SetMarkSound`
Set a new mark sound

#### `UpdateSettings` 
Update Settings

* `Offset` : Offset of the widget
* `Icon Size` : Size of the icon
* `Space` : Space between icon and text
* `Is Text Visible` : If true, the widget will display a text below the icon showing the distance in meters.
* `Deciaml Places` : Number of decimal places to display (e.g. 2 will display '3.87 M').
* `Update Time` : Time interval for updating the widget.

#### `GetMarkWidget` 
Return the widget of the markedComponent

#### `GetMarkWidgetSettings` 
Return settings

</details>

<details>
   <summary>Mark User Widget</summary>
   
   You can change the user widget in `Project Settings`
   ![image](https://github.com/user-attachments/assets/5420d24f-cdaf-4f9d-b372-3a37ec1787e7)

   This widget is used in both Ping and Mark widgets


   ### Create your own

   Create a new blueprint class with `MarkUserWidget` as parent
   
   ![image](https://github.com/user-attachments/assets/08875180-ebc0-444a-8ba3-a905c763d92f)

   Add all elements of the based widget in your new widget with correct names

   ![image](https://github.com/user-attachments/assets/34a30a21-c9f3-4eaf-858d-de66322b3bf1)

   Widget --> Name
   
   * Canvas Panel --> Canvas
      * Vertical Box --> MarkVecticalBox
         *  Image --> MarkIcon
         *  Spacer --> MarkSpacer
         *  TextBlock --> MarkText
       

   Vectical Box default settings

   ![image](https://github.com/user-attachments/assets/d93ed442-8be4-46a6-a7be-49d6ca0fd74a)

   Image default settings

   ![image](https://github.com/user-attachments/assets/deb1fde9-0461-4267-809c-1755fcbb5ea3)

   Spacer default settings

   ![image](https://github.com/user-attachments/assets/33a1561f-d375-4f7d-968b-7761c704321d)

   TextBlock default settings

   ![image](https://github.com/user-attachments/assets/7a5c8824-19db-4c60-a122-421db2fffde7)


   #### Event
   `OnPingEvent` : Called when ping or mark.

   ![image](https://github.com/user-attachments/assets/2def8e75-aee0-4f8c-97d5-7d831f2be0a5)


</details>
   

<details>
   <summary>Global Function</summary>   
   All of these function can be called in any blueprint.

   ## `UpdateTeam`

   ## `GetTeams`

   ## `GetMyTeam`

   ## `GetMemberIcon`

   ## `GetMyIcon`
   
---
Server Only 

   ## `ClearWidgets`
   Clear widgets of all player

   * `Type` : MarkType : Ping, Mark or Both

   ## `ServerPingOnTeam`
   Ping from any team to any team. the ping will be visible for all player of the choosen team (TeamName)

   *`Location` : Ping location
   *`Lifetime` : Ping lifetime
   *`PlayerId` : Player Id 
   *`FromTeamName` : Ping from team name
   *`TeamName` : Team name (the team on which you want the ping to be visible)

   ## `ServerRemovePingOnTeam`
   Remove ping from any team to any team. the ping will be hidden for all player of the choosen team (TeamName)

   *`PlayerId` : Player Id 
   *`FromTeamName` : Ping from team name
   *`TeamName` : Team name (the team on which you want the ping to be hidden)

   ## `ServerPingOnTeam`
   Ping from any team to all teams.

   *`Location` : Ping location
   *`Lifetime` : Ping lifetime
   *`PlayerId` : Player Id 
   *`FromTeamName` : Ping from team name

   ## `ServerRemovePingOnTeam`
   Remove ping from any team to all teams.

   *`PlayerId` : Player Id 
   *`FromTeamName` : Ping from team name

   ## `ServerMarkOnTeam`
   Mark an actor on a specific team.

   *`Actor` : Actor with a `MarkedComponent`
   *`Color` : Color of the mark
   *`TeamName` : Team name (the team on which you want the mark to be visible)

      ## `ServerRemoveMarkOnTeam`
  Remove a marked actor on a specific team.

   *`Actor` : Actor with a `MarkedComponent`
   *`TeamName` : Team name (the team on which you want the mark to be hidden)

   ## `ServerMarkOnAllTeam`
   Mark an actor on all teams

   *`Actor` : Actor with a `MarkedComponent`
   *`Color` : Color of the mark

   ## `ServerRemoveMarkOnAllTeam`
   Remove a marked actor on all teams

   *`Actor` : Actor with a `MarkedComponent`

   ## `UpdateMembersIcon`
   Sets all icons for the members of each team. This function must be called when all players have joined the game and have been added to a team.
   You can call this function at any time if you want to update the players' icons.
   
</details>
