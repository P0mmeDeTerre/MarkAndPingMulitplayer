# MarkAndPingMulitplayer


## Mark Team Component

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

  
