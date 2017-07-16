/*
Menubtn Types:
0 - onclick: push current page to stack, open linked menupage
1 - actual link (usually menu://)
2 - campaign/scenario entry with image, link to image is "image"

menu protocol:
menu://startchapter/N
	Starts a chapter
	N is the number of the chapter
	Only chapters 1 through 20 work
menu://startscenario/N
	Starts a scenario
	N is the number of the scenario
	Only scenarios 1 through 20 work
menu://continue
	Loads the latest save
menu://resume
	Resume the game
menu://exit
	Exit the game

menu protocol LEGACY:
menu://newgame
	Opens the HL2 "New Game" window
menu://loadgame
	Opens the HL2 "Load Game" window
menu://savegame
	Opens the HL2 "Save Game" window
menu://options
	Opens the HL2 "Options" window
menu://quit
	Opens the HL2 "Quit" window
 */

var mainmenu = {
	"title" : undefined,
	"buttons" : [
		{
			"title" : "Start Campaign",
			"type" : 0,
			"link" : "campaigns"
		},
		{
			"title" : "Start Scenario",
			"type" : 0,
			"link" : "scenarios"
		},
		/*{
			"title" : "Options",
			"type" : 0,
			"link" : "options",
		},*/
		{
			"title" : "Demo Menu",
			"type" : 0,
			"link" : "demo"
		},
		{
			"title" : "Options",
			"type" : 1,
			"link" : "menu://options",
		},
		{
			"title" : "Quit",
			"type" : 1,
			"link" : "menu://quit"
		}
	]
};

var gamemenu = {
	"title" : undefined,
	"buttons" : [
		{
			"title" : "Resume Game",
			"type" : 1,
			"link" : "menu://resume"
		},
		{
			"title" : "Start Campaign",
			"type" : 0,
			"link" : "campaigns"
		},
		{
			"title" : "Start Scenario",
			"type" : 0,
			"link" : "scenarios"
		},
		/*{
			"title" : "Options",
			"type" : 0,
			"link" : "options",
		},*/
		{
			"title" : "Demo Menu",
			"type" : 0,
			"link" : "demo"
		},
		{
			"title" : "Options",
			"type" : 1,
			"link" : "menu://options",
		},
		{
			"title" : "Quit",
			"type" : 1,
			"link" : "menu://quit"
		}
	]
};

var campaign = {
	"title" : "Campaign",
	"subtitle": "Select a chapter",
	"buttons" : [
	]
};

var scenario = {
	"title" : "Scenarios",
	"subtitle" : "Select a scenario",
	"buttons" : [
		{
			"title" : "Mansion",
			"type" : 2,
			"link" : 1,
			"image" : "mansion"
		}
	]
};

/*
var options = {
	"title" : "Options",
	"buttons" : [
		{
			"title" : "Edit keys",
			"type" : 1,
			"link" : "menu://"
		}
	]
};
*/

var demo = {
	"title" : "Demo Level 1",
	"buttons" : [
		{
			"title" : "Level 2 Branch 1",
			"type" : 0,
			"link" : "demo_2_1"
		},
		{
			"title" : "Level 2 Branch 2",
			"type" : 0,
			"link" : "demo_2_2"
		},
	]
};

var demo_2_1 = {
	"title" : "Demo Level 2 Branch 1",
	"buttons" : [
		{
			"title" : "Main Menu",
			"type" : 0,
			"link" : "mainmenu"
		},
		{
			"title" : "Level 2 Branch 1 (recursive)",
			"type" : 0,
			"link" : "demo_2_1"
		},
	]
};

var demo_2_2 = {
	"title" : "Demo Level 2 Branch 2",
	"buttons" : [
		{
			"title" : "There is nothing here",
			"type" : 1,
			"link" : "#"
		},
	]
};

var menupages = {
	"mainmenu" : mainmenu,
	"gamemenu" : gamemenu,
	"campaigns" : campaign,
	"scenarios" : scenario,

	"demo" : demo,
	"demo_2_1" : demo_2_1,
	"demo_2_2" : demo_2_2,
};



var chapters = [
	{
		"title" : "Chapter 1 - Segment Not Present",
		"map" : "c1a0"
	},
	{
		"title" : "Chapter 2 - Bound Range Exceeded",
		"map" : "c2a0"
	},
	{
		"title" : "Chapter 3 - Breakpoint",
		"map" : "c3a0",
	}
];

var scenarios = [
	{
		"title" : "Wastelands Outpost",
		"map" : "scenario_test1"
	},
	{
		"title" : "Arcade",
		"map" : "scenario_test2"
	},
	{
		"title" : "Mansion",
		"map" : "scenario_mansion"
	}
];