// Compliance console for the MainMenu

cconsole = {
	"error" : function(message, source, lineno, colno, error) {
		$("#console").show();
		var msg = message + ": on line " + lineno + " in file \"" + source + "\"";
		$("#console-output").append($("<p class='console-error'>" + msg + "</p>"));
		//window.location.href = "menu://vconsole/" + msg;
	},
	"log" : function(msg) {
		window.console.log(msg);
		//window.location.href = "menu://vconsole/" + msg;
	},
	"devmsg" : function(msg) {
		window.console.log(msg);
		//window.location.href = "menu://vconsole/" + msg;
	}
};

window.onerror = function(message, source, lineno, colno, error) {
	cconsole.error(message, source, lineno, colno, error);
}
