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
    func enforceContentTrust(fullImageName string, config *TrustConfig) bool {
    	for _, img := range config.Image {
    		// First check for an exact name match
    		if img == fullImageName {
    			return true
    		}
    		// Also check for an image name only match
    		// by removing a possible tag (with possibly added digest):
    		if img == strings.TrimSuffix(fullImageName, ":") {
    			return true
    		}
    		// and by removing a possible digest:
    		if img == strings.TrimSuffix(fullImageName, "@sha256:") {
    			return true
    		}
    	}

    	for _, org := range config.Org {
    		if strings.HasPrefix(fullImageName, org+"/") {
    			return true
    		}
    	}
    	return false
    }

    // Perform the actual build process
    // TODO return error not panic
    func buildInternal(m *Moby, name string, pull bool) []byte {
    	w := new(bytes.Buffer)
    	iw := tar.NewWriter(w)
    	//如果需要pull镜像(默认是false)或者说kernel镜像名是在yml文件的trust列表中，则pull对应的kernel镜像
    	if pull || enforceContentTrust(m.Kernel.Image, &m.Trust) {
    		log.Infof("Pull kernel image: %s", m.Kernel.Image)
          	//dockerPull将对应的镜像从dockerhub上pull下来
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
          //提取kernel镜像中的文件(要忽略掉一些文件，添加一些文件)并打包(不压缩), 该tar包含kernel(bzImage)，kernel.tar等文件
    		out, err := ImageExtract(m.Kernel.Image, "", enforceContentTrust(m.Kernel.Image, &m.Trust), pull)
    		if err != nil {
    			log.Fatalf("Failed to extract kernel image and tarball: %v", err)
    		}
    		buf := bytes.NewBuffer(out)
    		//提取tar包中的kernel(bzImage)，kernel.tar等文件内容并写入到名为kernel, ktar的缓冲区
    		kernel, ktar, err := untarKernel(buf, kernelName, kernelAltName, ktarName, m.Kernel.Cmdline)
    		if err != nil {
    			log.Fatalf("Could not extract kernel image and filesystem from tarball. %v", err)
    		}
          	//将kernel以及tar的缓冲区内容追加写到名为w的缓冲区
    		initrdAppend(iw, kernel)
    		initrdAppend(iw, ktar)
    	}

    	// convert init images to tarballs
    	if len(m.Init) != 0 {
    		log.Infof("Add init containers:")
    	}
      	//将init镜像内容提取到名为init的缓冲区，然后也是追加写到名为w的缓冲区
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
      	//遍历Onboot镜像
    	for i, image := range m.Onboot {
    		log.Infof("  Create OCI config for %s", image.Image)
          	//转化成OCI格式的container configuration file
          	//https://github.com/opencontainers/runtime-spec/blob/master/config.md
    		config, err := ConfigToOCI(&image)
    		if err != nil {
    			log.Fatalf("Failed to create config.json for %s: %v", image.Image, err)
    		}
    		so := fmt.Sprintf("%03d", i)
    		path := "containers/onboot/" + so + "-" + image.Name
          	//在路径path下生成一个OCI规范的tar包，包括一个镜像以及一个json文件
    		out, err := ImageBundle(path, image.Image, config, enforceContentTrust(image.Image, &m.Trust), pull)
    		if err != nil {
    			log.Fatalf("Failed to extract root filesystem for %s: %v", image.Image, err)
    		}
    		buffer := bytes.NewBuffer(out)
          	//追加到最终的输出镜像上
    		initrdAppend(iw, buffer)
    	}

    	if len(m.Services) != 0 {
    		log.Infof("Add service containers:")
    	}
      	//和Onboot类似
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
      	// 添加yml中指定的文件以及其内容
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

- 了解了`buildInternal(m *Moby, name string, pull bool)`函数的大致流程，来看一下其中某些过程的细节，下面对`ImageExtract(image, prefix string, trust bool, pull bool`函数进行分析，该函数的实现位于`image.go`，其主要功能就是从镜像中提取文件并重新打包

  - ```go
    // ImageExtract extracts the filesystem from an image and returns a tarball with the files prefixed by the given path
    func ImageExtract(image, prefix string, trust bool, pull bool) ([]byte, error) {
    	log.Debugf("image extract: %s %s", image, prefix)
    	out := new(bytes.Buffer)
    	tw := tar.NewWriter(out)
      	// tarPrefix creates the leading directories for a path
      	// 传入的path必须是相对路径,比如说../a/b/c/，则必须先创建目录a，目录b以及目录c，写入tar包
    	err := tarPrefix(prefix, tw)
    	if err != nil {
    		return []byte{}, err
    	}
      	//imageTar的主要作用是对镜像进行打包，不过需要删除和增加一些文件在tar包中
    	err = imageTar(image, prefix, tw, trust, pull)
    	if err != nil {
    		return []byte{}, err
    	}
    	err = tw.Close()
    	if err != nil {
    		return []byte{}, err
    	}
      	//返回tar包字节数组
    	return out.Bytes(), nil
    ```

- 下面对`ImageExtract`函数中`imageTar(image, prefix string, tw *tar.Writer, trust bool, pull bool)`函数进行分析，该函数主要功能就是对镜像文件镜像打包，并且在打包过程中需要删除和增加一些文件

  - ```go
    // This uses Docker to convert a Docker image into a tarball. It would be an improvement if we
    // used the containerd libraries to do this instead locally direct from a local image
    // cache as it would be much simpler.

    //tar包中需要被剔除的文件
    var exclude = map[string]bool{
    	".dockerenv":  true,
    	"Dockerfile":  true,
    	"dev/console": true,
    	"dev/pts":     true,
    	"dev/shm":     true,
    }
    //tar包中需要增加的文件内容
    var replace = map[string]string{
    	"etc/hosts": `127.0.0.1       localhost
    ::1     localhost ip6-localhost ip6-loopback
    fe00::0 ip6-localnet
    ff00::0 ip6-mcastprefix
    ff02::1 ip6-allnodes
    ff02::2 ip6-allrouters
    `,
    	"etc/resolv.conf": `nameserver 8.8.8.8
    nameserver 8.8.4.4
    nameserver 2001:4860:4860::8888
    nameserver 2001:4860:4860::8844
    `,
    	"etc/hostname": "moby",
    }

    func imageTar(image, prefix string, tw *tar.Writer, trust bool, pull bool) error {
    	log.Debugf("image tar: %s %s", image, prefix)
    	if prefix != "" && prefix[len(prefix)-1] != byte('/') {
    		return fmt.Errorf("prefix does not end with /: %s", prefix)
    	}
    	//如果需要pull或者是在trust列表中，则从dockerhub上将镜像pull下来
    	if pull || trust {
    		log.Infof("Pull image: %s", image)
    		err := dockerPull(image, trust)
    		if err != nil {
    			return fmt.Errorf("Could not pull image %s: %v", image, err)
    		}
    	}
      	//创建使用该镜像的容器
    	container, err := dockerCreate(image)
    	if err != nil {
    		// if the image wasn't found, pull it down.  Bail on other errors.
          	// 镜像没找到
    		if client.IsErrNotFound(err) {
    			log.Infof("Pull image: %s", image)
              	//再次从dockerhub上pull
    			err := dockerPull(image, trust)
    			if err != nil {
    				return fmt.Errorf("Could not pull image %s: %v", image, err)
    			}
    			container, err = dockerCreate(image)
    			if err != nil {
    				return fmt.Errorf("Failed to docker create image %s: %v", image, err)
    			}
    		} else {
    			return fmt.Errorf("Failed to create docker image %s: %v", image, err)
    		}
    	}
      	//将镜像内容导出到contents字节数组中
    	contents, err := dockerExport(container)
    	if err != nil {
    		return fmt.Errorf("Failed to docker export container from container %s: %v", container, err)
    	}
      	//删除这个临时的container
    	err = dockerRm(container)
    	if err != nil {
    		return fmt.Errorf("Failed to docker rm container %s: %v", container, err)
    	}

    	// now we need to filter out some files from the resulting tar archive
    	// 需要过滤镜像的一些内容以及增加一些文件
    	r := bytes.NewReader(contents)
    	tr := tar.NewReader(r)

    	for {
          	//每个文件头
    		hdr, err := tr.Next()
    		if err == io.EOF {
    			break
    		}
    		if err != nil {
    			return err
    		}
          	//包含某个文件名的需要从tar包中被剔除
    		if exclude[hdr.Name] {
    			log.Debugf("image tar: %s %s exclude %s", image, prefix, hdr.Name)
    			_, err = io.Copy(ioutil.Discard, tr)
    			if err != nil {
    				return err
    			}
    		} else if replace[hdr.Name] != "" {
              	//tar包中需要进行增加的文件
    			contents := replace[hdr.Name]
    			hdr.Size = int64(len(contents))
    			hdr.Name = prefix + hdr.Name
    			log.Debugf("image tar: %s %s add %s", image, prefix, hdr.Name)
              	//写文件头内容到tw对应的缓冲区中
    			if err := tw.WriteHeader(hdr); err != nil {
    				return err
    			}
    			buf := bytes.NewBufferString(contents)
              	//写到tw对应的缓冲区中
    			_, err = io.Copy(tw, buf)
    			if err != nil {
    				return err
    			}
    			_, err = io.Copy(ioutil.Discard, tr)
    			if err != nil {
    				return err
    			}
    		} else {
              	//复制tar中既不需要删除也不需要增加的文件到tw对应的缓冲区中
    			log.Debugf("image tar: %s %s add %s", image, prefix, hdr.Name)
    			hdr.Name = prefix + hdr.Name
    			if err := tw.WriteHeader(hdr); err != nil {
    				return err
    			}
    			_, err = io.Copy(tw, tr)
    			if err != nil {
    				return err
    			}
    		}
    	}
    	err = tw.Close()
    	if err != nil {
    		return err
    	}
    	return nil
    }
    ```

- 接下来对`buildInternal`函数中的`untarKernel(buf *bytes.Buffer, kernelName, kernelAltName, ktarName string, cmdline string) `函数进行分析，该函数的主要功能是将buf缓冲区的文件分别提取到名为`kernel(bzImage)`和`kernel.tar`的缓冲区，该函数的实现位于`build.go`文件中，下面来看具体的实现细节

  - ```go
    func untarKernel(buf *bytes.Buffer, kernelName, kernelAltName, ktarName string, cmdline string) (*bytes.Buffer, *bytes.Buffer, error) {
    	tr := tar.NewReader(buf)

    	var kernel, ktar *bytes.Buffer
    	foundKernel := false

    	for {
          	//读取文件头
    		hdr, err := tr.Next()
    		if err == io.EOF {
    			break
    		}
    		if err != nil {
    			log.Fatalln(err)
    		}
    		switch hdr.Name {
            //如果文件名为kernel或者bzImage
    		case kernelName, kernelAltName:
    			if foundKernel {
    				return nil, nil, errors.New("found more than one possible kernel image")
    			}
    			foundKernel = true
    			kernel = new(bytes.Buffer)
    			// make a new tarball with kernel in /boot/kernel
              	// 在/boot目录下创建kernel的tar包
              	// 首先创建boot目录
    			tw := tar.NewWriter(kernel)
    			whdr := &tar.Header{
    				Name:     "boot",
    				Mode:     0700,
    				Typeflag: tar.TypeDir,
    			}
    			if err := tw.WriteHeader(whdr); err != nil {
    				return nil, nil, err
    			}
              	//在boot目录下，再创建kernel文件
    			whdr = &tar.Header{
    				Name: "boot/kernel",
    				Mode: hdr.Mode,
    				Size: hdr.Size,
    			}
    			if err := tw.WriteHeader(whdr); err != nil {
    				return nil, nil, err
    			}
    			_, err = io.Copy(tw, tr)
    			if err != nil {
    				return nil, nil, err
    			}
    			// add the cmdline in /boot/cmdline
              	//将文件cmdline内容添加到/boot/cmdline中
    			whdr = &tar.Header{
    				Name: "boot/cmdline",
    				Mode: 0700,
    				Size: int64(len(cmdline)),
    			}
    			if err := tw.WriteHeader(whdr); err != nil {
    				return nil, nil, err
    			}
    			buf := bytes.NewBufferString(cmdline)
    			_, err = io.Copy(tw, buf)
    			if err != nil {
    				return nil, nil, err
    			}
    			if err := tw.Close(); err != nil {
    				return nil, nil, err
    			}
    		case ktarName:
              	//如果文件名为kernel.tar
    			ktar = new(bytes.Buffer)
    			_, err := io.Copy(ktar, tr)
    			if err != nil {
    				return nil, nil, err
    			}
    		default:
    			continue
    		}
    	}

    	if kernel == nil {
    		return nil, nil, errors.New("did not find kernel in kernel image")
    	}
    	if ktar == nil {
    		return nil, nil, errors.New("did not find kernel.tar in kernel image")
    	}

    	return kernel, ktar, nil
    }
    ```

- 将tar包内容读取到相应的缓冲区之后，在将该缓冲区的内容追加到最终输出镜像的缓冲区，`initrdAppend(iw *tar.Writer, r io.Reader)`正干了这件事

  - ```go
    func initrdAppend(iw *tar.Writer, r io.Reader) {
    	tr := tar.NewReader(r)
    	for {
    		hdr, err := tr.Next()
    		if err == io.EOF {
    			break
    		}
    		if err != nil {
    			log.Fatalln(err)
    		}
    		err = iw.WriteHeader(hdr)
    		if err != nil {
    			log.Fatalln(err)
    		}
    		_, err = io.Copy(iw, tr)
    		if err != nil {
    			log.Fatalln(err)
    		}
    	}
    }
    ```

- 由于Onboot以及Services的镜像都需要转化为OCI格式，并由runc来执行，因此不同于Init部分镜像的处理，通过`ConfigToOCI(image *MobyImage)`将镜像转化为OCI格式，然后通过`ImageBundle(path string, image string, config []byte, trust bool, pull bool)`函数在指定路径`path`下处理OCI格式的`rootfs`tar包以及`config.json`tar包，`ImageBundle`的实现位于`image.go`

  - ```go
    // ImageBundle produces an OCI bundle at the given path in a tarball, given an image and a config.json
    func ImageBundle(path string, image string, config []byte, trust bool, pull bool) ([]byte, error) {
    	log.Debugf("image bundle: %s %s cfg: %s", path, image, string(config))
    	out := new(bytes.Buffer)
    	tw := tar.NewWriter(out)
      	// tarPrefix creates the leading directories for a path
      	//将path对应的目录写到tw对应的缓冲区中
    	err := tarPrefix(path+"/rootfs/", tw)
    	if err != nil {
    		return []byte{}, err
    	}
      	//将config.json写到tw对应的缓冲区中
    	hdr := &tar.Header{
    		Name: path + "/" + "config.json",
    		Mode: 0644,
    		Size: int64(len(config)),
    	}
    	err = tw.WriteHeader(hdr)
    	if err != nil {
    		return []byte{}, err
    	}
    	buf := bytes.NewBuffer(config)
      	//写config文件到tw中
    	_, err = io.Copy(tw, buf)
    	if err != nil {
    		return []byte{}, err
    	}
      	//将rootfs下的image镜像tar包写入到tw对应的缓冲区
    	err = imageTar(image, path+"/rootfs/", tw, trust, pull)
    	if err != nil {
    		return []byte{}, err
    	}
    	err = tw.Close()
    	if err != nil {
    		return []byte{}, err
    	}
    	return out.Bytes(), nil
    }
    ```

- 最后来看一下`outputs(m *Moby, base string, image []byte)`函数，其主要作用就是把缓冲区image的内容生成最终要求的格式的镜像，其实现位于`output.go`文件

  - ```go
    func outputs(m *Moby, base string, image []byte) error {
    	log.Debugf("output: %s %s", m.Outputs, base)

    	for _, o := range m.Outputs {
    		switch o.Format {
    		case "tar":
    			err := outputTar(base, image)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "kernel+initrd":
    			kernel, initrd, cmdline, err := tarToInitrd(image)
    			if err != nil {
    				return fmt.Errorf("Error converting to initrd: %v", err)
    			}
    			err = outputKernelInitrd(base, kernel, initrd, cmdline)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "iso-bios":
    			kernel, initrd, cmdline, err := tarToInitrd(image)
    			if err != nil {
    				return fmt.Errorf("Error converting to initrd: %v", err)
    			}
    			err = outputISO(bios, base+".iso", kernel, initrd, cmdline)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "iso-efi":
    			kernel, initrd, cmdline, err := tarToInitrd(image)
    			if err != nil {
    				return fmt.Errorf("Error converting to initrd: %v", err)
    			}
    			err = outputISO(efi, base+"-efi.iso", kernel, initrd, cmdline)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "gcp-img":
    			kernel, initrd, cmdline, err := tarToInitrd(image)
    			if err != nil {
    				return fmt.Errorf("Error converting to initrd: %v", err)
    			}
    			err = outputImg(gcp, base+".img.tar.gz", kernel, initrd, cmdline)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "qcow", "qcow2":
    			kernel, initrd, cmdline, err := tarToInitrd(image)
    			if err != nil {
    				return fmt.Errorf("Error converting to initrd: %v", err)
    			}
    			err = outputImg(qcow, base+".qcow2", kernel, initrd, cmdline)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "vhd":
    			kernel, initrd, cmdline, err := tarToInitrd(image)
    			if err != nil {
    				return fmt.Errorf("Error converting to initrd: %v", err)
    			}
    			err = outputImg(vhd, base+".vhd", kernel, initrd, cmdline)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "vmdk":
    			kernel, initrd, cmdline, err := tarToInitrd(image)
    			if err != nil {
    				return fmt.Errorf("Error converting to initrd: %v", err)
    			}
    			err = outputImg(vmdk, base+".vmdk", kernel, initrd, cmdline)
    			if err != nil {
    				return fmt.Errorf("Error writing %s output: %v", o.Format, err)
    			}
    		case "":
    			return fmt.Errorf("No format specified for output")
    		default:
    			return fmt.Errorf("Unknown output type %s", o.Format)
    		}
    	}
    	return nil
    }
    ```