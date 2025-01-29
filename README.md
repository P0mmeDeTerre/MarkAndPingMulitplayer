![image](https://github.com/user-attachments/assets/7596dfc0-5b06-4b92-b7aa-8286927fac81)# MarkAndPingMulitplayer


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

## Ping Component
The `PingComponent` must be placed in the `PlayerController` of the player.

![image](https://github.com/user-attachments/assets/6e9847d1-c759-42e6-9215-d8b9a5530549)

The only settings of the `PingComponent` is the `IconMaterial`
![image](https://github.com/user-attachments/assets/9fab236b-e0b6-4a44-aa26-89d7b54a4707)

You can replace the `IconMaterial` by your own one but you need to keep those settings
This material is only used for your own ping. If you want to change all material, you also need to change it on the `MarkComponent`

 * `Texture` : Texture Parameter
 * `Color` : Vector Parameter

## Mark Component
This component is used to send a mark/ping to the server.

### Settings

![image](https://github.com/user-attachments/assets/83aea200-d6bc-40fc-99e2-ea021fc5266e)

* `Ping Material Instance` : The material instance for other player's ping
  *  * `Texture` : Texture Parameter
  *  * `Color` : Vector Parameter




  
