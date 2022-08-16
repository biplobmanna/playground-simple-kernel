# Writing a simple Linux Kernel Module

## 1. Setup a virtualization software

For this purpose, I will go with **QEMU** since this is something I have wished to try out for some time now.

### 1.1. Installing QEMU/KVM on Kubuntu 22.04

```bash
# install packages required for qemu & kvm
sudo apt install qemu qemu-kvm libvirt-clients libvirt-daemon-system bridge-utils virtinst libvirt-daemon
# install the gui
sudo apt install virt-manager
```

### 1.2. Give permission to current user to run virtualization

By default, the current user does not have permission to run the virtualization, so we need to add the current user to the `libvirt` group:

```bash
# adding the user
sudo usermod -aG libvirt $(whoami)
# check if the user is added
sudo getent group | grep libvirt
# or, using the below
sudo groups $(whoami)
```

On top of that, we also need to uncomment out some configuration lines in `/etc/libvirt/libvirtd.conf`

```bash
sudo vim /etc/libvirt/libvirtd.conf
```

Uncomment the following lines:

```vim
unix_sock_group = "libvirt"

unix_sock_ro_perms = "0777"

unix_sock_rw_perms = "0770"
```

> NOTE: <br>There might not be a need to do the above step of altering the default config of _libvirtd._ <br>It's better to test whether the system works before doing this step:<br> `virsh -c qemu:///system` <br>and if that gives an error, do a system _reboot_ and try again. <br>Only after these steps the error still persists, then alter the config file.

Then, _restart_ and _enable_ `libvirtd` daemon.

```bash
# check the status of libvirtd
sudo systemctl status libvirtd
# if not running, start the daemon
sudo systemctl start libvirtd
# if already running, restart the daemon
sudo systemctl restart libvirtd
# enable libvirtd to start on boot
sudo systemctl enable libvirtd
```

### 1.3. Create a VM for Lubuntu-20.04

Since I'm going to be running the VM in my laptop, which runs Kubuntu-20.04, I require something lightweight. So, **_Lubuntu-20.04_** is chosen.

#### 1.3.1. Create a OS pool

The **_lubuntu-20.04_** ISO is inside `./OS/` folder in the project root. All contents have been excluded in `.gitignore`

#### 1.3.2. Create a storage pool

In the extra HD (1TB), the storage pool is created, which contains a virtual hard-drive of 20GiB.

#### 1.3.3. Setting up Networking

Since this is going to be a development environment, a `bridged` network is a must. <br>
So, let's setup a _bridged_ network interface.

Using the **_Kubuntu Network Manager_** which already had a default _bridged network_ created `virbr0`

Using that in _virt-manager_ as the default network.

#### 1.3.4. Installing Lubuntu-20.04

Installing Lubuntu without any other hassle. <br>
> System: <br>CPU: 2 cores <br>RAM: 3072 MB <br>HD: 20GB <br>Network: bridged (virbr0)

#### 1.3.5. Setting up SSH (host -> guest)

Upgrade the host system & install `openssh-server`

```bash
# update & upgrade
sudo apt update && sudo apt upgrade -y
# install openssh-server
sudo apt install openssh-server -y
# restart the firewall
sudo systemctl restart ufw
# check the status of ufw
sudo systemctl status ufw
```

Find the IP of the _guest_ and use that to SSH into from the _host_

```bash
# find the IP of the host
hostname -I
```

## 2. Setup Development environment

### 2.1. Setup git

Setting up git & SSH connection to my Github account. <br>

1. [Generating a new SSH key and adding it to the ssh-agent](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent) <br>
2. [Adding a new SSH key to your GitHub account](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account)

### 2.2. Stup VIM using my desktop configuration

In my host machine I use Neovim, but here Vim will work. <br>
Setting up Vim the same as my host machine.

### 2.3. Install the development environment

Install the essential development tools & kernel header necessary for this to work.

```bash
sudo apt install build-essential linux-headers-`uname -r`
```

## 3. Playing around with a simple kernel module

## 3.1. First kernel module

Just a small _hello-world_ kernel module. Nothing really fancy, but so much learning here:

* build is done with the kernel build files
* so many different intermediate files are created `.o`, `.ko`, `.mod.o`, etc.. among others
* `Makefile` is a necessity for build
* `obj-m +=` part determines which files get build into `.ko` files
* `test1` directive contains the _bash_ script code to insert, remove to/from the kernel, & view the logs
* By default `printk` does not flush the contents of the buffer into log; used `\n` to flush.
* `dmesg` flushes it's full content into stdout, so it's better to pipe it through `tail`
* insert any module taints the kernel

Small demonstration of `lkm_01` module: <br>
![Linux Kernel Module 01 demonstration](/assets/gifs/playground-simple-linux-kernel-lkm_01.gif "linux kernel module 01 demostraion")

## 3.2. Second kernel module

This one goes a little bit deeper. The aim here is to create a device file, and read/write to it. Other applications in userspace can then read from the device file.

Functions used:

```c
// Prototypes for device functions
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
```

* `device_write` second parameter is `const char *`
* device is registered with a `major_num`
* reding from the device will read the message in a loop
* writing to device is not permitted
* `test2` directive contains the _bash_ script code to insert, remove to/from the kernel, & view the logs
* running `make test2` will give the **MAJOR** number, which is required to create the device
* create character device using `sudo mknod /dev/lkm_02 c MAJOR 0`
* grab the content: `dd if=/dev/lkm_02 of=test bs=14 count=10` or using `cat` or any other reader
* remove all traces once done: `make remove`

Small demonstration of `lkm_02` module: <br>
![Linux Kernel Module 02 demonstration](/assets/gifs/playground-simple-linux-kernel-lkm_02.gif "linux kernel module 02 demonstration")
