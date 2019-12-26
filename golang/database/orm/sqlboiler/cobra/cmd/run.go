package cmd

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"github.com/pkg/errors"
	"github.com/spf13/cobra"
)

func run(cmd *cobra.Command, args []string) error {
	var err error

	if len(args) == 0 {
		return errors.New(fmt.Sprintln("must provide command args"))
	}

	driverName := args[0]
	driverPath := args[0]

	if strings.ContainsRune(driverName, os.PathSeparator) {
		driverName = strings.Replace(filepath.Base(driverName), "sqlboiler-", "", 1)
		driverName = strings.Replace(driverName, ".exe", "", 1)
	} else {
		driverPath = "sqlboiler-" + driverPath
		if p, err := exec.LookPath(driverPath); err == nil {
			driverPath = p
		}
		fmt.Printf("[driverPath: %v]\n", driverPath)
		// Output:
		// [driverPath: /Users/Jiahuan/go/bin/sqlboiler-mysql]
	}

	driverPath, err = filepath.Abs(driverPath)
	if err != nil {
		return errors.Wrap(err, "could not find absolute path to driver")
	}

	fmt.Printf("[driverName: %v, driverPath: %v]\n", driverName, driverPath)
	// Output:
	// [driverName: mysql, driverPath: /Users/Jiahuan/go/bin/sqlboiler-mysql]
	return err
}
