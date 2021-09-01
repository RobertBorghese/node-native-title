
// show dev tools

require('nw.gui').Window.get().showDevTools();

// test module

const node = require("../build/Release/NodeNativeMenu");

console.log(node);

setTimeout(() => {
	console.log(node.FindHWNDByPID(process.pid));
	console.log(node.FindHWNDByPID(process.ppid));
	console.log(node.FindHWNDByTitle("Test Node Native Title"));
	console.log(node.FindHWNDByTitle("Test Node Native Title", "Chrome_WidgetWin_1"));
}, 10);
