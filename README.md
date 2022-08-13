# Writing a simple Linux Kernel Module

## 1. Installing a Virtualization Software
For this purpose, I will go with **QEMU** since this is something I have wished to try out for some time now.

### 1.1. Installing QEMU on Kubuntu 22.04
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


