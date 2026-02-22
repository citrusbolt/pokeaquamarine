VermilionCity_Mart_MapScripts::
	.byte 0

VermilionCity_Mart_EventScript_CooltrainerF::
	msgbox VermilionCity_Mart_Text_MonsGoodOrBadDependingOnTrainer, MSGBOX_NPC
	end

VermilionCity_Mart_EventScript_BaldingMan::
	msgbox VermilionCity_Mart_Text_TeamRocketAreWickedPeople, MSGBOX_NPC
	end

VermilionCity_Mart_EventScript_Clerk::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart 0
	msgbox Text_PleaseComeAgain
	release
	end
VermilionCity_Mart_EventScript_Clerk2::
	goto_if_questlog EventScript_ReleaseEnd
	lock
	faceplayer
	message Text_MayIHelpYou
	waitmessage
	pokemart VermilionCity_Mart_Items
	msgbox Text_PleaseComeAgain
	release
	end

	.align 2
VermilionCity_Mart_Items::
	.2byte ITEM_NEST_BALL
	.2byte ITEM_NONE
	release
	end
