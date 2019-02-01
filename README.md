# Interaction plug-in for the Unreal Engine. 

The Goal of this plug-in is to handle the interaction between player and objects/actors in the game by implementing a **component based architecture**. There are two main components, **Interactor Component** which is added to the player and **Interaction Component** for the Intractable Objects.

![Instant Interaction](https://i.imgur.com/7fk4zfo.gif )

![Hold Interaction](https://i.imgur.com/uGBkTiW.gif)

## Introduction  

The Interaction System is designed and developed based on a Component to Component communication architecture. All of the required logic during the interaction process is handled and processed by the components attached to the owners such as Characters and Interactive objects. The system is mainly made of two important components, Interactor Component which is added to the character or the player pawn and interaction component that is required by an interactive object.

## Road map / Features:
1. Interaction Types
	- Instant [DONE]
	- Hold [DONE]
2. Condition Based Interaction
	- Single Interaction [DONE]
	- Multiple Interaction [DONE]
	- Custom Defined Condition E.g Team Id [DONE]
3. Multiplayer/Network Support
	- Interaction States Replication [DONE]
	- Interaction Process [DONE]
	- Notifications for Animations [DONE]
4. Documentation [DONE]
	- Showcase Level

## Getting up and Running
1. Add an **Interactor Component** to the Character or Player Pawn
2. Add an **Interaction Component** to the Interactive Object (e.g Door)
3. Setup an Input key to Invoke **TryStartInteraction** on the **Interactor Component**
![Setup Interactor](https://i.imgur.com/kkyUdWZ.png)
4. Bind to the **OnInteractorStateChanged** Delegate on **Interactor Component** to Receive Interaction Results

## Interaction Components
### Class: UInteractionComponent
Interaction Component is added to an Interact-able object (E.g Doors,Pickup). There are currently two types of Interaction Components.

- **InteractionComponent_Instant:**   
Class: UInteractionComponent_Instant  
	Interaction is Instantly Completed upon Initiation by an Interactor.
- **InteractionComponent_Hold:**   
Class: UInteractionComponent_Hold  
	Hold Interaction is a Duration based interaction that requires the Interactor to actively interact with the object for the duration.

## Multiple Interaction
Interaction Components can allow multiple interactions simultaneously. A configuration Boolean on the interaction component named **bMultipleInteraction**, controls and determines whether Interaction components can allow simultaneous interaction or only one interaction at a time. This does not imply to Instant interaction, as the interaction is completed instantly.

## Condition Based Interaction
### Class: IInteractionInterface  
At times, **Custom Conditions** are required to be met before starting an interaction, for example, a lock system on a chest or a team only buildings and equipment. In order to handle such custom conditions both Interactor and Interaction Components will Execute an **Interface** call on their owners `ICanInteractWith(Actor* OtherOwner)` Passing the other party actor, this interface then returns a **Boolean** determining whether the interaction can be initiated or not. **This does not mean that the interface has to be always implemented on the owner even if custom conditions are not required, the components will simply ignore the interface call if the owner does not implement it.**  

**E.g:** In order to implement a team only chest, we simply add an Interaction Interface on the chest actor. Then we override the ICanInteractWith Function in the interface tab. Then inside the function, we get the team id of the OtherOwner then return true if the team id is equal to the chest team id.

![Example of Interaction Interface](https://i.imgur.com/L0XxKLZ.png)

## Interaction Result  
Both Interaction and Interactor Components implement Interaction **Results** and **State**.  Interaction Results are enums that are meant to provide more information during the interaction process. These results are **broadcasted** through delegates when an Interaction is initiated up to completion. 

#### EInteractionResult:
- **None [IR_None]:** Unknown Result
- **Started [IR_Started]**: Interaction Started
- **Successful [IR_Successful]**: Interaction Successfully Completed
- **Failed [IR_Failed]**: Interaction Failed due to Conditions Returning False
- **Interrupted [IR_Interrupted]**: Interaction Interrupted due to Player Pawn or Character Looking away and Going out of Reach during Interaction. Moreover, Interruptions Happen after removal of Interaction or Interactor Components during Interaction.  

These Interaction Results are received and broadcasted on Both Components through delegates (Blueprint Event Dispatcher).   

- **Interaction Component**: OnInteractionStateChanged  
- **Interactor Component**: OnInteractorStateChanged  

In a networked Environments, These results are send to the clients through **Remote Procedure Calls (RPC)**. But in Some cases this Information may not be relevant to all the clients or on the other hand, all the clients should be aware of these results. This can be controlled and configured on each component by changing the InteractorStateNetMode or InteractionStateNetMode.
- **None** : None of the Clients Receive the Result Update
- **OwnerOnly** : Only the Local Owner of the Component Will Receive the Update
- **All**  : All Clients With this Instance of the Component Will Receive the Update

## Interaction Focus
It is important to be able to notify and inform the player of an interactive object or even show and interaction widget (Press E to Interact). This can be **easily** implemented by binding/listening to any of these delegates. 

**OnNewInteraction**: This Delegate is broadcasted by the **Interactor Component** when a New Interactive object comes into the focus or leaves the reach of the player.

**OnInteractionFocusChanged**: Delegate Implemented by the **Interaction Component**. Broadcasted whenever the interaction object comes into the focus of a player.

## Author
Contact me on [Twitter](https://twitter.com/amiransari09).
