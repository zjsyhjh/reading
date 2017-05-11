#### moby/tool/cmd/moby源码分析

- 通过`moby build —help`命令查看，可以得到

  - ```shell
    USAGE: moby build [options] <file>[.yml] | -

    Options:
      -name string
        	Name to use for output files
      -pull
        	Always pull images
    ```

- `build(args []string)`函数位于`build.go`文件中，下面通过对该函数代码进行分析

  - ```go
    // Process the build arguments and execute build
    func build(args []string) {
      	//返回一个名字为"build"的空的flag集合
    	buildCmd := flag.NewFlagSet("build", flag.ExitOnError)
    	buildCmd.Usage = func() {
    		fmt.Printf("USAGE: %s build [options] <file>[.yml] | -\n\n", os.Args[0])
    		fmt.Printf("Options:\n")
    		buildCmd.PrintDefaults()
    	}
      	//定义string类型的name flag以及bool类型的pull flag
    	buildName := buildCmd.String("name", "", "Name to use for output files")
    	buildPull := buildCmd.Bool("pull", false, "Always pull images")
    	//解析args中的flag参数
    	if err := buildCmd.Parse(args); err != nil {
    		log.Fatal("Unable to parse args")
    	}
      	//解析非flag参数，根据之前的'moby build --help'可以看出，其实就两种可能，一种是<name>.yml文件，一种‘-’，表示从命令行中读入
    	remArgs := buildCmd.Args()

    	if len(remArgs) == 0 {
    		fmt.Println("Please specify a configuration file")
    		buildCmd.Usage()
    		os.Exit(1)
    	}
    	name := *buildName
    	var config []byte
        //如果从命令行读入
    	if conf := remArgs[0]; conf == "-" {
    		var err error
    		config, err = ioutil.ReadAll(os.Stdin)
    		if err != nil {
    			log.Fatalf("Cannot read stdin: %v", err)
    		}
    		if name == "" {
                //默认值为moby
    			name = defaultNameForStdin
    		}
    	} else {
          	//从<name>.yml文件读入，也可以为name,默认会加上.yml后缀
    		if !(filepath.Ext(conf) == ".yml" || filepath.Ext(conf) == ".yaml") {
    			conf = conf + ".yml"
    		}
    		var err error
    		config, err = ioutil.ReadFile(conf)
    		if err != nil {
    			log.Fatalf("Cannot open config file: %v", err)
    		}
    		if name == "" {
              	//去掉.yml后缀，如果路径是xxx/<example>.yml, 则取最后的example,别的都不要
    			name = strings.TrimSuffix(filepath.Base(conf), filepath.Ext(conf))
    		}
    	}
    	//用这个<example>.yml文件中的内容初始化一个名为Moby的结构体，位于config.go文件中
    	m, err := NewConfig(config)
    	if err != nil {
    		log.Fatalf("Invalid config: %v", err)
    	}
    	//构建镜像
    	image := buildInternal(m, name, *buildPull)

    	log.Infof("Create outputs:")
        //创建需要输出的镜像，名称为<example>.yml中的example
    	err = outputs(m, name, image)
    	if err != nil {
    		log.Fatalf("Error writing outputs: %v", err)
    	}
    }
    ```

- 查看位于`config.go`文件中的`NewConfig(config []byte) (*Moby, error)`函数，下面对该函数进行分析

  - ```go
    // Moby is the type of a Moby config file
    type Moby struct {
    	Kernel struct {
    		Image   string
    		Cmdline string
    	}
    	Init     []string
    	Onboot   []MobyImage
    	Services []MobyImage
    	Trust    TrustConfig
    	Files    []struct {
    		Path      string
    		Directory bool
    		Symlink   string
    		Contents  string
    	}
    	Outputs []struct {
    		Format string
    	}
    }

    // NewConfig parses a config file
    func NewConfig(config []byte) (*Moby, error) {
    	m := Moby{}

    	// Parse raw yaml
      	// 解析config字节数组，保存到rawYaml变量，可以查看yaml.go的实现
      	// For example:
    	//
    	//     type T struct {
    	//         F int `yaml:"a,omitempty"`
    	//         B int
    	//     }
    	//     var t T
    	//     yaml.Unmarshal([]byte("a: 1\nb: 2"), &t)
      
    	var rawYaml interface{}
    	err := yaml.Unmarshal(config, &rawYaml)
    	if err != nil {
    		return &m, err
    	}

    	// Convert to raw JSON
      	// 将rawYaml转换成json格式，主要是为了后面做json格式验证
    	rawJSON := convert(rawYaml)

    	// Validate raw yaml with JSON schema
      	//验证原始的config文件的格式是否符合定义的模式，可以查看schema.go文件看变量schema
    	schemaLoader := gojsonschema.NewStringLoader(schema)
    	documentLoader := gojsonschema.NewGoLoader(rawJSON)
    	result, err := gojsonschema.Validate(schemaLoader, documentLoader)
    	if err != nil {
    		return &m, err
    	}
    	if !result.Valid() {
    		fmt.Printf("The configuration file is invalid:\n")
    		for _, desc := range result.Errors() {
    			fmt.Printf("- %s\n", desc)
    		}
    		return &m, fmt.Errorf("invalid configuration file")
    	}

    	// Parse yaml
       	// 验证格式通过之后，将config字节数组接解析到m变量
    	err = yaml.Unmarshal(config, &m)
    	if err != nil {
    		return &m, err
    	}

    	return &m, nil
    }
    ```

