var page_stack = [];
var page_current = undefined;

var sfx_click;
var sfx_hover;

var chapter_queue = [];

function push_page(page)
{
	console.log("push: " + page);
	if(page_current != undefined)
	{
		page_stack.push(page_current);
		//$('#' + page_current).hide(400);
		$('#' + page_current).fadeOut(400);
	}
	page_current = page;
	$('#' + page).delay(400).fadeIn(400);
}

function pop_page()
{
	if(page_stack.length == 0)
		return;
	//$('#' + page_current).hide();
	$('#' + page_current).fadeOut(400);
	page_current = page_stack.pop();
	//$('#' + page_current).show();
	$('#' + page_current).delay(400).fadeIn(400);
}

function menu_init()
{
	if(menupages == undefined)
	{
		console.log("No menupages defined");
		return;
	}

	// load SFX
	sfx_click = document.createElement('audio');
	sfx_click.setAttribute('src', 'click.wav');
	sfx_hover = document.createElement('audio');
	sfx_hover.setAttribute('src', 'hover.wav');

	$("#logo").hide();
	var pagesroot = $("#pages");
	for(page in menupages)
	{
		var pagedesc = menupages[page];
		var pageclass;
		//if(page == "mainmenu")
			pageclass = 'class="page-mainmenu"';
		/*else
			pageclass = 'class="page"';*/
		var pageroot = $('<div ' + pageclass + ' id="' + page + '">');
		pageroot.append(page_create_header(pagedesc));
		var buttonlist = $('<ul>');
		for(button in pagedesc.buttons)
		{
			var buttondesc = pagedesc.buttons[button];
			var buttonelem = undefined;
			if(buttondesc.type == 0)
			{
				buttonelem = $('<li class="menubtn"><a href="#">' + buttondesc.title + '</a></li>');
				// fucking JS
				buttonelem.click((function(link) {
					return function() {
						push_page(link);
						sfx_click.play();
					};
				})(buttondesc.link));
				buttonelem.hover(function() {
					sfx_hover.play();
				});
			}
			else if(buttondesc.type == 1)
			{
				buttonelem = $('<li class="menubtn"><a href="' + buttondesc.link + '">' + buttondesc.title + '</a></li>');
				buttonelem.click(function() {
					sfx_click.play();
				});
				buttonelem.hover(function() {
					sfx_hover.play();
				});
			}
			else if(buttondesc.type == 2)
			{
				buttonelem = $('<li class="menubtn_image"></li>');
				buttonelem.click(function() {
					sfx_click.play();
				});
				buttonelem.hover(function() {
					sfx_hover.play();
				});
			}
			buttonlist.append(buttonelem);
		}
		if(page != "mainmenu" && page != "gamemenu")
		{
			var backbutton = $('<li class="menubtn"><a href="#">Back</a></li>');
			backbutton.click(function() {
				pop_page();
				sfx_click.play();
			});
			backbutton.hover(function() {
				sfx_hover.play();
			});
			buttonlist.append(backbutton);
		}
		pageroot.append(buttonlist);
		pagesroot.append(pageroot);
		pageroot.hide();
	}
	/*cconsole.devmsg("Adding " + chapter_queue.length + " chapters");
	for(chapter in chapter_queue)
	{
		menu_addchapter_(chapter);
	}*/
	cconsole.devmsg("Adding " + chapters.length + " chapters");
	for(i in chapters)
	{
		var chapter = chapters[i];
		menu_addchapter_([chapter.title, chapter.map]);
	}
	cconsole.devmsg("Adding " + scenarios.length + " scenarios");
	for(i in scenarios)
	{
		var scenario = scenarios[i];
		menu_addscenario_([scenario.title, scenario.map]);
	}
}

function menu_show()
{
	push_page("mainmenu");
	$("#logo").delay(400).fadeIn(400);
}

function page_create_header(page)
{
	/*if(page.title == undefined)
		return;
	var pageheader = $('<div class="pageheader">');
	var title = $('<h1>' + page.title + '</h1>')
	var subtitle = $('<h3>' + page.subtitle + '</h3>')

	pageheader.append(title);
	pageheader.append(subtitle);

	return pageheader;*/
}

function menu_addscenario_(scenario)
{
	var campaigns_page = $("#scenarios");
	var buttonlist = campaigns_page.children("ul")[0];

	var buttonelem = $('<li class="menubtn"><a href="menu://startscenario/' + scenario[1] + '">' + scenario[0] + '</a></li>');
	buttonelem.click(function() {
		sfx_click.play();
	});
	buttonelem.hover(function() {
		sfx_hover.play();
	});

	buttonelem.insertBefore(buttonlist.lastElementChild);
}

function menu_addchapter_(chapter)
{
	var campaigns_page = $("#campaigns");
	var buttonlist = campaigns_page.children("ul")[0];

	var buttonelem = $('<li class="menubtn"><a href="menu://startchapter/' + chapter[1] + '">' + chapter[0] + '</a></li>');
	buttonelem.click(function() {
		sfx_click.play();
	});
	buttonelem.hover(function() {
		sfx_hover.play();
	});

	buttonelem.insertBefore(buttonlist.lastElementChild);
}

function menu_addchapter(chapter)
{
	cconsole.devmsg("queued chapter");
	chapter_queue.push(chapter);
}

function menu_ingame(status)
{
	$('#' + page_current).hide();
	page_stack = [];
	page_current = undefined;
	if(status)
		push_page("gamemenu");
	else
		push_page("mainmenu");
}
