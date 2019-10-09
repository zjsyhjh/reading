package main

import (
	"cobra/cmd"
)

func main() {
	cmd.Execute()
	// go build -o sqlboiler . && ./sqlboiler

	// output:
	// 	Error: must provide command args

	// Usage:
	//   sqlboiler [flags] <driver>

	// Examples:
	// sqlboiler mysql

	// Flags:
	//       --add-global-variants        Enable generation for global variants
	//       --add-panic-variants         Enable generation for panic variants
	//   -c, --config string              Filename of config file to override default lookup
	//   -d, --debug                      Debug mode prints stack traces on error
	//   -h, --help                       help for sqlboiler
	//       --no-auto-timestamps         Disable automatic timestamps for created_at/updated_at
	//       --no-context                 Disable context.Context usage in the generated code
	//       --no-hooks                   Disable hooks feature for your models
	//       --no-rows-affected           Disable rows affected in the generated API
	//       --no-tests                   Disable generated go test files
	//   -o, --output string              The name of the folder to output to (default "models")
	//   -p, --pkgname string             The name you wish to assign to your generated package (default "models")
	//       --replace strings            Replace templates by directory: relpath/to_file.tpl:relpath/to_replacement.tpl
	//       --struct-tag-casing string   Decides the casing for go structure tag names. camel, title or snake (default snake) (default "snake")
	//   -t, --tag strings                Struct tags to be included on your models in addition to json, yaml, toml
	//       --templates strings          A templates directory, overrides the bindata'd template folders in sqlboiler
	//       --version                    Print the version
	//       --wipe                       Delete the output folder (rm -rf) before generation to ensure sanity
}
