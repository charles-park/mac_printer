# mac_printer
odroid mac address printer

### 준비사항
ODROID-M1S, MIPI 5 Inch LCD (ODROID VU5S)

### 사용 이미지
https://dn.odroid.com/RK3566/ODROID-M1S/Ubuntu/ubuntu-20.04-server-odroidm1s-20231030.img.xz

### 설치순서
```
// ubuntu system upgrade
root@server:/home/odroid# apt update && apt upgrade -y

// ubuntu package
root@server:~# apt install samba ssh build-essential python3 python3-pip ethtool net-tools usbutils git i2c-tools vim cups cups-bsd overlayroot nmap

// python3 package
root@server:~# pip install aiohttp asyncio
```

### LCD(VU5S) 설정
* https://wiki.odroid.com/odroid-m1s/application_note/vu5s 참조
* 2024/01/09 SDCard에서는 동작하지 않음 (bootloader수정 또는 bootloader에서 setenv panel display_vu5s하여 저장 후 사용)

### ssh root user설정
```
// root user add
root@server:~# passwd root
root@server:~# vi /etc/ssh/sshd_config
...
# PermitRootLogin prohibit-password
PermitRootLogin yes
StrictModes yes
PubkeyAuthentication yes
...

root@server:~# service ssh restart

```

### auto login
```
root@server:~# systemctl edit getty@tty1.service
...
[Service]
ExecStart=
ExecStart=-/sbin/agetty --noissue --autologin root %I $TERM
Type=idle

```

### samba config
```
root@server:~# smbpasswd -a root
root@server:~# vi /etc/samba/smb.conf
...
[odroidm1s]
   comment = odroid-m1s jig root
   path = /root
   guest ok = no
   browseable = no
   writable = yes
   create mask = 0755
   directory mask = 0755
...
root@server:~# service smbd restart
```

### overlayroot
```
root@server:~# update-initramfs -c -k $(uname -r)
Using DTB: rockchip/rk3566-odroid-m1s.dtb
Installing rockchip into /boot/dtbs/5.10.0-odroid-arm64/rockchip/
Installing rockchip into /boot/dtbs/5.10.0-odroid-arm64/rockchip/
flash-kernel: installing version 5.10.0-odroid-arm64
Generating boot script u-boot image... done.
Taking backup of boot.scr.
Installing new boot.scr.

root@server:~# mkimage -A arm64 -O linux -T ramdisk -C none -a 0 -e 0 -n uInitrd -d /boot/initrd.img-$(uname -r) /boot/uInitrd 
Image Name:   uInitrd
Created:      Fri Oct 27 04:27:58 2023
Image Type:   AArch64 Linux RAMDisk Image (uncompressed)
Data Size:    7805996 Bytes = 7623.04 KiB = 7.44 MiB
Load Address: 00000000
Entry Point:  00000000

// Change overlayroot value "" to "tmpfs" for overlayroot enable
root@server:~# vi /etc/overlayroot.conf
...
overlayroot_cfgdisk="disabled"
overlayroot="tmpfs"
```

### 사용 submodule
```
```



  

