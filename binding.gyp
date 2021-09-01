{
	"targets": [
		{
			"target_name": "NodeNativeMenu",
			"sources": [ "src/main.cpp" ],
			"include_dirs": [
				"<!(node -e \"require('nan')\")"
			]
		}
	]
}