- 回到`build.go`文件的`build(args []string)`函数，当调用`NewConfig(config)`函数完成对`yml`配置文件的内容解析之后，通过`buildInternal(m *Moby, name string, pull bool) []byte`函数来进行镜像的构建，该函数也位于`build.go`文件中，下面对该函数进行分析

  - ```go
    // Perform the actual build process
    // TODO return error not panic
    func buildInternal(m *Moby, name string, pull bool) []byte {
    	w := new(bytes.Buffer)
    	iw := tar.NewWriter(w)

    	if pull || enforceContentTrust(m.Kernel.Image, &m.Trust) {
    		log.Infof("Pull kernel image: %s", m.Kernel.Image)
    		err := dockerPull(m.Kernel.Image, enforceContentTrust(m.Kernel.Image, &m.Trust))
    		if err != nil {
    			log.Fatalf("Could not pull image %s: %v", m.Kernel.Image, err)
    		}
    	}
    	if m.Kernel.Image != "" {
    		// get kernel and initrd tarball from container
    		log.Infof("Extract kernel image: %s", m.Kernel.Image)
    		const (
    			kernelName    = "kernel"
    			kernelAltName = "bzImage"
    			ktarName      = "kernel.tar"
    		)
    		out, err := ImageExtract(m.Kernel.Image, "", enforceContentTrust(m.Kernel.Image, &m.Trust), pull)
    		if err != nil {
    			log.Fatalf("Failed to extract kernel image and tarball: %v", err)
    		}
    		buf := bytes.NewBuffer(out)

    		kernel, ktar, err := untarKernel(buf, kernelName, kernelAltName, ktarName, m.Kernel.Cmdline)
    		if err != nil {
    			log.Fatalf("Could not extract kernel image and filesystem from tarball. %v", err)
    		}
    		initrdAppend(iw, kernel)
    		initrdAppend(iw, ktar)
    	}

    	// convert init images to tarballs
    	if len(m.Init) != 0 {
    		log.Infof("Add init containers:")
    	}
    	for _, ii := range m.Init {
    		log.Infof("Process init image: %s", ii)
    		init, err := ImageExtract(ii, "", enforceContentTrust(ii, &m.Trust), pull)
    		if err != nil {
    			log.Fatalf("Failed to build init tarball from %s: %v", ii, err)
    		}
    		buffer := bytes.NewBuffer(init)
    		initrdAppend(iw, buffer)
    	}

    	if len(m.Onboot) != 0 {
    		log.Infof("Add onboot containers:")
    	}
    	for i, image := range m.Onboot {
    		log.Infof("  Create OCI config for %s", image.Image)
    		config, err := ConfigToOCI(&image)
    		if err != nil {
    			log.Fatalf("Failed to create config.json for %s: %v", image.Image, err)
    		}
    		so := fmt.Sprintf("%03d", i)
    		path := "containers/onboot/" + so + "-" + image.Name
    		out, err := ImageBundle(path, image.Image, config, enforceContentTrust(image.Image, &m.Trust), pull)
    		if err != nil {
    			log.Fatalf("Failed to extract root filesystem for %s: %v", image.Image, err)
    		}
    		buffer := bytes.NewBuffer(out)
    		initrdAppend(iw, buffer)
    	}

    	if len(m.Services) != 0 {
    		log.Infof("Add service containers:")
    	}
    	for _, image := range m.Services {
    		log.Infof("  Create OCI config for %s", image.Image)
    		config, err := ConfigToOCI(&image)
    		if err != nil {
    			log.Fatalf("Failed to create config.json for %s: %v", image.Image, err)
    		}
    		path := "containers/services/" + image.Name
    		out, err := ImageBundle(path, image.Image, config, enforceContentTrust(image.Image, &m.Trust), pull)
    		if err != nil {
    			log.Fatalf("Failed to extract root filesystem for %s: %v", image.Image, err)
    		}
    		buffer := bytes.NewBuffer(out)
    		initrdAppend(iw, buffer)
    	}

    	// add files
    	buffer, err := filesystem(m)
    	if err != nil {
    		log.Fatalf("failed to add filesystem parts: %v", err)
    	}
    	initrdAppend(iw, buffer)
    	err = iw.Close()
    	if err != nil {
    		log.Fatalf("initrd close error: %v", err)
    	}

    	return w.Bytes()
    }
    ```

