package cmd

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/spf13/cobra"
	"github.com/spf13/viper"
)

var (
	flagConfigFile string
	rootCmd        = &cobra.Command{
		Use:   "sqlboiler [flags] <driver>",
		Short: "SQL Boiler generates an ORM tailored to your database schema.",
		Long: "SQL Boiler generates a Go ORM from template files, tailored to your database schema.\n" +
			`Complete documentation is available at http://github.com/volatiletech/sqlboiler`,
		Example: `sqlboiler mysql`,
		RunE:    run,
	}
)

func init() {
	cobra.OnInitialize(initConfig)
	// Set up the cobra root command flags
	rootCmd.PersistentFlags().StringVarP(&flagConfigFile, "config", "c", "", "Filename of config file to override default lookup")
	rootCmd.PersistentFlags().StringP("output", "o", "models", "The name of the folder to output to")
	rootCmd.PersistentFlags().StringP("pkgname", "p", "models", "The name you wish to assign to your generated package")
	rootCmd.PersistentFlags().StringSliceP("templates", "", nil, "A templates directory, overrides the bindata'd template folders in sqlboiler")
	rootCmd.PersistentFlags().StringSliceP("tag", "t", nil, "Struct tags to be included on your models in addition to json, yaml, toml")
	rootCmd.PersistentFlags().StringSliceP("replace", "", nil, "Replace templates by directory: relpath/to_file.tpl:relpath/to_replacement.tpl")
	rootCmd.PersistentFlags().BoolP("debug", "d", false, "Debug mode prints stack traces on error")
	rootCmd.PersistentFlags().BoolP("no-context", "", false, "Disable context.Context usage in the generated code")
	rootCmd.PersistentFlags().BoolP("no-tests", "", false, "Disable generated go test files")
	rootCmd.PersistentFlags().BoolP("no-hooks", "", false, "Disable hooks feature for your models")
	rootCmd.PersistentFlags().BoolP("no-rows-affected", "", false, "Disable rows affected in the generated API")
	rootCmd.PersistentFlags().BoolP("no-auto-timestamps", "", false, "Disable automatic timestamps for created_at/updated_at")
	rootCmd.PersistentFlags().BoolP("add-global-variants", "", false, "Enable generation for global variants")
	rootCmd.PersistentFlags().BoolP("add-panic-variants", "", false, "Enable generation for panic variants")
	rootCmd.PersistentFlags().BoolP("version", "", false, "Print the version")
	rootCmd.PersistentFlags().BoolP("wipe", "", false, "Delete the output folder (rm -rf) before generation to ensure sanity")
	rootCmd.PersistentFlags().StringP("struct-tag-casing", "", "snake", "Decides the casing for go structure tag names. camel, title or snake (default snake)")
}

func initConfig() {
	if len(flagConfigFile) != 0 {
		viper.SetConfigFile(flagConfigFile)
		if err := viper.ReadInConfig(); err != nil {
			fmt.Println("Can't read config:", err)
			os.Exit(1)
		}
		return
	}

	var err error
	viper.SetConfigName("sqlboiler")

	configHome := os.Getenv("XDG_CONFIG_HOME")
	homePath := os.Getenv("HOME")
	wd, err := os.Getwd()
	if err != nil {
		wd = "."
	}

	configPaths := []string{wd}
	if len(configHome) > 0 {
		configPaths = append(configPaths, filepath.Join(configHome, "sqlboiler"))
	} else {
		configPaths = append(configPaths, filepath.Join(homePath, ".config/sqlboiler"))
	}

	for _, p := range configPaths {
		viper.AddConfigPath(p)
	}

	// sqlboiler config文件有两个路径，一个是HOME目录下的.config/sqlboiler文件
	// 另一个是当前目录下的.config/sqlboiler文件
	if err := viper.ReadInConfig(); err == nil {
		fmt.Println("using config file:", viper.ConfigFileUsed())
	} /*else {
		fmt.Println(err.Error())
	}*/
}

// Execute 方法
func Execute() {
	if err := rootCmd.Execute(); err != nil {
		os.Exit(1)
	}
}
